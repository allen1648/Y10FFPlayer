package com.stan.y10ffplayer;

import android.Manifest;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.View;
import android.widget.TextView;

import com.stan.ff10player.FF10Player;
import com.stan.ff10player.listener.OnLoadListener;
import com.stan.ff10player.listener.OnPreparedListener;
import com.stan.ff10player.listener.OnResumePauseListener;
import com.stan.ff10player.listener.OnTimeChangedListener;
import com.stan.y10ffplayer.Utils.LogUtil;
import com.stan.y10ffplayer.Utils.PermissionUtil;
import com.stan.y10ffplayer.Utils.TimeUtil;

public class MainActivity extends Activity {

    private static final int SET_PROGRESS = 1;
    private FF10Player mPlayer = new FF10Player();
    private TextView mProgressTV;
    private Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case SET_PROGRESS:
                    mProgressTV.setText(TimeUtil.secondsToDateFormat(msg.arg1, false) + "/" + TimeUtil.secondsToDateFormat(msg.arg2, false));
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        PermissionUtil.requestPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE);
        PermissionUtil.requestPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        initView();
        init();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        mHandler.removeMessages(SET_PROGRESS);
    }

    private void initView() {
        mProgressTV = findViewById(R.id.tv_progress);
    }

    private void init() {
        mPlayer.setOnPrepareListener(new OnPreparedListener() {
            @Override
            public void onSuccess() {
                mPlayer.start();
            }

            @Override
            public void onError() {

            }
        });
        mPlayer.setOnResumePauseListener(new OnResumePauseListener() {
            @Override
            public void onResumePauseChanged(boolean pause) {
                if (pause) {
                    LogUtil.i("yyl", "pause");
                } else {
                    LogUtil.i("yyl", "resume");
                }
            }
        });

        mPlayer.setOnTimeChangedListener(new OnTimeChangedListener() {
            @Override
            public void onTimeChanged(int currentTime, int totalTime) {
                mHandler.obtainMessage(SET_PROGRESS, currentTime, totalTime).sendToTarget();
            }
        });

        mPlayer.setOnLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean load) {
                if (load) {
                    LogUtil.i("yyl", "加载中...");
                } else {
                    LogUtil.i("yyl", "播放中...");
                }
            }
        });
    }

    public void prepare(View view) {
//        mPlayer.setDataSourceAndPrepare("http://cmp3.o2ting.com:8081/mp3_32k/100374/873790/bazc001.mp3?ver=TYYDYSB_A1.5.4.2&pno=0000&userName=&audio=1188555&md5=wnVgHti5zygNGNpdnxFZUA&expires=1539941128");
//        mPlayer.setDataSourceAndPrepare("/sdcard/input.mp3");
        mPlayer.setDataSourceAndPrepare("/sdcard/zly.mp3");
    }

    public void resume(View view) {
        mPlayer.resume();
    }

    public void pause(View view) {
        mPlayer.pause();
    }

    public void stop(View view) {
        mPlayer.stop();
    }
}
