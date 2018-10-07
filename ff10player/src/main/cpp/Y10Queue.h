//
// Created by stan on 18/10/2.
//

#ifndef INC_10FFMPEGPLAYER_Y10QUEUE_H
#define INC_10FFMPEGPLAYER_Y10QUEUE_H

#include <pthread.h>
#include "queue"
#include "PlayStatus.h"
#include "AndroidLog.h"
extern "C"{
#include <libavcodec/avcodec.h>
};

class Y10Queue {
public:
    std::queue<AVPacket *> mPacketQueue;
    pthread_mutex_t mThreadMutex;
    pthread_cond_t mThreadCond;
    PlayStatus *mPlayStatus = NULL;
public:
    Y10Queue(PlayStatus *playStatus);
    ~Y10Queue();
    int putAVPacket(AVPacket *packet);
    int getAVPacket(AVPacket *packet);
    void releaseQueue();
    int getQueueSize();
};


#endif //INC_10FFMPEGPLAYER_Y10QUEUE_H
