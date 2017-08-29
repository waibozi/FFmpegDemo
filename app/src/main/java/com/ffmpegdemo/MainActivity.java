package com.ffmpegdemo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void decode(View view) {
        final String input = Environment.getExternalStorageDirectory() + File.separator + "Movies" + File.separator + "tuboshu.mp4";
        final String output = Environment.getExternalStorageDirectory() + File.separator + "output.yuv";
        Log.e("tag", input);
        new Thread() {
            @Override
            public void run() {
                VideoUtils.Decode(input, output);
            }
        }.start();
    }
}
