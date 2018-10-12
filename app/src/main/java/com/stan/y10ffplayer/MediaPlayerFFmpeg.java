package com.stan.y10ffplayer;

import android.content.Context;

import com.stan.ff10player.FF10Player;

public class MediaPlayerFFmpeg implements MediaPlayerInterface {
    private FF10Player mPlayer;

    public MediaPlayerFFmpeg() {
        mPlayer = new FF10Player();
    }

    @Override
    public void setWakeMode(Context context, int mode) {

    }

    @Override
    public void setAudioStreamType(int type) {

    }

    @Override
    public boolean isPlaying() {
        return mPlayer.isPlaying();
    }

    @Override
    public void setVolume(float left, float right) {
        mPlayer.setVolume((int) left);
    }

    @Override
    public int getCurrentPosition() {
        return mPlayer.getCurrentPosition();
    }

    @Override
    public int getDuration() {
        return mPlayer.getDuration();
    }

    @Override
    public void reset() {

    }

    @Override
    public void setDataSourceAndPrepare(String url) throws Exception {

    }

    @Override
    public void stop() {
        mPlayer.stop();
    }

    @Override
    public void pause() {
        mPlayer.pause();
    }

    @Override
    public void start() {
        mPlayer.resume();
    }

    @Override
    public void seekTo(int msec) {

    }

    @Override
    public void release() {

    }

    @Override
    public void setEventListener(MediaPlayerEventListener listener) {

    }

    @Override
    public void setPlaySpeed(float speed) {
        mPlayer.setSpeed(speed);
    }
}
