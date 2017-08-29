package com.ffmpegdemo;

/**
 * Created by liupei on 2017/8/29.
 */

public class VideoUtils {

    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("video");
    }

    public native static void Decode(String input, String output);
}
