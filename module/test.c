#include "decoder.h"
#include "encoder.h"
#include "overlay.h"

int main(int argc, char **argv)
{
    PypoproDecoder *decoder = pypopro_decoder_init("1.webm");
    if (decoder == NULL)
    {
        printf("no decoder\n");
        return 1;
    }

    PypoproDecoder *decoder2 = pypopro_decoder_init("2.webm");
    if (decoder2 == NULL)
    {
        printf("no decoder2\n");
        return 1;
    }


    PypoproEncoder *encoder = pypopro_encoder_init("out.ivf");
    if (encoder == NULL)
    {
        printf("no encoder\n");
        return 1;
    }

    PypoproOverlayer *overlayer = pypopro_overlayer_init();
    if (encoder == NULL)
    {
        printf("no overlayer\n");
        return 1;
    }


    AVFrame *frames[2];
    AVFrame *outFrame;
    int i = 0;
    int count = 0;
    do
    {
        printf("i=%d\n", i);

        if (count % 40 < 20)
        {
        frames[0] = pypopro_decoder_read(decoder, i);
        frames[1] = pypopro_decoder_read(decoder2, i);
        } else
        {
        frames[1] = pypopro_decoder_read(decoder, i);
        frames[0] = pypopro_decoder_read(decoder2, i);
        }

        outFrame = pypopro_overlayer_overlay(overlayer,
                                             frames,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             0);

        if (outFrame != NULL)
            pypopro_encoder_add_frame(encoder, outFrame, (int64_t) i);
        i+=40;
        count++;
    } while ( i < 20000 && outFrame != NULL);
    printf("encoded %d frames\n", count);

    return 0;
}
