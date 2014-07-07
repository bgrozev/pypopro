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
