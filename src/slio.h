#ifndef SLIO_H
#define SLIO_H

#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>


typedef struct SLio {
    const char *input_media_filename;
    //const char *output_media_filename;

    const char *output_tag;

    //const char* temp;
    
    AVFormatContext *input_ctx;
    AVFormatContext *output_ctx;
} SLio;

SLio *init_io(const char *filename);
void open_media_input(SLio *io);
void write_file_header(SLio *io);
void alloc_output_ctx(SLio *io);
//const char* create_output_tag(SLio *io);
void free_io(SLio *io);

#endif // SLIOMANAGER_H
