
#include <unistd.h>
#include "Y10FFmpeg.h"

void *decodeFFmpeg(void *data) {
    LOGI("ffmpeg decodeFFmpeg");
    Y10FFmpeg *ffmpeg = (Y10FFmpeg *) data;
    ffmpeg->decodeFFmpegThread();
    pthread_exit(&ffmpeg->mDecodeThread);
}

//void *readPacket(void *data) {
//    LOGI("ffmpeg readPacket");
//    Y10FFmpeg *ffmpeg = (Y10FFmpeg *) data;
//    int count = 0;
//    while (ffmpeg->mPlayStatus != NULL && !ffmpeg->mPlayStatus->mExit) {
//        AVPacket *avPacket = av_packet_alloc();
//        if (av_read_frame(ffmpeg->mAVFormatContext, avPacket) == 0) {
//            if (avPacket->stream_index == ffmpeg->mAudio->mStreamIndex) {//如果是音频流
//                //解码操作
//                count++;
//                LOGE("解码第 %d 帧", count);
//                av_packet_free(&avPacket);
//                av_free(avPacket);
//                ffmpeg->mAudio->mY10Queue->putAVPacket(avPacket);
//            } else {
//                av_packet_free(&avPacket);
//                av_free(avPacket);
//            }
//        } else {
//            LOGE("decode finished");
//            av_packet_free(&avPacket);
//            av_free(avPacket);
//            break;
//        }
//    }
//    if (LOG_DEBUG) {
//        LOGD("解码完成");
//    }
//    pthread_exit(&ffmpeg->mReadPacketThread);
//}

Y10FFmpeg::Y10FFmpeg(PlayStatus *playStatus, CallJava *callJava, const char *url) {
    this->mPlayStatus = playStatus;
    this->mCallJava = callJava;
    this->mUrl = url;
}

void Y10FFmpeg::prepare() {
    LOGI("ffmpeg prepare");
    pthread_create(&mDecodeThread, NULL, decodeFFmpeg, this);
}

void Y10FFmpeg::decodeFFmpegThread() {
    LOGI("ffmpeg decodeFFmpegThread");
    //注册
    av_register_all();
    avformat_network_init();
    //申请内存
//    LOGI("1");
    mAVFormatContext = avformat_alloc_context();
    if (!mAVFormatContext) {
        LOGE("alloc formatcontext failed!");
        return;
    }
    //打开url
    LOGI("2 url:%s", mUrl);
    if (avformat_open_input(&mAVFormatContext, mUrl, NULL, NULL) != 0) {
//    if (avio_open(&mAVFormatContext, mUrl, NULL, NULL) != 0) {
        LOGE("invalid url! :%s", mUrl);
        return;
    }
    //找音频流
//    LOGI("3");
    if (avformat_find_stream_info(mAVFormatContext, NULL) < 0) {
        LOGE("can't find stream");
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
//    LOGI("4");
    AVCodec *codec = avcodec_find_decoder(mAudio->mCodecpar->codec_id);
    if (!codec) {
        LOGE("can't find codec");
        return;
    }
    //获取解码器上下文
//    LOGI("5");
    mAudio->mAVCodecContext = avcodec_alloc_context3(codec);
    if (!mAudio->mAVCodecContext) {
        LOGE("can't alloc new decode context");
        return;
    }
    //把解码器信息复制到刚申请的解码器上下文内存中
//    LOGI("6");
    if (avcodec_parameters_to_context(mAudio->mAVCodecContext, mAudio->mCodecpar)) {
        LOGE("can't fill decode context");
        return;
    }
    //用解码器打开流
//    LOGI("7");
    if (avcodec_open2(mAudio->mAVCodecContext, codec, 0) != 0) {
        LOGE("can't open audio stream");
        return;
    }
    mCallJava->onCallPrepared(CHILD_THREAD);
}

void Y10FFmpeg::start() {
    if (mAudio == NULL) {
        LOGE("audio is null");
        return;
    }
    mAudio->start();
//    pthread_create(&mReadPacketThread, NULL, readPacket, this);
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
