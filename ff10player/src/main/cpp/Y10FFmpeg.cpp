#include "Y10FFmpeg.h"

void *decodeFFmpeg(void *data) {
    LOGI("ffmpeg decodeFFmpeg");
    Y10FFmpeg *ffmpeg = (Y10FFmpeg *) data;
    ffmpeg->decodeFFmpegThread();
    pthread_exit(&ffmpeg->mDecodeThread);
}

int avformatCallback(void *data) {
    LOGI("avformatCallback");
    Y10FFmpeg *ffmpeg = (Y10FFmpeg *)(data);
    if(ffmpeg->mDecodeExit) {
        return AVERROR_EOF;
    }
    return 0;
}

Y10FFmpeg::Y10FFmpeg(PlayStatus *playStatus, CallJava *callJava, const char *url) {
    this->mPlayStatus = playStatus;
    this->mCallJava = callJava;
    this->mUrl = url;
    pthread_mutex_init(&mInitMutex, NULL);
}

Y10FFmpeg::~Y10FFmpeg() {
    pthread_mutex_destroy(&mInitMutex);
}

void Y10FFmpeg::prepare() {
    LOGI("ffmpeg prepare");
    pthread_create(&mDecodeThread, NULL, decodeFFmpeg, this);
}

void Y10FFmpeg::decodeFFmpegThread() {
    LOGI("ffmpeg decodeFFmpegThread");
    pthread_mutex_lock(&mInitMutex);
    //注册
    av_register_all();
    avformat_network_init();
    //申请内存
    mAVFormatContext = avformat_alloc_context();
    //这个回调是为了防止无限等待,比如打开网络url
    mAVFormatContext->interrupt_callback.callback = avformatCallback;
    mAVFormatContext->interrupt_callback.opaque = this;
    if (!mAVFormatContext) {
        LOGE("alloc formatcontext failed!");
        pthread_mutex_unlock(&mInitMutex);
        mDecodeExit = true;
        return;
    }
    //打开url
    LOGI("2 url:%s", mUrl);
    if (avformat_open_input(&mAVFormatContext, mUrl, NULL, NULL) != 0) {
        LOGE("invalid url! :%s", mUrl);
        pthread_mutex_unlock(&mInitMutex);
        mDecodeExit = true;
        mCallJava->onCallError(CHILD_THREAD, 1001, "open input failed");
        return;
    }
    //找音频流
    if (avformat_find_stream_info(mAVFormatContext, NULL) < 0) {
        pthread_mutex_unlock(&mInitMutex);
        mDecodeExit = true;
        mCallJava->onCallError(CHILD_THREAD, 1002, "can't find stream");
        return;
    }
    int streamsCount = mAVFormatContext->nb_streams;
    for (int i = 0; i < streamsCount; i++) {
        if (mAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (mAudio == NULL) {
                mAudio = new Y10Audio(mPlayStatus,
                                      mAVFormatContext->streams[i]->codecpar->sample_rate,
                                      mCallJava);
                mAudio->mStreamIndex = i;
                mAudio->mCodecpar = mAVFormatContext->streams[i]->codecpar;
                mAudio->mDuration = mAVFormatContext->duration / AV_TIME_BASE;
                mAudio->mTimeBase = mAVFormatContext->streams[i]->time_base;
            }
        }
    }
    //找解码器
    AVCodec *codec = avcodec_find_decoder(mAudio->mCodecpar->codec_id);
    if (!codec) {
        pthread_mutex_unlock(&mInitMutex);
        mDecodeExit = true;
        mCallJava->onCallError(CHILD_THREAD, 1003, "can't find codec");
        return;
    }
    //获取解码器上下文
    mAudio->mAVCodecContext = avcodec_alloc_context3(codec);
    if (!mAudio->mAVCodecContext) {
        pthread_mutex_unlock(&mInitMutex);
        mDecodeExit = true;
        mCallJava->onCallError(CHILD_THREAD, 1004, "can't alloc new decode context");
        return;
    }
    //把解码器信息复制到刚申请的解码器上下文内存中
    if (avcodec_parameters_to_context(mAudio->mAVCodecContext, mAudio->mCodecpar)) {
        pthread_mutex_unlock(&mInitMutex);
        mDecodeExit = true;
        mCallJava->onCallError(CHILD_THREAD, 1005, "can't fill decode context");
        return;
    }
    //用解码器打开流
    if (avcodec_open2(mAudio->mAVCodecContext, codec, 0) != 0) {
        LOGE("can't open audio stream");
        pthread_mutex_unlock(&mInitMutex);
        mDecodeExit = true;
        mCallJava->onCallError(CHILD_THREAD, 1006, "can't open audio stream");
        return;
    }

    if(mCallJava != NULL) {
        if(mPlayStatus != NULL && !mPlayStatus->mExit) {
            mCallJava->onCallPrepared(CHILD_THREAD);
        } else {
            mDecodeExit = true;
        }
    }

    pthread_mutex_unlock(&mInitMutex);
}

void Y10FFmpeg::start() {
    if (mAudio == NULL) {
        LOGE("audio is null");
        return;
    }
    mAudio->start();
    int count = 0;
    while (mPlayStatus != NULL && !mPlayStatus->mExit) {
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(mAVFormatContext, avPacket) == 0) {
            if (avPacket->stream_index == mAudio->mStreamIndex) {//如果是音频流
                //解码操作
                count++;
//                LOGE("解码第 %d 帧", count);
                mAudio->mY10Queue->putAVPacket(avPacket);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
            }
        } else {
            LOGE("decode finished");
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            while (mPlayStatus != NULL && !mPlayStatus->mExit) {//缓存还有数据要播放
                if (mAudio->mY10Queue->getQueueSize() > 0) {
                    continue;
                } else {
                    mPlayStatus->mExit = true;
                    break;
                }
            }
        }
    }
    mDecodeExit = true;
    if (LOG_DEBUG) {
        LOGD("解码完成");
    }
}

void Y10FFmpeg::resume() {
    if (mAudio != NULL) {
        LOGI("ffmpeg resume");
        mAudio->resume();
    }
}

void Y10FFmpeg::pause() {
    if (mAudio != NULL) {
        LOGI("ffmpeg pause");
        mAudio->pause();
    }
}

void Y10FFmpeg::release() {
    if(LOG_DEBUG) {
        LOGI("release ffmpeg");
    }

    if(mPlayStatus->mExit) {
        return;
    }

    mPlayStatus->mExit = true;
    pthread_mutex_lock(&mInitMutex);

    //防止无限等待的处理
    int sleepCount = 0;
    while (!mDecodeExit) {
        if(sleepCount > 1000) {
            mDecodeExit = true;
        }

        if(LOG_DEBUG) {
            LOGE("wait ffmpeg exit %d", sleepCount);
        }
        sleepCount++;
        av_usleep(1000 * 10);//暂停10毫秒
    }

    if(mAudio != NULL) {
        mAudio->release();
        delete(mAudio);
        mAudio = NULL;
    }

    if(mAVFormatContext != NULL) {
        avformat_close_input(&mAVFormatContext);
        avformat_free_context(mAVFormatContext);
        mAVFormatContext = NULL;
    }

    if(mCallJava != NULL) {
        mCallJava = NULL;
    }

    if(mPlayStatus != NULL) {
        mPlayStatus = NULL;
    }

    pthread_mutex_unlock(&mInitMutex);
}
