/*
 * Copyright (C) 2014 Boris Grozev <boris@jitsi.org>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _JIPOPRO_ENCODER_H_
#define _JIPOPRO_ENCODER_H_

#include "libavcodec/avcodec.h"

struct PypoproEncoder
{
    AVCodec *codec;
    AVCodecContext *codecCtx;
    AVFrame *encodedFrame;
    int buf_size;
    uint8_t *buf;
    FILE *file;
};

typedef struct PypoproEncoder PypoproEncoder;

PypoproEncoder *pypopro_encoder_init(const char *filename);
void pypopro_encoder_add_frame(PypoproEncoder *encoder, AVFrame *frame, int64_t pts);
void pypopro_encoder_close(PypoproEncoder *encoder);

#endif
