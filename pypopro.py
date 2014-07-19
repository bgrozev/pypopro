#!/usr/bin/env python3
"""
Copyright (C) 2014 Boris Grozev <boris@jitsi.org>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
"""

import sys
import os
import json
import time

import pypopro
import Participants


def main():
    if len(sys.argv) < 3:
        print("Usage: {} <input_dir> <output_dir>".format(sys.argv[0]))
        return

    #read args
    global input_dir, output_dir, config, debug
    input_dir = sys.argv[1]
    if input_dir[-1] != '/':
        input_dir += '/'
    if not os.path.isdir(input_dir):
        print(
            'Input directory {} does not exists (or is not a directory)'.format(
                input_dir))
        return
    output_dir = sys.argv[2]
    if output_dir[-1] != '/':
        output_dir += '/'
    os.makedirs(output_dir, mode=0o755, exist_ok=True)
    print("input_dir={}\noutput_dir={}".format(input_dir, output_dir))

    #load configuration
    config = json.load(open('config.json'))
    config['frameDuration'] = int(1000 / config['outputFramerate'])
    print("Configuration: ", config)
    print()

    debug = parse_bool(config.get('debug'))

    #read metadata
    events = json.load(open(input_dir + config['metadataFilename']))

    if config['processAudio']:
        #get this before we change the events' instants
        audio_start = events.get('audio')[0]['instant']

        audio_file = do_audio(events.get('audio'))
        add_timing('Audio complete')

    if config['processVideo']:
        #get this before we change the events' instants
        video_start = events.get('video')[0]['instant']

        video_file = do_video(events.get('video'))
        add_timing('Video complete')

    if config['processAudio'] and config['processVideo']:
        #merge audio and video
        audio_offset, video_offset = 0, 0
        if (audio_start > video_start):
            audio_offset = audio_start - video_start
        else:
            video_offset = video_start - audio_start
        print('audio start ', audio_start, ' video start ', video_start)
        print('audio offset ', audio_offset, ' video offset ', video_offset)
        merge(audio_file, audio_offset,
              video_file, video_offset,
              output_dir + config['outputFilename'])
        add_timing('Merge audio and video')

    print()
    for s in timing_strings:
        print(s)


def do_audio(events):
    normalize_instants(events)
    os.system('mkdir -p {}audio_tmp'.format(output_dir))
    mix_command = 'sox --combine mix-power'
    for e in events:
        #TODO: check if file exists and it's min length
        in_file = input_dir + e['filename'];
        if not os.path.isfile(in_file) or os.stat(in_file).st_size < 10000:
            print('Skipping audio file', in_file,
                  'because it does not exist or is too short.')
            continue

        decode_audio(in_file,
                     output_dir + 'audio_tmp/' + e['filename'] + '.wav',
                     e['instant'])
        mix_command += ' {}.wav'.format(
            output_dir + 'audio_tmp/' + e['filename'])
    filename = output_dir + 'out.wav'
    mix_command += ' ' + filename
    add_timing('Audio padded')
    os.system(mix_command)
    add_timing('Audio mixed')
    os.system('rm -rf {}audio_tmp'.format(output_dir))

    return filename


