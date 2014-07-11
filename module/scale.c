#include "libavcodec/avcodec.h"
#include "scale.h"
#include "pypopro-constants.h"

PypoproScaler *pypopro_scaler_init(int in_w, int in_h, int out_w, int out_h)
{
    //av_register_all();
    PypoproScaler *scaler = (PypoproScaler *) malloc(sizeof(PypoproScaler));

    scaler->in_w = in_w;
    scaler->in_h = in_h;
    scaler->out_w = out_w;
    scaler->out_h = out_h;
    scaler->outputFrame = avcodec_alloc_frame();
    scaler->sws = sws_getContext(in_w,
                                 in_h,
                                 PIX_FMT_YUV420P,
                                 out_w,
                                 out_h,
                                 PIX_FMT_YUV420P,
                                 0,
                                 NULL,
                                 NULL,
                                 NULL);

    return scaler;
}

void pypopro_scaler_close(PypoproScaler *scaler)
{
    //free outputFrame?
    //free sws?
    free(scaler);
}

AVFrame *pypopro_scaler_scale(PypoproScaler *scaler,
                               AVFrame *frame)
{
    int s = avpicture_get_size(PIX_FMT_YUV420P,
                               scaler->out_w,
                               scaler->out_h);
    uint8_t *buffer = (uint8_t *) av_malloc(s * sizeof(uint8_t));
    avpicture_fill((AVPicture *) (scaler->outputFrame),
                   buffer,
                   PIX_FMT_YUV420P,
                   scaler->out_w,
                   scaler->out_h);

    sws_scale(scaler->sws,
              (const unsigned char * const *) frame->data,
              frame->linesize,
              0,
              scaler->in_h,
              scaler->outputFrame->data,
              scaler->outputFrame->linesize);

    return scaler->outputFrame;
}
