#include <unistd.h>
#include "Y10Queue.h"

Y10Queue::Y10Queue(PlayStatus *playStatus) {
    this->mPlayStatus = playStatus;
//    LOGI("queue size: %d", mY10Queue.size());
    pthread_mutex_init(&mThreadMutex, NULL);
    pthread_cond_init(&mThreadCond, NULL);
}

Y10Queue::~Y10Queue() {
    releaseQueue();
    pthread_mutex_destroy(&mThreadMutex);
    pthread_cond_destroy(&mThreadCond);
}

int Y10Queue::putAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mThreadMutex);
    mPacketQueue.push(packet);
//    LOGE("after push size:%d",mPacketQueue.size());
    pthread_cond_signal(&mThreadCond);
    pthread_mutex_unlock(&mThreadMutex);
    return 0;
}

int Y10Queue::getAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mThreadMutex);
    while (mPlayStatus != NULL && !mPlayStatus->mExited) {
        if (mPacketQueue.size() > 0) {
            AVPacket *front = mPacketQueue.front();//得到对列头但不出栈
            if (av_packet_ref(packet, front) == 0) {//把得到的AVPacket引用附给packet
                mPacketQueue.pop();
            }
            av_packet_free(&front);//先把pkt中的内容清空，然后再把指针清空，让pkt彻底无法使用了，如果需要重新使用，需要重新分配内存
            av_free(front);//释放指针所指的那块内存
            front = NULL;
            break;
        } else {
            if (LOG_DEBUG) {
//                LOGI("wait for resource ......");
            }
            pthread_cond_wait(&mThreadCond, &mThreadMutex);
        }
    }
    pthread_mutex_unlock(&mThreadMutex);
    return 0;
}

int Y10Queue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mThreadMutex);
    size = mPacketQueue.size();
    pthread_mutex_unlock(&mThreadMutex);
    return size;
}

void Y10Queue::releaseQueue(){
    pthread_cond_signal(&mThreadCond);//getPacket的时候有可能会等待
    pthread_mutex_unlock(&mThreadMutex);
    while (!mPacketQueue.empty()) {
        AVPacket *packet = mPacketQueue.front();
        mPacketQueue.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mThreadMutex);
}
