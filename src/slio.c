#include "slio.h"


SLio *init_io()
{
    SLio *io = malloc(sizeof(SLio));

    io->input_ctx = NULL;
    io->output_ctx = NULL;
    io->input_media_filename = NULL;
    io->output_tag = "sl-";

    io->queue = NULL;
    
    return io;
}

void open_media_input(SLio *io)
{
    if (avformat_open_input(&io->input_ctx, io->input_media_filename, NULL, NULL) < 0) {
        printf("Error: Failed to open input.\n");
        return;
    }
}

void write_file_header(SLio *io)
{
    if (!(io->output_ctx->flags & AVFMT_NOFILE)) {
        if (avio_open(&io->output_ctx->pb, io->output_tag, AVIO_FLAG_WRITE) < 0) {
            printf("Error: Failed to open output file.\n");
            return;
        }
    }
    if (avformat_write_header(io->output_ctx, NULL) < 0) {
        printf("Error: Failed write header to output file.\n");
        return;
    }
}

void alloc_output_ctx(SLio *io)
{
    if (avformat_alloc_output_context2(&io->output_ctx, NULL, NULL, io->output_tag) > 0) {
        printf("Error: Failed to allocate output context to output file.\n");
        return;
    }
}

void load_input(SLio *io)
{
    SLqueue *queue = io->queue;
    if (queue != NULL) {
        io->input_media_filename = queue->data;
        printf("filename: %s\n", queue->data);
    } else {
        printf("Queue is empty...\n");
    }
}

void free_io(SLio *io)
{
    if (io != NULL) {
        avformat_close_input(&io->input_ctx);
        avformat_free_context(io->output_ctx);
        // TODO free list
        free(io);
    }
}
