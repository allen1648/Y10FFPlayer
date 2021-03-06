#ifndef INC_10FFMPEGPLAYER_Y10FFMPEG_H
#define INC_10FFMPEGPLAYER_Y10FFMPEG_H

#include <pthread.h>
#include <unistd.h>
#include "AndroidLog.h"
#include "Y10Audio.h"
#include "CallJava.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};

class Y10FFmpeg {
public:
    pthread_t mDecodeThread;//解码线程
    pthread_mutex_t mInitMutex;//初始化的锁
    pthread_mutex_t mSeekMutex;//seek的锁
    AVFormatContext *mAVFormatContext = NULL;
    Y10Audio *mAudio = NULL;//ffmpeg prepare的时候会创建
    CallJava *mCallJava = NULL;
    PlayStatus *mPlayStatus = NULL;
    const char *mUrl = NULL;
    bool mPrepareExit = false;
    int mDuration = 0;
    int mVolume = 0;
public:
    Y10FFmpeg(PlayStatus *playStatus, CallJava *callJava, const char *url);

    ~Y10FFmpeg();

    void prepare();//开启一个线程解码
    void decodeFFmpegThread();//解码主要逻辑
    void start();//
    void resume();
    void pause();
    void release();
    void seek(int64_t secs);
    void setVolume(int value);
    void setMute(int mute);
    void setPitch(float pitch);
    void setSpeed(float speed);
    int getSampleRate();
    bool isPlaying();
    int getCurrentPosition();
    void startStopRecord(bool start);

    bool cutAudio(int start, int end, bool showPcm);
};

#endif //INC_10FFMPEGPLAYER_Y10FFMPEG_H
