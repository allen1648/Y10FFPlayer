#ifndef INC_10FFMPEGPLAYER_Y10FFMPEG_H
#define INC_10FFMPEGPLAYER_Y10FFMPEG_H

#include <pthread.h>
#include "AndroidLog.h"
#include "Y10Audio.h"
#include "CallJava.h"

extern "C" {
#include <libavformat/avformat.h>
};

class Y10FFmpeg {
public:
    pthread_t mDecodeThread;//解码线程
    pthread_t mReadPacketThread;//解码线程
    const char *mUrl = NULL;
    AVFormatContext *mAVFormatContext = NULL;
    Y10Audio *mAudio = NULL;//ffmpeg prepare的时候会创建
    CallJava *mCallJava = NULL;
    PlayStatus *mPlayStatus = NULL;
public:
    Y10FFmpeg(PlayStatus *playStatus, CallJava *callJava, const char *url);

    ~Y10FFmpeg();

    void prepare();//开启一个线程解码
    void decodeFFmpegThread();//解码主要逻辑
    void start();//
    void resume();
    void pause();
};

#endif //INC_10FFMPEGPLAYER_Y10FFMPEG_H
