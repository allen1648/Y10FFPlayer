package com.stan.y10ffplayer;

public interface MediaPlayerEventListener {

	public void onCompletion();
	
	public void onSeekComplete();
	
	public boolean onError(int what, int extra);
}
