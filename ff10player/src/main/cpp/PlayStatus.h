#ifndef INC_10FFMPEGPLAYER_PLAYSTATUS_H
#define INC_10FFMPEGPLAYER_PLAYSTATUS_H


class PlayStatus {
public:
    bool mExit;//判断播放是否停止
    bool mLoad = true;
    bool mSeeking = false;//是否正在seek

public:
    PlayStatus();
    ~PlayStatus();
};


#endif //INC_10FFMPEGPLAYER_PLAYSTATUS_H
