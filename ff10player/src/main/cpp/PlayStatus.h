#ifndef INC_10FFMPEGPLAYER_PLAYSTATUS_H
#define INC_10FFMPEGPLAYER_PLAYSTATUS_H


class PlayStatus {
public:
    bool mExited;//判断播放是否已经停止
    bool mLoad = true;
    bool mSeeking = false;//是否正在seek
    bool mIsPlaying = false;//是否正在播放

public:
    PlayStatus();
    ~PlayStatus();
};


#endif //INC_10FFMPEGPLAYER_PLAYSTATUS_H
