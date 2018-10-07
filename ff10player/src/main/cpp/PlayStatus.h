#ifndef INC_10FFMPEGPLAYER_PLAYSTATUS_H
#define INC_10FFMPEGPLAYER_PLAYSTATUS_H


class PlayStatus {
public:
    bool mExit;//是否停止从
    bool mLoad = true;
    bool mSeeking = false;

public:
    PlayStatus();
    ~PlayStatus();
};


#endif //INC_10FFMPEGPLAYER_PLAYSTATUS_H
