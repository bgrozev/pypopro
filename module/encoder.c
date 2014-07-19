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
#include "encoder.h"
#include "ivf.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "pypopro-constants.h"

PypoproEncoder *pypopro_encoder_init(const char *filename)
{
    av_register_all();

    PypoproEncoder *encoder = (PypoproEncoder*) malloc(sizeof(PypoproEncoder));


    encoder->codec = avcodec_find_encoder(CODEC_ID_VP8);
    if (!encoder->codec)
    {
        printf("Failed to find encoder\n");
        free(encoder);
        return NULL;
    }

    encoder->codecCtx = avcodec_alloc_context3(encoder->codec);
    encoder->encodedFrame = avcodec_alloc_frame();

    encoder->codecCtx->bit_rate = ENCODER_BITRATE;
    encoder->codecCtx->width = OUT_WIDTH;
    encoder->codecCtx->height = OUT_HEIGHT;
    encoder->codecCtx->time_base= (AVRational){1,25};
    encoder->codecCtx->pix_fmt = PIX_FMT_YUV420P;

    encoder->codecCtx->thread_count = THREAD_COUNT;

    if (avcodec_open2(encoder->codecCtx, encoder->codec, NULL) < 0)
    {
        printf("encoder: avcodec_open failed\n");
        free(encoder);
        return NULL;
    }

    encoder->buf_size = 300000;
    encoder->buf = malloc(encoder->buf_size);

    encoder->file = fopen(filename, "wb");
    if (!encoder->file)
    {
        printf("failed to open file for encoder: %s\n", filename);
        free(encoder->buf);
        free(encoder);
        return NULL;
    }

    write_ivf_header(encoder->file, OUT_WIDTH, OUT_HEIGHT);

    printf("pypopro_encoder_init finish\n");
    return encoder;
}

void pypopro_encoder_close(PypoproEncoder *encoder)
{
    free(encoder->buf);
    fclose(encoder->file);
    free(encoder);
}

void pypopro_encoder_add_frame(PypoproEncoder *encoder, AVFrame *frame, int64_t pts)
{
    //printf("encoder add_frame pts=%lld\n", pts);
    int out_size = avcodec_encode_video(encoder->codecCtx,
                                        encoder->buf,
                                        encoder->buf_size,
                                        frame);
    //printf("encoder encoded: pts=%lld, out_size=%d\n", pts, out_size);

    write_ivf_frame_header(encoder->file, pts, out_size);
    fwrite(encoder->buf, 1, out_size, encoder->file);
}
