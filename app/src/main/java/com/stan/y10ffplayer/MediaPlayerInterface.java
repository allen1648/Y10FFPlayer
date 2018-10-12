package com.stan.y10ffplayer;

import android.content.Context;

public interface MediaPlayerInterface {
	
	void setWakeMode(Context context, int mode);
	
	void setAudioStreamType(int type);
	
	boolean isPlaying();
	
	void setVolume(float left, float right);
	
	int getCurrentPosition();
	
	int getDuration();
	
	void reset();

	void setDataSourceAndPrepare(String url) throws Exception;
	
	void stop();
	
	void pause();
	
	void start();
	
	void seekTo(int msec);
	
	void release();
	
	void setEventListener(MediaPlayerEventListener listener);

	/** 倍速播放 */
	void setPlaySpeed(float speed);
}
