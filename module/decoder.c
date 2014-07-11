#include "decoder.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

PypoproDecoder *pypopro_decoder_init(const char *filename)
{
    av_register_all();

    PypoproDecoder *decoder = (PypoproDecoder *) malloc(sizeof(PypoproDecoder));

    decoder->formatCtx = avformat_alloc_context();

    if (avformat_open_input( &(decoder->formatCtx), filename, 0, NULL) != 0)
    {
        printf("Failed to open file %s:\n", filename);
        free(decoder);
        return NULL;
    }

    decoder->codecCtx = decoder->formatCtx->streams[0]->codec;

    //TODO: make sure the libvpx decoder is used
    decoder->codec = avcodec_find_decoder(decoder->codecCtx->codec_id);

    if (decoder->codec == NULL)
    {
        printf("Codec null for  %s)\n", filename);
        avformat_close_input( &(decoder->formatCtx) );
        avformat_free_context(decoder->formatCtx);
        free(decoder);
        return NULL;
    }

    if (avcodec_open2(decoder->codecCtx, decoder->codec, NULL) < 0 )
    {
        printf("Failed to open codec for %s)\n", filename);
        avformat_close_input( &(decoder->formatCtx) );
        avformat_free_context(decoder->formatCtx);
        free(decoder);
        return NULL;
    }

    decoder->frame = avcodec_alloc_frame();

    decoder->peekPacket = (AVPacket *) malloc(sizeof(AVPacket)); 

    // initialize peekPacket
    if (av_read_frame(decoder->formatCtx, decoder->peekPacket) < 0)
    {
        printf("Failed to read first packet from %s\n", filename);
        pypopro_decoder_close(decoder);
        return NULL;
    }

    decoder->last_read_pts = -1;
    decoder->currentPacket = NULL;

    printf("pypopro decoder initialized: %s\n", filename);
    return decoder;
}

void pypopro_decoder_close(PypoproDecoder *decoder)
{
    printf("pypopro closing decoder %ld\n", (intptr_t) decoder);
    fflush(stdout);
    avformat_close_input( &(decoder->formatCtx) );
    avformat_free_context(decoder->formatCtx);
    avcodec_close(decoder->codecCtx);
    avcodec_free_context( &(decoder->codecCtx) );
    avcodec_free_frame( &(decoder->frame) );
    if (decoder->peekPacket != NULL)
        free(decoder->peekPacket);
    if (decoder->currentPacket != NULL)
        free(decoder->currentPacket);
    free(decoder);
}

int advance(PypoproDecoder *decoder)
{
    if (decoder->currentPacket == NULL)
        decoder->currentPacket = (AVPacket *) malloc(sizeof(AVPacket));

    AVPacket *swap = decoder->peekPacket;
    decoder->peekPacket = decoder->currentPacket;
    decoder->currentPacket = swap;

    //read next packet
    if (av_read_frame(decoder->formatCtx, decoder->peekPacket) < 0)
    {
        printf("Failed to read next packet\n");
        return -1;
    }

    //decode the current packet
    //printf("Decoding packet with pts=%lld\n", decoder->currentPacket->pts);
    int frameFinished = 0;
    avcodec_decode_video2(decoder->codecCtx,
                          decoder->frame,
                          &frameFinished,
                          decoder->currentPacket);

    return frameFinished;
}

AVFrame *pypopro_decoder_read(PypoproDecoder *decoder, int64_t pts)
{
    if (pts < decoder->last_read_pts)
    {
        printf("Trying to read backwards?\n");
        return NULL;
    }
    decoder->last_read_pts = pts;

    while (decoder->currentPacket == NULL
            || decoder->peekPacket->pts <= pts)
    {
        if (advance(decoder) == -1)
        {
            printf("Failed to advance.\n");
            return NULL;
        }
    }

    
    printf("Read %lld, returning pts=%lld\n", pts, decoder->currentPacket->pts);
    printf("returning width=%d height=%d frame_pts=%lld frame_dts=%lld\n", decoder->frame->width, decoder->frame->height, decoder->frame->pkt_pts, decoder->frame->pkt_dts);
    return decoder->frame;
}
