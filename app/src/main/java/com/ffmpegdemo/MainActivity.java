package com.ffmpegdemo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView= (SurfaceView) findViewById(R.id.surface);
    }

    public void render(View view) {
        final String input = Environment.getExternalStorageDirectory() + File.separator + "Movies" + File.separator + "tuboshu.mp4";
        final String output = Environment.getExternalStorageDirectory() + File.separator + "output.yuv";
        VideoUtils.render(input, surfaceView.getHolder().getSurface());
        Log.e("tag", input);
    }


}
