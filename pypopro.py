#!/usr/bin/env python3

import sys, os, json, time

def main():
    if (len(sys.argv) < 2):
        print("Usage: {} <input_dir> <output_dir>".format(sys.argv[0]))
        return

    #read args
    global inputDir, outputDir, config, debug
    inputDir = sys.argv[1]
    if inputDir[-1] != '/':
        inputDir += '/'
    if not os.path.isdir(inputDir):
        print('Input directory {} does not exists (or is not a directory)'.format(inputDir))
        return
    outputDir = sys.argv[2]
    if outputDir[-1] != '/':
        outputDir += '/'
    os.makedirs(outputDir, mode=0o755, exist_ok=True)
    print("inputDir={}\noutputDir={}".format(inputDir, outputDir))

    #load configuration
    config = json.load(open('config.json'))
    config['frameDuration'] = int(1000 / config['outputFramerate'])
    print("Configuration: ", config)
    print()

    debug = parseBool(config.get('debug'))

    #read metadata
    events = json.load(open(inputDir + config['metadataFilename']))

    doAudio(events.get('audio'))
    addTiming('Audio complete')

    doVideo(events.get('video'))
    addTiming('Video complete')

    #merge audio and video

    print()
    for s in timingStrings:
        print(s)

def doAudio(events):
    normalizeInstants(events)
    os.system('mkdir -p {}audio_tmp'.format(outputDir))
    mixCommand = 'sox --combine mix-power'
    for e in events:
        decodeAudio(inputDir + e['filename'],
                    outputDir + 'audio_tmp/' + e['filename'] + '.wav',
                    e['instant'])
        mixCommand += ' {}.wav'.format(outputDir + 'audio_tmp/' + e['filename'])
    mixCommand += ' {}out.wav'.format(outputDir)
    addTiming('Audio padded')
    os.system(mixCommand)
    addTiming('Audio mixed')
    os.system('rm -rf {}audio_tmp'.format(outputDir))

def doVideo(events):
    events = preprocessVideoEvents(events)
    addTiming('Video events pre-processed (extracted durations)')

    lastInstant = events[-1]['instant']
    index = -1 
    #TODO: refactor participants in its own class
    participants = set()
    active = -1
    for ms in range(0, lastInstant, config['frameDuration']):
        #read events in (0, ms]
        added = False
        while events[index + 1]['instant'] <= ms:
            event = events[index + 1]
            if (debug):
                print('Event {} at {} handled at ms={}'.format(event['type'],
                                                               event['instant'],
                                                               ms))
            type = event['type']
            if type == 'RECORDING_STARTED':
                participants.add(event['ssrc'])
                if active == -1:
                    active = event['ssrc']
            elif type == 'RECORDING_ENDED':
                participants.remove(event['ssrc'])
                if active == event['ssrc']:
                    active = next(iter(participants)) if participants else -1
            elif type == 'SPEAKER_CHANGED':
                active = event['ssrc']

            index += 1
            added = True

        if debug and added:
            print('ms={} participants={} active={}'.format(ms, participants, active))

def preprocessVideoEvents(events):
    #remove SPEAKER_CHANGED events from the beginning
    for i in range(len(events)):
        if events[i]['type'] == 'SPEAKER_CHANGED':
            del events[i]
        else:
            break

    #insert RECORDING_ENDED events
    #TODO: cleaner syntax for this?
    ended = [];
    for event in events:
        if event['type'] == 'RECORDING_STARTED':
            e = dict(filename = event['filename'],
                     mediaType = 'video',
                     type = 'RECORDING_ENDED',
                     ssrc = event['ssrc'],
                     instant = event['instant'] + getDuration(inputDir + event['filename']))
            ended.append(e)
    
    events.sort(key = lambda x: x['instant'])

    return normalizeInstants(events)

def normalizeInstants(events):
    firstInstant = events[0]['instant']
    for e in events:
        e['instant'] -= firstInstant
    return events

def getDuration(filename):
    """
        Get duration of the webm file 'filename' in milliseconds.
    """
    proc = os.popen('mkvinfo -s -v ' + filename + " | tail -n 1 | awk '{print $6;}'")
    return int(proc.read())

def decodeAudio(inName, outName, padding):
    ret = os.system("sox {} {} pad {} > /dev/null 2>&1".format(inName,
                                                               outName,
                                                               millisToSeconds(padding)))

def millisToSeconds(millis):
    return str(int(millis/1000)) + "." + str(millis % 1000).zfill(3)

def addTiming(s):
    global lastTime, timingStrings
    cur = int(round(time.time() * 1000))
    timingStrings.append("[TIME] {}s (total {}s): {}".format(millisToSeconds(cur - lastTime),
                                                             millisToSeconds(cur - firstTime),
                                                             s))
    lastTime = cur

def parseBool(v):
  if v is None:
      return False
  return str(v).lower() in ("yes", "true", "t", "1")


#TODO stats to save: num frames, duration, time
lastTime = int(round(time.time() * 1000))
firstTime = lastTime
timingStrings = []
inputDir = None
outputDir = None
config = None
debug = None

main()
