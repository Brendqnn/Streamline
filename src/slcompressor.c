#include "slcompressor.h"

SLcompressor *init_compressor(SLio *io, SLcodec *codec) {
    SLcompressor *compressor = malloc(sizeof(SLcompressor));
    
    compressor->frame = av_frame_alloc();
        
    compressor->packet.data = NULL;
    compressor->packet.size = 0;

    compressor->io = io;
    compressor->codec = codec;
    
    return compressor;
}

void compressor_setup(SLcompressor *compressor, SLcodec *codec, SLio *io)
{
    if (codec == NULL || io == NULL) {
        printf("Error: Failed to setup compressor. Codec and or io pointers are NULL.\n");
        return;
    }

    load_input(io);
    open_media_input(io);
    find_media_streams(codec, io);
    open_decoder_ctx(codec);
    open_encoder_ctx(codec);
    alloc_output_ctx(io);
    stream_to_output(codec, io);
    write_file_header(io);
}

void process_batch(SLcodec *codec, SLio *io, AVPacket **packet_batch, AVFrame **frame_batch, int frame_count)
{
    for (int i = 0; i < frame_count; i++) {
        if (avcodec_send_frame(codec->video_encoder_ctx, frame_batch[i]) < 0) {
            printf("Error: Failed to send frame to the encoder.\n");
            break;
        }
        while (avcodec_receive_packet(codec->video_encoder_ctx, packet_batch[i]) >= 0) {
            if (av_interleaved_write_frame(io->output_ctx, packet_batch[i]) < 0) {
                printf("Error: Error writing video frame to output.\n");
                break;
            }
        }
        
        av_packet_unref(packet_batch[i]);
        av_frame_unref(frame_batch[i]);
        av_packet_free(&packet_batch[i]);
        av_frame_free(&frame_batch[i]);
    }
}

void compress(SLcompressor *compressor, SLcodec *codec, SLio *io)
{
    const int max_batch_size = 6;

    AVPacket *packet_batch[max_batch_size];
    AVFrame *frame_batch[max_batch_size];

    int frame_count = 0;

    while (av_read_frame(io->input_ctx, &compressor->packet) >= 0) {
        if (compressor->packet.stream_index == codec->video_stream_idx) {
            if (avcodec_send_packet(codec->video_decoder_ctx, &compressor->packet) < 0) {
                printf("Error: Failed to send packet to decoder.\n");
                break;
            }
            int ret;
            while ((ret = avcodec_receive_frame(codec->video_decoder_ctx, compressor->frame)) >= 0) {
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    printf("Error: Failed to return decoded frame data.\n");
                    break;
                }

                packet_batch[frame_count] = av_packet_alloc();
                frame_batch[frame_count] = av_frame_alloc();
                av_frame_ref(frame_batch[frame_count], compressor->frame);
                av_packet_ref(packet_batch[frame_count], &compressor->packet);
                frame_count++;

                if (frame_count >= max_batch_size) {
                    process_batch(codec, io, packet_batch, frame_batch, frame_count);
                    frame_count = 0;
                }
            }
        } else if (compressor->packet.stream_index == codec->audio_stream_idx) {
            if (av_interleaved_write_frame(io->output_ctx, &compressor->packet) < 0) {
                printf("Error: Error writing audio packet to output.\n");
                break;
            }
        }
        av_packet_unref(&compressor->packet);
    }
    
    if (frame_count > 0) {
        process_batch(codec, io, packet_batch, frame_batch, frame_count);
    }

    av_write_trailer(io->output_ctx);
}

void free_compressor(SLcompressor *compressor, SLcodec *codec, SLio *io)
{
    if (compressor != NULL) {
        av_frame_free(&compressor->frame);
        free_io(io);
        free_codec(codec);
        free(compressor);
    }
}

