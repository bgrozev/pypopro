#!/usr/bin/env python3

import sys
import os
import json
import time


def main():
    if len(sys.argv) < 2:
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

    do_audio(events.get('audio'))
    add_timing('Audio complete')

    do_video(events.get('video'))
    add_timing('Video complete')

    #merge audio and video

    print()
    for s in timing_strings:
        print(s)


def do_audio(events):
    normalize_instants(events)
    os.system('mkdir -p {}audio_tmp'.format(output_dir))
    mix_command = 'sox --combine mix-power'
    for e in events:
        decode_audio(input_dir + e['filename'],
                     output_dir + 'audio_tmp/' + e['filename'] + '.wav',
                     e['instant'])
        mix_command += ' {}.wav'.format(
            output_dir + 'audio_tmp/' + e['filename'])
    mix_command += ' {}out.wav'.format(output_dir)
    add_timing('Audio padded')
    os.system(mix_command)
    add_timing('Audio mixed')
    os.system('rm -rf {}audio_tmp'.format(output_dir))


def do_video(events):
    events = preprocess_video_events(events)
    add_timing('Video events pre-processed (extracted durations)')

    last_instant = events[-1]['instant']
    index = -1
    #TODO: refactor participants in its own class
    participants = set()
    active = -1
    for ms in range(0, last_instant, config['frameDuration']):
        #read events in (0, ms]
        added = False
        while events[index + 1]['instant'] <= ms:
            event = events[index + 1]
            if debug:
                print('Event {} at {} handled at ms={}'.format(event['type'],
                                                               event['instant'],
                                                               ms))
            event_type = event['type']
            if event_type == 'RECORDING_STARTED':
                participants.add(event['ssrc'])
                if active == -1:
                    active = event['ssrc']
            elif event_type == 'RECORDING_ENDED':
                participants.remove(event['ssrc'])
                if active == event['ssrc']:
                    active = next(iter(participants)) if participants else -1
            elif event_type == 'SPEAKER_CHANGED':
                active = event['ssrc']

            index += 1
            added = True

        if debug and added:
            print('ms={} participants={} active={}'.format(ms, participants,
                                                           active))


def preprocess_video_events(events):
    #remove SPEAKER_CHANGED events from the beginning
    for i in range(len(events)):
        if events[i]['type'] == 'SPEAKER_CHANGED':
            del events[i]
        else:
            break

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
    return int(proc.read())


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


#TODO stats to save: num frames, duration, time
last_time = int(round(time.time() * 1000))
first_time = last_time
timing_strings = []
input_dir = None
output_dir = None
config = None
debug = None

main()
