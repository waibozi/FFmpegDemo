//
// Created by 刘培 on 2017/9/1.
//

#include "video.h"
#include <stdlib.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "android/log.h"
#include <unistd.h>

#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "libyuv/libyuv.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"tag",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"tag",FORMAT,##__VA_ARGS__);

JNIEXPORT void JNICALL Java_com_ffmpegdemo_VideoUtils_render
        (JNIEnv *env, jclass jobj, jstring input_jstr, jobject surface) {

    const char *input = (*env)->GetStringUTFChars(env, input_jstr, NULL);

    LOGE("进来了");

    av_register_all();

    AVFormatContext *context = avformat_alloc_context();

    if (avformat_open_input(&context, input, NULL, NULL) != 0) {
        LOGE("%s", "无法打开输入视频文件");
        return;
    }

    if (avformat_find_stream_info(context, NULL) < 0) {
        LOGE("%s", "无法获取视频文件信息");
        return;
    }

    int idx = -1;

    for (int i = 0; i < context->nb_streams; ++i) {
        if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        LOGE("无法获取视屏文件");
        return;

    }
    AVCodecContext *codecContext = context->streams[idx]->codec;
    AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);

    if (codec == NULL) {
        LOGE("%s", "无法解码");
        return;
    }

    //5.打开解码器
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }

    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));

    //像素数据（解码数据）
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

    //native绘制
    //窗体
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env,surface);
    //绘制时的缓冲区
    ANativeWindow_Buffer outBuffer;

    int got_frame;

    while (av_read_frame(context, avPacket) > 0) {
        avcodec_decode_video2(codecContext, yuv_frame, &got_frame, avPacket);
        if (got_frame) {
            ANativeWindow_setBuffersGeometry(nativeWindow,codecContext->width,codecContext->height,WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(nativeWindow,&outBuffer,NULL);
            avpicture_fill((AVPicture *)rgb_frame,outBuffer.bits,AV_PIX_FMT_RGBA,codecContext->width,codecContext->height);
            I420ToARGB(yuv_frame->data[0],yuv_frame->linesize[0],
                       yuv_frame->data[2],yuv_frame->linesize[2],
                       yuv_frame->data[1],yuv_frame->linesize[1],
                       rgb_frame->data[0], rgb_frame->linesize[0],
                       codecContext->width,codecContext->height);

            //unlock
            ANativeWindow_unlockAndPost(nativeWindow);

            usleep(1000 * 16);
        }
        av_free_packet(avPacket);
    }

    ANativeWindow_release(nativeWindow);
    av_frame_free(&yuv_frame);
    avcodec_close(codecContext);
    avformat_free_context(context);

    (*env)->ReleaseStringUTFChars(env, input_jstr, input);
}