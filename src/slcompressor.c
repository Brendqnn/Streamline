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

    open_media_input(io);
    find_media_streams(codec, io);
    open_decoder_ctx(codec);
    open_audio_decoder_ctx(codec);
    open_encoder_ctx(codec);
    open_audio_encoder_ctx(codec);
    alloc_output_ctx(io);
    stream_to_output(codec, io);
    write_file_header(io);
}

void compress(SLcompressor *compressor, SLcodec *codec, SLio *io)
{
    while (av_read_frame(io->input_ctx, &compressor->packet) >= 0) {
        if (compressor->packet.stream_index == codec->video_stream_idx) {
            // Handle video packets as before
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
                if (avcodec_send_frame(codec->video_encoder_ctx, compressor->frame) < 0) {
                    printf("Error: Failed to send frame to the encoder.\n");
                    break;
                }
                while (avcodec_receive_packet(codec->video_encoder_ctx, &compressor->packet) >= 0) {
                    if (av_interleaved_write_frame(io->output_ctx, &compressor->packet) < 0) {
                        printf("Error: Error writing video frame to output.\n");
                        break;
                    }
                }
            }
        } else if (compressor->packet.stream_index == codec->audio_stream_idx) {
            // Handle audio packets separately using audio encoder context
            if (avcodec_send_packet(codec->audio_decoder_ctx, &compressor->packet) < 0) {
                printf("Error: Failed to send audio packet to decoder.\n");
                break;
            }
            int ret;
            while ((ret = avcodec_receive_frame(codec->audio_decoder_ctx, compressor->frame)) >= 0) {
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    printf("Error: Failed to return decoded audio frame data.\n");
                    break;
                }
                if (avcodec_send_frame(codec->audio_encoder_ctx, compressor->frame) < 0) {
                    printf("Error: Failed to send audio frame to the audio encoder.\n");
                    break;
                }
                while (avcodec_receive_packet(codec->audio_encoder_ctx, &compressor->packet) >= 0) {
                    if (av_interleaved_write_frame(io->output_ctx, &compressor->packet) < 0) {
                        printf("Error: Error writing audio packet to output.\n");
                        break;
                    }
                }
            }
        }
        av_packet_unref(&compressor->packet);
    }
    av_frame_free(&compressor->frame);
}

void free_compressor(SLcompressor *compressor)
{
    if (compressor != NULL) {
        free_io(compressor->io);
        free_codec(compressor->codec);
        free(compressor);
    }
}

