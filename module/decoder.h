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
#ifndef _JIPOPRO_DECODER_H_
#define _JIPOPRO_DECODER_H_

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

struct PypoproDecoder
{
    AVFormatContext *formatCtx;
    AVCodecContext  *codecCtx;
    AVCodec         *codec;

    AVPacket *currentPacket;
    AVPacket *peekPacket;

    AVFrame *frame;

    int64_t last_read_pts;
};

typedef struct PypoproDecoder PypoproDecoder;
PypoproDecoder *pypopro_decoder_init(const char *filename);
AVFrame *pypopro_decoder_read(PypoproDecoder* decoder, int64_t pts);
void pypopro_decoder_close(PypoproDecoder *decoder);

#endif
