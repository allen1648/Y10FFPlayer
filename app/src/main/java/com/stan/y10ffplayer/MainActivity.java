package com.stan.y10ffplayer;

import android.Manifest;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.stan.ff10player.FF10Player;
import com.stan.ff10player.listener.OnCallPcmInfoListener;
import com.stan.ff10player.listener.OnCallPcmRateListener;
import com.stan.ff10player.listener.OnErrorListener;
import com.stan.ff10player.listener.OnLoadListener;
import com.stan.ff10player.listener.OnPreparedListener;
import com.stan.ff10player.listener.OnResumePauseListener;
import com.stan.ff10player.listener.OnTimeChangedListener;
import com.stan.y10ffplayer.Utils.LogUtil;
import com.stan.y10ffplayer.Utils.PermissionUtil;
import com.stan.y10ffplayer.Utils.TimeUtil;

import java.io.File;

public class MainActivity extends Activity {
    private static final int OPEN_INPUT_FAILED = 1001;
    private static final int FIND_STREAM_FAILED = 1002;
    private static final int FIND_CODEC_FAILED = 1003;
    private static final int MALLOC_FAILED = 1004;
    private static final int FILL_DECODER_FAILED = 1005;
    private static final int OPEN_STREAM_FAILED = 1006;
    private static final int SET_PROGRESS = 1;
    private int mPosition;
    private FF10Player mPlayer = new FF10Player();
    private TextView mProgressTV;
    private TextView mVolumeTV;
    private SeekBar mProgressBar;
    private SeekBar mVolumeBar;
    private Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case SET_PROGRESS:
                    mProgressTV.setText(TimeUtil.secondsToDateFormat(msg.arg1, false) + "/" + TimeUtil.secondsToDateFormat(msg.arg2, false));
                    mProgressBar.setProgress(msg.arg1 * 100 / msg.arg2);
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
        mVolumeTV = findViewById(R.id.tv_volume_sb);
        mProgressBar = findViewById(R.id.seek_bar);
        mProgressBar.setMax(100);
        mProgressBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (!fromUser) {
                    return;
                }
                mPosition = mPlayer.getDuration() * progress / 100;
                Log.i("yyl", "position:" + mPosition + " progress:" + progress);
                mPlayer.seek(mPosition);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });


        mVolumeBar = findViewById(R.id.sb_volume);
        mVolumeBar.setMax(100);
        mVolumeBar.setProgress(50);
        mVolumeBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser) {
                    if (mPlayer != null) {
                        mPlayer.setVolume(progress);
                    }
                    mVolumeTV.setText("音量:" + progress + "%");
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
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

        mPlayer.setOnErrorListener(new OnErrorListener() {
            @Override
            public void onError(int code, String msg) {
                LogUtil.i("yyl", "onError code:" + code + " msg:" + msg);
            }
        });
        mPlayer.setOnCallPcmInfoListener(new OnCallPcmInfoListener() {
            @Override
            public void onCallPcmInfo(byte[] buffer, int bufferSize) {
                Log.i("yyl", "bufferSize:" + bufferSize);
            }
        });
        mPlayer.setOnCallPcmRateListener(new OnCallPcmRateListener() {
            @Override
            public void onCallPcmRate(int sampleRate) {
                Log.i("yyl", "sampleRate:" + sampleRate);
            }
        });
        mPlayer.setVolume(50);
    }

    public void prepare(View view) {
//        mPlayer.setDataSourceAndPrepare("http://cmp3.o2ting.c1om:8081/mp3_32k/100374/873790/bazc001.mp3?ver=TYYDYSB_A1.5.4.2&pno=0000&userName=&audio=1188555&md5=wnVgHti5zygNGNpdnxFZUA&expires=1539941128");
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

    public void volumeLeft(View view) {
        mPlayer.setMute(FF10Player.MUTE_LEFT);
    }

    public void volumeCenter(View view) {
        mPlayer.setMute(FF10Player.MUTE_STEREO);
    }

    public void volumeRight(View view) {
        mPlayer.setMute(FF10Player.MUTE_RIGHT);
    }

    public void speedUpNoPitchUp(View view) {
        mPlayer.setSpeed(1.5f);
        mPlayer.setPitch(1f);
    }

    public void pitchUpNoSpeedUp(View view) {
        mPlayer.setSpeed(1f);
        mPlayer.setPitch(1.5f);
    }

    public void speedUpPitchUp(View view) {
        mPlayer.setSpeed(1.5f);
        mPlayer.setPitch(1.5f);
    }

    public void noSpeedUpNoPitchUp(View view) {
        mPlayer.setSpeed(1f);
        mPlayer.setPitch(1f);
    }

    public void startRecord(View view) {
        File out = new File("/sdcard/testrecord.aac");
        mPlayer.startRecord(out);
    }

    public void pauseRecord(View view) {
        mPlayer.pauseReord();
    }

    public void resumeRecord(View view) {
        mPlayer.resumeRecord();
    }

    public void stopRecord(View view) {
        mPlayer.stopRecord();
    }

    public void cut(View view) {
        mPlayer.setDataSourceAndPrepare("/sdcard/zly.mp3");
        mPlayer.setOnPrepareListener(new OnPreparedListener() {
            @Override
            public void onSuccess() {
                mPlayer.cutAudio();
            }

            @Override
            public void onError() {

            }
        });
    }
}
