#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

struct PypoproOverlayer
{
    AVFrame *outputFrame;

    //temp
    int inW;
    int inH;
    int outW;
    int outH;
    struct SwsContext *sws;
};

typedef struct PypoproOverlayer PypoproOverlayer;

PypoproOverlayer *pypopro_overlayer_init();
void pypopro_overlayer_close(PypoproOverlayer *overlayer);
AVFrame *pypopro_overlayer_overlay(PypoproOverlayer *overlayer,
                               AVFrame *frames[],
                               int widths[],
                               int heights[],
                               int posX[],
                               int posY[],
                               int n);
