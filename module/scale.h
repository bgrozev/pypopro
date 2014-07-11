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
