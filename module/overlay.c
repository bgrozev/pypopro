#include "libavcodec/avcodec.h"
#include "overlay.h"
#include "pypopro.h"

PypoproOverlayer *pypopro_overlayer_init()
{
    //av_register_all();
    PypoproOverlayer *overlayer = (PypoproOverlayer *) malloc(sizeof(PypoproOverlayer));

    overlayer->inW = -1;
    overlayer->inH = -1;
    overlayer->outW = -1;
    overlayer->outH = -1;
    overlayer->outputFrame = avcodec_alloc_frame();

    return overlayer;
}

void pypopro_overlayer_close(PypoproOverlayer *overlayer)
{
    //free outputFrame
    free(overlayer);
}

struct SwsContext *getSws(int inW, int inH, int outW, int outH)
{
    printf("New sws %dx%d -> %dx%d\n", inW, inH, outW, outH);
    return sws_getContext(inW,
                          inH,
                          PIX_FMT_YUV420P,
                          outW,
                          outH,
                          PIX_FMT_YUV420P,
                          0,
                          NULL,
                          NULL,
                          NULL);
}

AVFrame *pypopro_overlayer_overlay(PypoproOverlayer *overlayer,
                               AVFrame *frames[],
                               int widths[],
                               int heights[],
                               int posX[],
                               int posY[],
                               int n)
{
    AVFrame *inFrame = frames[0];
    int inW = inFrame->width;
    int inH = inFrame->height;

    int outW = OUT_WIDTH;
    int outH = OUT_HEIGHT;

    printf("overlay: scaling from %dx%d -> %dx%d\n", inW, inH, outW, outH);

    if ( (inW != overlayer->inW)
         || (inH != overlayer->inH)
         || (outW != overlayer->outW)
         || (outH != overlayer->outH)
         || overlayer->sws == NULL)
    {
        overlayer->sws = getSws(inW, inH, outW, outH);
        if (overlayer->sws == NULL)
        {
            printf("Failed to get sws");
            return NULL;
        }
        overlayer->inW = inW;
        overlayer->inH = inH;
        overlayer->outW = outW;
        overlayer->outH = outH;
        
        int s = avpicture_get_size(PIX_FMT_YUV420P,
                                   outW,
                                   outH);
        uint8_t *buffer = (uint8_t *) av_malloc(s * sizeof(uint8_t));
        avpicture_fill((AVPicture *) (overlayer->outputFrame),
                       buffer,
                       PIX_FMT_YUV420P,
                       outW,
                       outH);
    }

    sws_scale(overlayer->sws,
              (const unsigned char * const *) inFrame->data,
              inFrame->linesize,
              0,
              inH,
              overlayer->outputFrame->data,
              overlayer->outputFrame->linesize);

    return overlayer->outputFrame;
}
