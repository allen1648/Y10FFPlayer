#include "Y10BufferQueue.h"
#include "AndroidLog.h"

Y10BufferQueue::Y10BufferQueue(PlayStatus *playStatus) {
    this->mPlayStatus = playStatus;
    pthread_mutex_init(&mBufferMutex, NULL);
    pthread_cond_init(&mBufferCond, NULL);
}

Y10BufferQueue::~Y10BufferQueue() {
    mPlayStatus = NULL;
    pthread_mutex_destroy(&mBufferMutex);
    pthread_cond_destroy(&mBufferCond);
    if (LOG_DEBUG) {
        LOGE("WlBufferQueue 释放完了");
    }
}

void Y10BufferQueue::release() {
    if (LOG_DEBUG) {
        LOGE("WlBufferQueue::release");
    }
    notifyThread();
    clearBuffer();
    if (LOG_DEBUG) {
        LOGE("WlBufferQueue::release success");
    }
}

int Y10BufferQueue::putBuffer(SAMPLETYPE *buffer, int size) {
    pthread_mutex_lock(&mBufferMutex);
    PCMBean *pcmBean = new PCMBean(buffer, size);
    mBufferQueue.push_back(pcmBean);
    pthread_cond_signal(&mBufferCond);
    pthread_mutex_unlock(&mBufferMutex);
    return 0;
}

int Y10BufferQueue::getBuffer(PCMBean **pcmBean) {
    pthread_mutex_lock(&mBufferMutex);
    while (mPlayStatus != NULL && !mPlayStatus->mExited) {
        if (mBufferQueue.size() > 0) {
            *pcmBean = mBufferQueue.front();
            mBufferQueue.pop_front();
            break;
        } else {
            if (!mPlayStatus->mExited) {
                pthread_cond_wait(&mBufferCond, &mBufferMutex);
            }
        }
    }
    pthread_mutex_unlock(&mBufferMutex);
    return 0;
}

int Y10BufferQueue::clearBuffer() {
    pthread_cond_signal(&mBufferCond);
    pthread_mutex_lock(&mBufferMutex);
    while (!mBufferQueue.empty()) {
        PCMBean *pcmBean = mBufferQueue.front();
        mBufferQueue.pop_front();
        delete (pcmBean);
    }
    pthread_mutex_unlock(&mBufferMutex);
    return 0;
}

int Y10BufferQueue::getBufferSize() {
    int size = 0;
    pthread_mutex_lock(&mBufferMutex);
    size = mBufferQueue.size();
    pthread_mutex_unlock(&mBufferMutex);
    return size;
}


int Y10BufferQueue::notifyThread() {
    pthread_cond_signal(&mBufferCond);
    return 0;
}