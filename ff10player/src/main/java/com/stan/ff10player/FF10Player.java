package com.stan.ff10player;

import android.util.Log;

import com.stan.ff10player.listener.OnErrorListener;
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

    public static final int MUTE_RIGHT = 0;
    public static final int MUTE_LEFT = 1;
    public static final int MUTE_STEREO = 2;
    private OnPreparedListener mOnPrepareListener;
    private OnResumePauseListener mOnResumePauseListener;
    private OnTimeChangedListener mOnTimeChangedListener;
    private OnLoadListener mOnLoadListener;
    private OnErrorListener mOnErrorListener;
    private String mSourceUrl;
    private int mDuration;

    /* 音量值0~100 */
    private int mVolume;

    public int getDuration() {
        return mDuration;
    }

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

    public void setOnErrorListener(OnErrorListener listener) {
        this.mOnErrorListener = listener;
    }

    public void setDataSourceAndPrepare(String url) {
        mSourceUrl = url;
        release();
        prepare();
    }

    private void release() {
        nRelease();
    }

    private void prepare() {
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

    public void stop() {
        nStop();
    }

    public void seek(int secs) {
        nSeek(secs);
    }

    public void setVolume(int percent) {
        nSetVolume(percent);
    }

    public void setMute(int mute) {
        nSetMute(mute);
    }

    /* called from jni */
    private void onCallPrepared() {
        mDuration = nGetDuration();
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
        if (mOnTimeChangedListener != null) {
            mOnTimeChangedListener.onTimeChanged(currentTime, totalTime);
        }
    }

    /* called from jni */
    private void onCallError(int code, String msg) {
        if (mOnErrorListener != null) {
            mOnErrorListener.onError(code, msg);
        }
    }

    /* called from jni */
    private void onCallComplete() {
        stop();
        Log.i("yyl", "onCallComplete java");
    }

    private native int nPrepare(String url);

    private native int nStart();

    private native int nResume();

    private native int nPause();

    private native int nStop();

    private native int nSeek(int secs);

    private native int nRelease();

    private native int nGetDuration();

    private native int nSetVolume(int percent);

    private native int nSetMute(int mute);
}
