#!/usr/bin/env python3

from collections import namedtuple, defaultdict

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
        self.scalers = defaultdict(lambda: None)

        self.frames_decoded = 0
        self.frames_scaled = 0
        self.frames_encoded = 0

    def print_stats(self):
        print('Total frames decoded: ', self.frames_decoded)
        print('Total frames scaled: ', self.frames_scaled)
        print('Total frames encoded: ', self.frames_encoded)

    def get_scaler(self, in_w, in_h, out_w, out_h):
        scaler = self.scalers[in_w, in_h, out_w, out_h]
        if not scaler:
            scaler = pypopro.scaler_init(in_w, in_h, out_w, out_h)
            self.scalers[in_w, in_h, out_w, out_h] = scaler

        return scaler

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
        #print("read from decoder (active) at {}".format(ms-self.active.start))
        frame = pypopro.decoder_read(self.active.decoder,
                                     ms - self.active.start)
        #print("read active frame: " + str(frame))
        self.frames_decoded += 1

        out_w = self.config['outputWidth']
        out_h = self.config['outputHeight']
        frame = self.scale(frame, out_w, out_h)
        #print("scaled active frame: " + str(frame))

        frames.append(frame)
        widths.append(out_w)
        heights.append(out_h)
        pos_x.append(0)
        pos_y.append(0)
        #/active

        x = self.config['outputWidth'] - SMALL_VIDEO_WIDTH
        y = self.config['outputHeight'] - SMALL_VIDEO_HEIGHT
        for p in self.participants:
            if p == self.active:
                continue

            #print("read from decoder (non-active) at {}".format(ms-p.start))
            #frame = pypopro.decoder_read(p.decoder,
            #                             ms - p.start)
            self.frames_decoded += 1
            #frame = self.scale(frame, SMALL_VIDEO_WIDTH, SMALL_VIDEO_HEIGHT)

            frames.append(frame)
            widths.append(SMALL_VIDEO_WIDTH)
            heights.append(SMALL_VIDEO_HEIGHT)
            pos_x.append(x)
            pos_y.append(y)

            x -= SMALL_VIDEO_WIDTH

        return frames, widths, heights, pos_x, pos_y

    def scale(self, frame, out_w, out_h):
        in_w, in_h = pypopro.get_dimensions(frame)
        #print('dims: '+str(in_w)+' '+str(in_h))

        if (in_w, in_h) != (out_w, out_h):
            scaler = self.get_scaler(in_w, in_h, out_w, out_h)
            #print('scaler: '+str(scaler))
            frame = pypopro.scaler_scale(scaler, frame)
            self.frames_scaled += 1

        return frame

    #TODO: make private somehow?
    def find_participant(self, ssrc):
        for p in self.participants:
            if p.ssrc == ssrc:
                return p
        return None

