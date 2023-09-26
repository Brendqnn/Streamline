#include "slio.h"


SLio *init_io()
{
    SLio *io = malloc(sizeof(SLio));

    io->input_ctx = NULL;
    io->output_ctx = NULL;
    io->input_media_filename = NULL;
    io->output_media_filename = NULL;
    io->output_media_filename_temp = NULL;
    io->output_tag = "res/sl-";

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
        if (avio_open(&io->output_ctx->pb, io->output_media_filename, AVIO_FLAG_WRITE) < 0) {
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
    if (io->output_media_filename == NULL) {
        fprintf(stderr, "Error: output_media_filename was not allocated correctly\n");
        return;
    }
    if (avformat_alloc_output_context2(&io->output_ctx, NULL, NULL, io->output_media_filename) > 0) {
        printf("Error: Failed to allocate output context to output file.\n");
        return;
    }
}
 
void load_input(SLio *io)
{
    SLqueue *queue = io->queue;
    if (queue != NULL) {
        io->input_media_filename = queue->data;
        printf("filename from load_input: %s\n", queue->data);
    } else {
        printf("Queue is empty...\n");
    }
    create_output(io);
}

void remove_queue_node(SLio *io)
{
    SLqueue *queue = io->queue;
    if (queue != NULL) {
        remove_node(&queue);
        
        printf("node successfully freed.\n");
        
    }
}

void create_output(SLio *io)
{
    if (io->input_media_filename == NULL) {
        fprintf(stderr, "Error: input media filename is NULL.\n");
        return;
    }
    
    const char* last_slash = strrchr(io->input_media_filename, '/');
    
    if (last_slash != NULL) {
        io->output_media_filename_temp = last_slash + 1;
        size_t length = strlen(io->output_tag) + strlen(io->output_media_filename_temp + 1);
        const char *result = malloc(length);
        
        strcpy(result, io->output_tag);
        strcat(result, io->output_media_filename_temp);

        if (result != NULL) {
            io->output_media_filename = result;
        }
    }
}

void free_io(SLio *io)
{
    if (io != NULL) {
        avformat_close_input(&io->input_ctx);
        avformat_free_context(io->output_ctx);
        free(io);
    }
}
