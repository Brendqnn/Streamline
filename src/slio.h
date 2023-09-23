#ifndef SLIO_H
#define SLIO_H

#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include "slqueue.h"

typedef struct {
    const char *input_media_filename;
    const char *output_tag;
    
    AVFormatContext *input_ctx;
    AVFormatContext *output_ctx;

    SLqueue *queue;
} SLio;

SLio *init_io();
void open_media_input(SLio *io);
void write_file_header(SLio *io);
void alloc_output_ctx(SLio *io);
void insert_queue_node(SLio *io);
void remove_queue_node(SLio *io);
void free_io(SLio *io);

#endif // SLIO_H

