package com.stan.ff10player;

import android.util.Log;

import com.stan.ff10player.listener.OnCallPcmInfoListener;
import com.stan.ff10player.listener.OnCallPcmRateListener;
import com.stan.ff10player.listener.OnCompleteListener;
import com.stan.ff10player.listener.OnErrorListener;
import com.stan.ff10player.listener.OnLoadListener;
import com.stan.ff10player.listener.OnPreparedListener;
import com.stan.ff10player.listener.OnResumePauseListener;
import com.stan.ff10player.listener.OnTimeChangedListener;

import java.io.File;

public class FF10Player {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-58");
        System.loadLibrary("avdevice-58");
        System.loadLibrary("avfilter-7");
        System.loadLibrary("avformat-58");
        System.loadLibrary("avutil-56");
        System.loadLibrary("postproc-55");
        System.loadLibrary("swresample-3");
        System.loadLibrary("swscale-5");
    }
    public static final int ERROR_CAN_NOT_OPEN_INPUT = 1001;
    public static final int ERROR_CAN_NOT_FIND_STREAM = 1002;
    public static final int ERROR_CAN_NOT_FIND_CODEC = 1003;

    public static final int MUTE_RIGHT = 0;
    public static final int MUTE_LEFT = 1;
    public static final int MUTE_STEREO = 2;

    private static final int STATE_IDLE = 0;
    private static final int STATE_PREPARING = STATE_IDLE + 1;
    private static final int STATE_PREPARED = STATE_PREPARING + 1;
    private static final int STATE_PLAYING = STATE_PREPARED + 1;
    private static final int STATE_PAUSED = STATE_PLAYING + 1;
    private static final int STATE_STOPPING = STATE_PAUSED + 1;
    private static final int STATE_STOPPED = STATE_STOPPING + 1;
    private static final int STATE_RELEASED = STATE_STOPPED + 1;
    private static final int STATE_SEEKING = STATE_RELEASED + 1;


    private OnPreparedListener mOnPrepareListener;
    private OnResumePauseListener mOnResumePauseListener;
    private OnTimeChangedListener mOnTimeChangedListener;
    private OnLoadListener mOnLoadListener;
    private OnErrorListener mOnErrorListener;
    private OnCallPcmInfoListener mOnCallPcmInfoListener;
    private OnCallPcmRateListener mOnCallPcmRateListener;
    private OnCompleteListener mOnCompleteListener;

    private String mSourceUrl;
    private int mDuration;
    private FF10Encoder mAACEncoder;
    private int mPlayState;
    public static boolean initmediacodec = false;

    /* 音量值0~100 */
    private int mVolume;

    public FF10Player() {
        mAACEncoder = new FF10Encoder(this);
        setState(STATE_IDLE);
    }

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

    public void setOnCallPcmInfoListener(OnCallPcmInfoListener listener) {
        this.mOnCallPcmInfoListener = listener;
    }

    public void setOnCallPcmRateListener(OnCallPcmRateListener listener) {
        this.mOnCallPcmRateListener = listener;
    }

    public void setOnCompleteListener(OnCompleteListener listener) {
        this.mOnCompleteListener = listener;
    }

    public void setDataSourceAndPrepare(String url) {
        mSourceUrl = url;
        prepare();
    }

    private void prepare() {
        nPrepare(mSourceUrl);
        setState(STATE_PREPARING);
    }

    public void start() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                setState(STATE_PLAYING);
                nStart();
            }
        }).start();
    }

    public void resume() {
        nResume();
        setState(STATE_PLAYING);
        if (mOnResumePauseListener != null) {
            mOnResumePauseListener.onResumePauseChanged(false);
        }
    }

    public void pause() {
        nPause();
        setState(STATE_PAUSED);
        if (mOnResumePauseListener != null) {
            mOnResumePauseListener.onResumePauseChanged(true);
        }
    }

    public void stop() {
        nStop();
        setState(STATE_STOPPED);
    }

    public void seek(int secs) {
        nSeek(secs);
        setState(STATE_SEEKING);
    }

    public void setVolume(int percent) {
        nSetVolume(percent);
    }

    public void setMute(int mute) {
        nSetMute(mute);
    }

    public void setSpeed(float speed) {
        nSetSpeed(speed);
    }

    public void setPitch(float pitch) {
        nSetPitch(pitch);
    }

    public boolean isPlaying() {
        return nIsPlaying();
    }

    /* 时间是秒 */
    public int getCurrentPosition() {
        return nGetCurrentPosition();
    }

    public void startRecord(File out) {
        if (!initmediacodec) {
            int sampleRate = nGetSampleRate();
            if (sampleRate > 0) {
                initmediacodec = true;
                mAACEncoder.initMediaCodec(sampleRate, out);
            }
            nStopStartRecord(true);
        }
    }

    public void pauseReord() {
        nStopStartRecord(false);
    }

    public void resumeRecord() {
        nStopStartRecord(true);
    }


    public void stopRecord() {
        if (initmediacodec) {
            nStopStartRecord(false);
            mAACEncoder.releaseMediaCodec();
        }
    }

    public int getSampleRate() {
        return nGetSampleRate();
    }

    public void cutAudio() {
        if (nCutAudio(20, 30, true)) {
            start();
        } else {
            stop();
            onCallError(2001, "cutaudio params is wrong");
        }
    }

    private void setState(int state) {
        mPlayState = state;
    }

    /* called from jni */
    private void onCallPrepared() {
        setState(STATE_PREPARED);
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
        setState(STATE_IDLE);
        if (mOnErrorListener != null) {
            mOnErrorListener.onError(code, msg);
        }
    }

    /* called from jni */
    private void onCallComplete() {
        stop();
        if (mOnCompleteListener != null) {
            mOnCompleteListener.onComplete();
        }
        Log.i("yyl", "onCallComplete java");
    }

    /* called from jni */
    private void pcm2aac(int size, byte[] buffer) {
        Log.i("yyl", "pcm2aac size:" + size);
        mAACEncoder.pcm2aac(size, buffer);
    }

    private void releaseMediaCodec() {
        mAACEncoder.releaseMediaCodec();
    }

    private void onCallPcmInfo(byte[] buffer, int bufferSize) {
        if (mOnCallPcmInfoListener != null) {
            mOnCallPcmInfoListener.onCallPcmInfo(buffer, bufferSize);
        }
    }

    private void onCallPcmRate(int sampleRate) {
        if (mOnCallPcmRateListener != null) {
            mOnCallPcmRateListener.onCallPcmRate(sampleRate);
        }
    }

    /* called from jni */
    private void onCallStart() {
        setState(STATE_PLAYING);
    }

    /* called from jni */
    private void onCallStop() {
        if(mOnCompleteListener != null) {
            mOnCompleteListener.onComplete();
        }
    }

    private native int nPrepare(String url);

    private native int nStart();

    private native int nResume();

    private native int nPause();

    private native int nStop();

    private native int nSeek(int secs);

    private native int nGetDuration();

    private native int nSetVolume(int percent);

    private native int nSetMute(int mute);

    private native int nSetPitch(float pitch);

    private native int nSetSpeed(float speed);

    private native int nGetSampleRate();

    private native int nGetCurrentPosition();

    private native boolean nIsPlaying();

    private native void nStopStartRecord(boolean start);

    private native boolean nCutAudio(int start, int end, boolean showPcm);

}