def do_video(events):
    events = preprocess_video_events(events)
    add_timing('Video events pre-processed (extracted durations)')

    if debug:
        print('Pre-processed video events:')
        for e in events:
            print(e)

    last_instant = events[-1]['instant']
    index = -1

    participants = Participants.Participants(config, input_dir)

    overlayer = pypopro.overlayer_init()
    filename = output_dir + config['outputFilename'] + '.ivf'
    encoder = pypopro.encoder_init(filename)

    print()
    for ms in range(0, last_instant, config['frameDuration']):
        #read events in (0, ms] and update participants
        added = False
        while events[index + 1]['instant'] <= ms:
            event = events[index + 1]
            if debug:
                print('\nEvent {} at {} handled at ms={}'.format(event['type'],
                                                               event['instant'],
                                                               ms))
            participants.add_event(event)

            index += 1
            added = True

        #        if debug and added:
        #            print('ms={} participants={} active={}'.format(ms, participants,
        #                                                           active))

        if not participants.active:
            print('\nno active speaker left, ending at ms=', ms)
            participants.print_stats()
            break
        if 0 < config['maxVideoDurationMs'] < ms:
            break

        #read frames for all participants and overlay them
        f, w, h, x, y = participants.get_frames(ms)
        #print("overlay: " + str(overlayer) + ' ' + str(f) + "; " + str(
        #    w) + "; " + str(h) + '; ' + str(x) + '; ' + str(y))

        #overlay currently disabled, just use the 'active' frame
        #frame = pypopro.overlayer_overlay(overlayer,
        #                                  f, w, h, x, y)
        frame = f[0]

        #encode the frame and write it to disk
        #print('encode: ' + str(encoder) + " " + str(frame) + " " + str(ms))
        pypopro.encoder_add_frame(encoder, frame, ms)
        participants.frames_encoded += 1
        print('\rProcessing {} / {}ms'.format(str(ms).zfill(len(str(last_instant))),
                                              last_instant),
              end='')

    participants.print_stats()
    pypopro.encoder_close(encoder)
    pypopro.overlayer_close(overlayer)

    return filename


def merge(audio_file, audio_offset, video_file, video_offset, output_file):
    print(
        'Merging {}({}) and {}({})'.format(audio_file, audio_offset, video_file,
                                           video_offset))
    os.system(
        'ffmpeg -y -itsoffset {} -i {} -itsoffset {} -i {} -vcodec copy {}'.format(
            audio_offset, audio_file, video_offset, video_file, output_file))


def preprocess_video_events(events):
    #remove SPEAKER_CHANGED events from the beginning
    for i in range(len(events)):
        if events[i]['type'] == 'SPEAKER_CHANGED':
            del events[i]
        else:
            break

    events = [i for i in events if i['type'] != 'RECORDING_ENDED']

    #insert RECORDING_ENDED events
    #TODO: cleaner syntax for this?
    ended = []
    for event in events:
        if event['type'] == 'RECORDING_STARTED':
            e = dict(filename=event['filename'],
                     mediaType='video',
                     type='RECORDING_ENDED',
                     ssrc=event['ssrc'],
                     instant=event['instant'] + get_duration(
                         input_dir + event['filename']))
            ended.append(e)

    events += ended

    events.sort(key=lambda x: x['instant'])

    return normalize_instants(events)


def normalize_instants(events):
    """
        Decreases the 'instance' field of each element of 'events' by the
         'instance' of the first event (making the first event's instance '0').
        Assumes 'events' is ordered by 'instance'.
    """
    assert events
    first_instant = events[0]['instant']
    for e in events:
        e['instant'] -= first_instant
    return events


def get_duration(filename):
    """
        Gets the duration of the webm file 'filename' in milliseconds.
    """
    proc = os.popen(
        'mkvinfo -s -v ' + filename + " | tail -n 1 | awk '{print $6;}'")
    return2 = int(proc.read())

    print('duration {} {}'.format(filename, return2))
    return return2


def decode_audio(in_name, out_name, padding):
    """
        Converts 'in_name' to 'out_name' using 'sox', adding 'padding'
         milliseconds of padding in the beginning.
    """
    os.system("sox {} {} pad {} > /dev/null 2>&1".format(in_name,
                                                         out_name,
                                                         millis_to_seconds(
                                                             padding)))


def millis_to_seconds(millis):
    """
        Converts 'millis' to seconds and return a string representation with
        three digits after the decimal point.
    """
    return "{0:.3f}".format(millis / 1000)


def add_timing(s):
    global last_time, timing_strings
    cur = int(round(time.time() * 1000))
    timing_strings.append(
        "[TIME] {}s (total {}s): {}".format(millis_to_seconds(cur - last_time),
                                            millis_to_seconds(cur - first_time),
                                            s))
    last_time = cur


def parse_bool(v):
    if v is None:
        return False
    return str(v).lower() in ("yes", "true", "t", "1")


last_time = int(round(time.time() * 1000))
first_time = last_time
timing_strings = []
input_dir = None
output_dir = None
config = None
debug = None

main()
