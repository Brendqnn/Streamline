#ifndef SLCODEC_H
#define SLCODEC_H

#include <stdio.h>
#include <stdlib.h>

#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>

#include "slio.h"


typedef struct SLcodec {
    const AVCodec *video_decoder;
    AVCodecContext *video_decoder_ctx;

    const AVCodec *video_encoder;
    AVCodecContext *video_encoder_ctx;

    const AVCodec* audio_decoder;
    AVCodecContext* audio_decoder_ctx;

    const AVCodec* audio_encoder;
    AVCodecContext* audio_encoder_ctx;
    
    AVStream *input_video_stream;
    AVStream *input_audio_stream;
    AVStream *output_video_stream;
    AVStream *output_audio_stream;
    
    AVRational input_video_framerate;

    int video_stream_idx;
    int audio_stream_idx;

    SLio *io;
} SLcodec;

SLcodec *init_codec(SLio *io);
void find_media_streams(SLcodec *codec, SLio *io);
void open_decoder_ctx(SLcodec *codec);
void open_encoder_ctx(SLcodec *codec);
void open_audio_decoder_ctx(SLcodec *codec);
void open_audio_encoder_ctx(SLcodec *codec);
void copy_audio_parameters(SLcodec *codec, SLio *io);
void stream_to_output(SLcodec *codec, SLio *io);
void set_encoder_properties(SLcodec *codec);
void stream_to_output(SLcodec *codec, SLio *io);
void free_codec(SLcodec *codec);

#endif // SLCODEC_H
