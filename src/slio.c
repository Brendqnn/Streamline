#include "slio.h"


SLio *init_io(const char *filename)
{
    SLio *io = malloc(sizeof(SLio));

    io->input_ctx = NULL;
    io->output_ctx = NULL;
    io->input_media_filename = filename;
    io->output_tag = "sl-";
        
    return io;
}

void insert_node(SLio *io, const char *data)
{
    SLioNode *new_node = malloc(sizeof(SLioNode));
    new_node->data = data;
    new_node->next = io->node_head;
    io->node_head = new_node;
}

void remove_node(SLio *io, const char *data)
{
    SLioNode *current = io->node_head;
    SLioNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->data, data) == 0) {
            if (previous == NULL) {
                // Removing the head node
                io->node_head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return; // Node found and removed
        }
        previous = current;
        current = current->next;
    }
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

void free_io(SLio *io)
{
    if (io != NULL) {
        avformat_close_input(&io->input_ctx);
        avformat_free_context(io->output_ctx);

        // Free the linked list
        SLioNode *current = io->node_head;
        while (current != NULL) {
            SLioNode *next = current->next;
            free(current);
            current = next;
        }

        free(io);
    }
}
