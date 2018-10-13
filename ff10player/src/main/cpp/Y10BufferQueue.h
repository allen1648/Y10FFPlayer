#ifndef Y10FFPLAYER_Y10BUFFERQUEUE_H
#define Y10FFPLAYER_Y10BUFFERQUEUE_H

#include "deque"
#include "PlayStatus.h"
#include "PCMBean.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include "pthread.h"
};

class Y10BufferQueue {
public:
    std::deque<PCMBean *> mBufferQueue;
    pthread_mutex_t mBufferMutex;
    pthread_cond_t mBufferCond;
    PlayStatus *mPlayStatus = NULL;
public:
    Y10BufferQueue(PlayStatus *playStatus);

    ~Y10BufferQueue();

    int putBuffer(SAMPLETYPE *buffer, int size);

    int getBuffer(PCMBean **pcmBean);

    int clearBuffer();

    void release();

    int getBufferSize();

    int notifyThread();
};


#endif //Y10FFPLAYER_Y10BUFFERQUEUE_H
