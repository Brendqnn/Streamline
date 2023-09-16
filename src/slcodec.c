#include "slcodec.h"


SLcodec *init_codec(SLio *io)
{
    SLcodec *codec = malloc(sizeof(SLcodec));
    
    codec->video_decoder = NULL;
    codec->video_decoder_ctx = NULL;
    codec->video_encoder = NULL;
    codec->video_encoder_ctx = NULL;
    codec->audio_decoder = NULL;
    codec->audio_decoder_ctx = NULL;
    codec->audio_encoder = NULL;
    codec->audio_encoder_ctx = NULL;
    codec->input_video_stream = NULL;
    codec->input_audio_stream = NULL;
    codec->output_video_stream = NULL;
    codec->output_audio_stream = NULL;
    codec->input_video_framerate = (AVRational){0, 0}; // Initialize as 0/0
    codec->video_stream_idx = -1; // Initialize to -1
    codec->audio_stream_idx = -1; // Initialize to -1

    codec->io = io;
        
    return codec;
}

void find_media_streams(SLcodec *codec, SLio *io)
{
    for (size_t i = 0; i < io->input_ctx->nb_streams; i++) {
        if (io->input_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            codec->video_stream_idx = i;
        }
        else if (io->input_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            codec->audio_stream_idx = i;
        }
    }
    
    if (codec->video_stream_idx == -1) {
        printf("Error: Video stream index not found.\n");
        return;
    }
    if (codec->audio_stream_idx == -1) {
        printf("Error: Audio stream index not found.\n");
        return;
    }
    
    codec->input_video_stream = io->input_ctx->streams[codec->video_stream_idx];
    codec->input_audio_stream = io->input_ctx->streams[codec->audio_stream_idx];
    codec->input_video_framerate = codec->input_video_stream->r_frame_rate;
}

void open_decoder_ctx(SLcodec *codec)
{
    codec->video_decoder = avcodec_find_decoder(codec->input_video_stream->codecpar->codec_id);
    codec->video_decoder_ctx = avcodec_alloc_context3(codec->video_decoder);
    if (avcodec_parameters_to_context(codec->video_decoder_ctx, codec->input_video_stream->codecpar) < 0) {
        printf("Error: Failed to copy video codec context to parameters.\n");
        return;
    }
    codec->video_decoder_ctx->thread_type = FF_THREAD_FRAME;
    codec->video_decoder_ctx->thread_count = 4;
    if (avcodec_open2(codec->video_decoder_ctx, codec->video_decoder, NULL) < 0) {
        printf("Error: Failed to open video decoder codec.\n");
        return;
    }
}

void open_audio_decoder_ctx(SLcodec *codec)
{
    if (codec->audio_stream_idx == -1) {
        printf("Error: Audio stream index not found.\n");
        return;
    }

    codec->audio_decoder = avcodec_find_decoder(codec->input_audio_stream->codecpar->codec_id);
    if (!codec->audio_decoder) {
        printf("Error: Failed to find audio decoder.\n");
        return;
    }

    // Initialize audio decoder context
    codec->audio_decoder_ctx = avcodec_alloc_context3(codec->audio_decoder);
    if (!codec->audio_decoder_ctx) {
        printf("Error: Failed to allocate audio decoder context.\n");
        return;
    }

    if (avcodec_parameters_to_context(codec->audio_decoder_ctx, codec->input_audio_stream->codecpar) < 0) {
        printf("Error: Failed to copy audio codec context to parameters.\n");
        return;
    }

    if (avcodec_open2(codec->audio_decoder_ctx, codec->audio_decoder, NULL) < 0) {
        printf("Error: Failed to open audio decoder codec.\n");
        return;
    }
}

