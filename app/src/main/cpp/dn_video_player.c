#include "video.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"jason",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"jason",FORMAT,##__VA_ARGS__);

#include "libyuv/libyuv.h"

//封装格式
#include "include/libavformat/avformat.h"
//解码
#include "include/libavcodec/avcodec.h"
//缩放
#include "include/libswscale/swscale.h"
#include "include/libavutil/imgutils.h"


JNIEXPORT void JNICALL Java_com_ffmpegdemo_VideoUtils_render
        (JNIEnv *env, jclass jobj, jstring input_jstr, jobject surface) {
    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    //1.注册组件
    av_register_all();

    //封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    //2.打开输入视频文件
    if (avformat_open_input(&pFormatCtx, input_cstr, NULL, NULL) != 0) {
        LOGE("%s", "打开输入视频文件失败");
        return;
    }
    //3.获取视频信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
        return;
    }

    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int video_stream_idx = -1;
    int i = 0;
    for (; i < pFormatCtx->nb_streams; i++) {
        //根据类型判断，是否是视频流
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    AVDictionaryEntry *entry = NULL;
    entry = av_dict_get(pFormatCtx->streams[video_stream_idx]->metadata, "rotate", entry, NULL);

    if (entry != NULL) {
        LOGE("%s", entry->value);
    }
    //4.获取视频解码器
    AVCodecContext *pCodecCtx = pFormatCtx->streams[video_stream_idx]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("%s", "无法解码");
        return;
    }

    //5.打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }

    //编码数据
//	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    //像素数据（解码数据）
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

    //native绘制
    //窗体
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //绘制时的缓冲区
    ANativeWindow_Buffer outBuffer;

    AVFrame *pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        return;
    }

    // Determine required buffer size and allocate buffer
    // buffer中数据就是用于渲染的,且格式为RGBA
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height,
                                            1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == video_stream_idx) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &outBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) outBuffer.bits;
                int dstStride = outBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < pCodecCtx->height; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_packet_unref(&packet);
    }

//	int len ,got_frame, framecount = 0;
//	//6.一阵一阵读取压缩的视频数据AVPacket
//	while(av_read_frame(pFormatCtx,packet) >= 0){
//		//解码AVPacket->AVFrame
//		len = avcodec_decode_video2(pCodeCtx, yuv_frame, &got_frame, packet);
//
//		//Zero if no frame could be decompressed
//		//非零，正在解码
//		if(got_frame){
//			LOGI("解码%d帧",framecount++);
//			//lock
//			//设置缓冲区的属性（宽、高、像素格式）
//			ANativeWindow_setBuffersGeometry(nativeWindow, pCodeCtx->width, pCodeCtx->height,WINDOW_FORMAT_RGBA_8888);
//			ANativeWindow_lock(nativeWindow,&outBuffer,NULL);
//
//			//设置rgb_frame的属性（像素格式、宽高）和缓冲区
//			//rgb_frame缓冲区与outBuffer.bits是同一块内存
//			avpicture_fill((AVPicture *)rgb_frame, outBuffer.bits, AV_PIX_FMT_RGBA, pCodeCtx->width, pCodeCtx->height);
//
//			//YUV->RGBA_8888
//			I420ToARGB(yuv_frame->data[0],yuv_frame->linesize[0],
//                       yuv_frame->data[2],yuv_frame->linesize[2],
//					   yuv_frame->data[1],yuv_frame->linesize[1],
//					   rgb_frame->data[0], rgb_frame->linesize[0],
//					   pCodeCtx->width,pCodeCtx->height);
//
//			//unlock
//			ANativeWindow_unlockAndPost(nativeWindow);
//
//			usleep(1000 * 16);
//
//		}
//
//		av_free_packet(packet);
//	}

    ANativeWindow_release(nativeWindow);
    avcodec_close(pCodecCtx);
    avformat_free_context(pFormatCtx);

    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
}

