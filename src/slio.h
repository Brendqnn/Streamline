#ifndef SLIO_H
#define SLIO_H

#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>

typedef struct {
    const char *data;
    struct SLioNode *next;  // Pointer to the next node
} SLioNode;

typedef struct {
    const char *input_media_filename;
    const char *output_tag;
        
    AVFormatContext *input_ctx;
    AVFormatContext *output_ctx;
    
    SLioNode *node_head;  // Head of the linked list
    
    //SLio *next;
} SLio;

SLio *init_io(const char *filename);
void open_media_input(SLio *io);
void write_file_header(SLio *io);
void alloc_output_ctx(SLio *io);
void insert_node(SLio *io, const char *data);
void remove_node(SLio *io, const char *data);
void free_io(SLio *io);

#endif // SLIO_H

