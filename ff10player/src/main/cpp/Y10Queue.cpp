#include <unistd.h>
#include "Y10Queue.h"

Y10Queue::Y10Queue(PlayStatus *playStatus) {
    this->mPlayStatus = playStatus;
//    LOGI("queue size: %d", mY10Queue.size());
    pthread_mutex_init(&mThreadMutex, NULL);
    pthread_cond_init(&mThreadCond, NULL);
}

Y10Queue::~Y10Queue() {

}

int Y10Queue::putAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mThreadMutex);
    mPacketQueue.push(packet);
    LOGE("after push size:%d",mPacketQueue.size());
    pthread_cond_signal(&mThreadCond);
    pthread_mutex_unlock(&mThreadMutex);
    return 0;
}

int Y10Queue::getAVPacket(AVPacket *packet) {
//    LOGI("before get lock");
    pthread_mutex_lock(&mThreadMutex);
//    LOGI("after get lock");
    while (mPlayStatus != NULL && !mPlayStatus->mExit) {
        if (mPacketQueue.size() > 0) {
            AVPacket *front = mPacketQueue.front();//得到对列头但不出栈
            if (av_packet_ref(packet, front) == 0) {//把得到的AVPacket引用附给packet
//                LOGI("before get pop");
                mPacketQueue.pop();
                LOGI("after get pop size:%d", mPacketQueue.size());
            }
            av_packet_free(&front);//先把pkt中的内容清空，然后再把指针清空，让pkt彻底无法使用了，如果需要重新使用，需要重新分配内存
            av_free(front);//释放指针所指的那块内存
            front = NULL;
            break;
        } else {
            if (LOG_DEBUG) {
//                LOGI("wait for resource ......");
            }
//            LOGI("before get wait");
            pthread_cond_wait(&mThreadCond, &mThreadMutex);
            LOGI("after get wait");
        }
    }
//    LOGI("before get unlock");
    pthread_mutex_unlock(&mThreadMutex);
//    LOGI("after get unlock");
    return 0;
}

int Y10Queue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mThreadMutex);
    size = mPacketQueue.size();
    pthread_mutex_unlock(&mThreadMutex);
    return size;
}
