#ifndef SLCOMPRESSOR_H
#define SLCOMPRESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

#include "slio.h"
#include "slcodec.h"
#include "slcompressor.h"
#include "../include/util.h"


typedef struct {
    AVFrame *frame; 
    AVPacket packet;
    
    SLio *io;
    SLcodec *codec;
} SLcompressor;

SLcompressor *init_compressor(SLio *io, SLcodec *codec);
void compressor_setup(SLcompressor *compressor, SLcodec *codec, SLio *io);
void process_batch(SLcodec *codec, SLio *io, AVPacket **packet_batch, AVFrame **frame_batch, int frame_count);
void compress(SLcompressor *compressor, SLcodec *codec, SLio *io);
void free_compressor(SLcompressor *compressor, SLcodec *codec, SLio *io);

#endif // SLCOMPRESSOR_H
