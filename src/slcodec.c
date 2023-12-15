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
    codec->input_video_framerate = (AVRational) {0, 0}; // Initialize as 0/0
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
    codec->video_decoder_ctx->thread_count = 1;
    if (avcodec_open2(codec->video_decoder_ctx, codec->video_decoder, NULL) < 0) {
        printf("Error: Failed to open video decoder codec.\n");
        return;
    }
}

void set_encoder_prop(SLcodec *codec)
{
    codec->video_encoder_ctx->width = codec->video_decoder_ctx->width;
    codec->video_encoder_ctx->height = codec->video_decoder_ctx->height;
    codec->video_encoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    codec->video_encoder_ctx->time_base = codec->input_video_stream->time_base;
    
    codec->video_encoder_ctx->thread_type = FF_THREAD_SLICE; // Enable slice-level multithreading
    codec->video_encoder_ctx->thread_count = 1;
}

void set_nvenc_encoder_prop(SLcodec *codec)
{
    av_opt_set(codec->video_encoder_ctx->priv_data, "preset", "default", 0);
    av_opt_set(codec->video_encoder_ctx->priv_data, "profile", "main", 0);
    av_opt_set(codec->video_encoder_ctx->priv_data, "tune", "zerolatency", 0);
    
    codec->video_encoder_ctx->bit_rate = 4000000; 
    codec->video_encoder_ctx->width = codec->video_decoder_ctx->width;
    codec->video_encoder_ctx->height = codec->video_decoder_ctx->height;
    codec->video_encoder_ctx->pix_fmt = AV_PIX_FMT_NV12;
    codec->video_encoder_ctx->time_base = codec->input_video_stream->time_base;
    codec->video_encoder_ctx->gop_size = 10;
    codec->video_encoder_ctx->max_b_frames = 2;
    
    codec->video_encoder_ctx->thread_type = FF_THREAD_SLICE; // Enable slice-level multithreading
    codec->video_encoder_ctx->thread_count = 1;
}

void open_encoder_ctx(SLcodec *codec)
{
    //codec->video_encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    codec->video_encoder = avcodec_find_encoder_by_name("h264_nvenc");
    codec->video_encoder_ctx = avcodec_alloc_context3(codec->video_encoder);
    set_nvenc_encoder_prop(codec);

    if (avcodec_open2(codec->video_encoder_ctx, codec->video_encoder, NULL) < 0) {
        printf("Error: Failed to open encoder codec.\n");
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

