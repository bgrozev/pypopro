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
