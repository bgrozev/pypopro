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
#ifndef _JIPOPRO_SCALE_H
#define _JIPOPRO_SCALE_H

#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

struct PypoproScaler
{
    AVFrame *outputFrame;

    int in_w;
    int in_h;
    int out_w;
    int out_h;
    struct SwsContext *sws;
};

typedef struct PypoproScaler PypoproScaler;

PypoproScaler *pypopro_scaler_init(int in_w, int in_h, int out_w, int out_h);
void pypopro_scaler_close(PypoproScaler *scaler);
AVFrame *pypopro_scaler_scale(PypoproScaler *scaler,
                               AVFrame *frame);

#endif
