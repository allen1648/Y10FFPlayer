package com.stan.ff10player;

import android.util.Log;

import com.stan.ff10player.listener.OnLoadListener;
import com.stan.ff10player.listener.OnPreparedListener;
import com.stan.ff10player.listener.OnResumePauseListener;
import com.stan.ff10player.listener.OnTimeChangedListener;

public class FF10Player {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }

    private String mSourceUrl;
    private OnPreparedListener mOnPrepareListener;
    private OnResumePauseListener mOnResumePauseListener;
    private OnTimeChangedListener mOnTimeChangedListener;
    private OnLoadListener mOnLoadListener;

    public void setOnPrepareListener(OnPreparedListener listener) {
        this.mOnPrepareListener = listener;
    }

    public void setOnResumePauseListener(OnResumePauseListener listener) {
        this.mOnResumePauseListener = listener;
    }

    public void setOnTimeChangedListener(OnTimeChangedListener listener) {
        this.mOnTimeChangedListener = listener;
    }

    public void setOnLoadListener(OnLoadListener listener) {
        this.mOnLoadListener = listener;
    }

    public void setDataSourceAndPrepare(String url) {
        mSourceUrl = url;
        prepare();
    }

    private void prepare() {
        onCallLoaded(true);
        nPrepare(mSourceUrl);
    }

    public void start() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                nStart();
            }
        }).start();
    }

    public void resume() {
        nResume();
        if (mOnResumePauseListener != null) {
            mOnResumePauseListener.onResumePauseChanged(false);
        }
    }

    public void pause() {
        nPause();
        if (mOnResumePauseListener != null) {
            mOnResumePauseListener.onResumePauseChanged(true);
        }
    }

    /* called from jni */
    private void onCallPrepared() {
        if (mOnPrepareListener != null) {
            mOnPrepareListener.onSuccess();
        }
    }

    /* called from jni */
    private void onCallLoaded(boolean load) {
        if (mOnLoadListener != null) {
            mOnLoadListener.onLoad(load);
        }
    }

    /* called from jni */
    private void onCallTimeChanged(int currentTime, int totalTime) {
        Log.i("yyl", "onCallTimeChanged..."+currentTime+" "+totalTime);
        if (mOnTimeChangedListener != null) {
            mOnTimeChangedListener.onTimeChanged(currentTime, totalTime);
        }
    }

    private native int nPrepare(String url);

    private native int nStart();

    private native int nResume();

    private native int nPause();
}