void set_encoder_properties(SLcodec *codec)
{
    codec->video_encoder_ctx->rc_max_rate = 8000000;  // Higher bitrate for better quality
    codec->video_encoder_ctx->rc_min_rate = 0;  // Minimum bitrate (0 for auto)
    codec->video_encoder_ctx->rc_buffer_size = 8000000;
    
    //Set CRF (Constant Rate Factor) for VBR mode (e.g., CRF 23 for moderate quality)
    av_opt_set(codec->video_encoder_ctx->priv_data, "crf", "14", 0);
    av_opt_set(codec->video_encoder_ctx->priv_data, "preset", "superfast", 0);
    
    //Set other properties like resolution, framerate, etc.
    codec->video_encoder_ctx->width = codec->video_decoder_ctx->width;
    codec->video_encoder_ctx->height = codec->video_decoder_ctx->height;
    codec->video_encoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P; // or YUV422P
    codec->video_encoder_ctx->time_base = codec->input_video_stream->time_base;
    codec->video_encoder_ctx->framerate = codec->input_video_framerate;

    codec->video_encoder_ctx->thread_type = FF_THREAD_SLICE; // Enable slice-level multithreading
    codec->video_encoder_ctx->thread_count = 4;
}

void open_encoder_ctx(SLcodec *codec)
{
    codec->video_encoder = avcodec_find_encoder(codec->input_video_stream->codecpar->codec_id);
    codec->video_encoder_ctx = avcodec_alloc_context3(codec->video_encoder);
    set_encoder_properties(codec);
    if (avcodec_open2(codec->video_encoder_ctx, codec->video_encoder, NULL) < 0) {
        printf("Error: Failed to open encoder codec.\n");
        return;
    }
}

void open_audio_encoder_ctx(SLcodec *codec)
{
    codec->audio_encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec->audio_encoder) {
        printf("Error: Failed to find audio encoder.\n");
        return;
    }

    // Initialize audio encoder context
    codec->audio_encoder_ctx = avcodec_alloc_context3(codec->audio_encoder);
    if (!codec->audio_encoder_ctx) {
        printf("Error: Failed to allocate audio encoder context.\n");
        return;
    }
    
    codec->audio_encoder_ctx->bit_rate = 128000; // Adjust the bitrate as needed
    codec->audio_encoder_ctx->sample_fmt = AV_SAMPLE_FMT_FLTP; // or AV_SAMPLE_FMT_S16P
    codec->audio_encoder_ctx->sample_rate = 44100; // Adjust the sample rate as needed
    codec->audio_encoder_ctx->channel_layout = AV_CH_LAYOUT_STEREO;
        
    if (avcodec_open2(codec->audio_encoder_ctx, codec->audio_encoder, NULL) < 0) {
        printf("Error: Failed to open audio encoder codec.\n");
        return;
    }
}
 
void copy_audio_parameters(SLcodec *codec, SLio *io)
{
    codec->output_audio_stream = avformat_new_stream(io->output_ctx, NULL);
    if (!codec->output_audio_stream) {
        printf("Error: Failed to find output stream.\n");
        return;
    }
    avcodec_parameters_copy(codec->output_audio_stream->codecpar, codec->input_audio_stream->codecpar);
}

void stream_to_output(SLcodec *codec, SLio *io)
{
    codec->output_video_stream = avformat_new_stream(io->output_ctx, NULL);
    codec->output_video_stream->time_base = codec->input_video_stream->time_base;
    copy_audio_parameters(codec, io);
    if (!codec->output_video_stream) {
        printf("Error: Failed to allocate output video stream.\n");
        return;
    }
    if (avcodec_parameters_from_context(codec->output_video_stream->codecpar, codec->video_encoder_ctx) < 0) {
        printf("Error: Failed to copy codec parameters.\n");
        return;
    }
}

void free_codec(SLcodec *codec)
{
    if (codec != NULL) {
        avcodec_free_context(&codec->video_decoder_ctx);
        avcodec_free_context(&codec->video_encoder_ctx);
        avcodec_free_context(&codec->audio_decoder_ctx);
        avcodec_free_context(&codec->audio_encoder_ctx);
        free(codec);
    }
}

