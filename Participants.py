#!/usr/bin/env python3

from collections import namedtuple

import pypopro

Participant = namedtuple("Participant", "ssrc decoder start")
SMALL_VIDEO_WIDTH = 173
SMALL_VIDEO_HEIGHT = 97


class Participants:
    def __init__(self, config, input_dir):
        self.active = None
        self.participants = []
        self.config = config
        self.input_dir = input_dir

    def add_event(self, event):
        event_type = event['type']

        if event_type == 'RECORDING_STARTED':
            decoder = pypopro.decoder_init(self.input_dir + event['filename'])
            participant = Participant(event['ssrc'],
                                      decoder,
                                      event['instant'])
            self.participants.append(participant)
            self.active = participant
        elif event_type == 'RECORDING_ENDED':
            participant = self.find_participant(event['ssrc'])
            if participant:
                self.participants.remove(participant)
                #pypopro.decoder_close(participant.decoder)
                if self.active == participant:
                    self.active = None
                    if self.participants:
                        self.active = self.participants[0]

        elif event_type == 'SPEAKER_CHANGED':
            participant = self.find_participant(event['ssrc'])
            if participant:
                self.active = participant


    def get_frames(self, ms):
        frames = []
        widths = []
        heights = []
        pos_x = []
        pos_y = []

        #active
        print("read from decoder (active) at {}".format(ms-self.active.start))
        frames.append(pypopro.decoder_read(self.active.decoder,
                                           ms - self.active.start))
        widths.append(self.config['outputWidth'])
        heights.append(self.config['outputHeight'])
        pos_x.append(0)
        pos_y.append(0)

        x = self.config['outputWidth'] - SMALL_VIDEO_WIDTH
        y = self.config['outputHeight'] - SMALL_VIDEO_HEIGHT
        for p in self.participants:
            if p == self.active:
                continue

            print("read from decoder (non-active) at {}".format(ms-p.start))
            frames.append(pypopro.decoder_read(p.decoder,
                                               ms - p.start))
            widths.append(SMALL_VIDEO_WIDTH)
            heights.append(SMALL_VIDEO_HEIGHT)
            pos_x.append(x)
            pos_y.append(y)

            x -= SMALL_VIDEO_WIDTH

        return frames, widths, heights, pos_x, pos_y

    #TODO: make private somehow?
    def find_participant(self, ssrc):
        for p in self.participants:
            if p.ssrc == ssrc:
                return p
        return None

