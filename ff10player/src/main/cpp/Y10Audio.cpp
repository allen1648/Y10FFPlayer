#include "Y10Audio.h"

FILE *outfile = fopen("/sdcard/outpcm.pcm", "w");

Y10Audio::Y10Audio(PlayStatus *playStatus, int sampleRate, CallJava *callJava) {
    this->mPlayStatus = playStatus;
    this->mSampleRate = sampleRate;
    this->mCallJava = callJava;
    mY10Queue = new Y10Queue(playStatus);
    mResampleBuffer = (uint8_t *) av_malloc(sampleRate * 2 * 2);
}

Y10Audio::~Y10Audio() {
}

int Y10Audio::resampleAudio() {
//    LOGI("resample");
    while (mPlayStatus != NULL && !mPlayStatus->mExit) {
        if (mY10Queue->getQueueSize() == 0) {//加载中
            if (!mPlayStatus->mLoad) {
                mPlayStatus->mLoad = true;
                mCallJava->onCallLoad(CHILD_THREAD, true);
            }
            continue;
        } else {
            if (mPlayStatus->mLoad) {
                mPlayStatus->mLoad = false;
                mCallJava->onCallLoad(CHILD_THREAD, false);
            }
        }

        mAVPacket = av_packet_alloc();
        if (mY10Queue->getAVPacket(mAVPacket) != 0) {
            //从队列获取AVPacket失败的情况
            av_packet_free(&mAVPacket);
            av_free(mAVPacket);
            mAVPacket = NULL;
            continue;
        }

        ret = avcodec_send_packet(mAVCodecContext, mAVPacket);//把AVPacket数据集成到avCodecContext中
        if (ret != 0) {//失败
            av_packet_free(&mAVPacket);
            av_free(mAVPacket);
            mAVPacket = NULL;
            continue;
        }

        //开始将AVPacket转成AVFrame pcm数据
        mAVFrame = av_frame_alloc();
        ret = avcodec_receive_frame(mAVCodecContext, mAVFrame);
        if (ret == 0) {
            //处理错误情况
            if (mAVFrame->channels > 0 && mAVFrame->channel_layout == 0) {
                mAVFrame->channel_layout = av_get_default_channel_layout(mAVFrame->channels);
            } else if (mAVFrame->channels == 0 && mAVFrame->channel_layout > 0) {
                mAVFrame->channels = av_get_channel_layout_nb_channels(mAVFrame->channel_layout);
            }

            SwrContext *swr_ctx = NULL;
            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, mAVFrame->sample_rate,
                    mAVFrame->channel_layout, (AVSampleFormat) mAVFrame->format,
                    mAVFrame->sample_rate,
                    NULL, NULL);

            if (!swr_ctx || swr_init(swr_ctx) < 0) {//初始化失败
                av_packet_free(&mAVPacket);
                av_free(mAVPacket);
                mAVPacket = NULL;

                av_frame_free(&mAVFrame);
                av_free(mAVFrame);
                mAVFrame = NULL;
                if (swr_ctx != NULL) {
                    swr_free(&swr_ctx);
                    swr_ctx = NULL;
                }
                continue;
            }

            //计算采样个数,返回这个AVFrame里每个通道的样本输出
            int nb = swr_convert(
                    swr_ctx,
                    &mResampleBuffer,
                    mAVFrame->nb_samples,//采样个数
                    (const uint8_t **) mAVFrame->data,//pcm
                    mAVFrame->nb_samples);

            //输出声道
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            mDataSize = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
//            LOGE("data_size is %d nb=%d", mDataSize, nb);
//            fwrite(&mResampleBuffer,1,mDataSize,outfile);

            //计算时间
            mNowTime = mAVFrame->pts * av_q2d(mTimeBase);//当前播放帧数*每一帧所占时间就是当前播放时间
            if(mNowTime < mClockTime) {
                mNowTime = mClockTime;
            }
            mClockTime = mNowTime;

            //释放资源
            av_packet_free(&mAVPacket);
            av_free(mAVPacket);
            mAVPacket = NULL;

            av_frame_free(&mAVFrame);
            av_free(mAVFrame);
            mAVFrame = NULL;

            swr_free(&swr_ctx);
            swr_ctx = NULL;
            break;
        } else {
            av_packet_free(&mAVPacket);
            av_free(mAVPacket);
            mAVPacket = NULL;

            av_frame_free(&mAVFrame);
            av_free(mAVFrame);
            mAVFrame = NULL;
            continue;
        }
    }
//    fclose(outfile);
    return mDataSize;
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
//    LOGI("pcmBufferCallBack");
    Y10Audio *audio = (Y10Audio *) context;
    if (audio != NULL) {
        int bufferSize = audio->resampleAudio();
        if (bufferSize > 0) {
            //播放时间
            audio->mClockTime += bufferSize / ((double)(audio->mSampleRate * 2 * 2));//采样个数除以一秒的采样个数就是相应的时间
            if(audio->mClockTime - audio->mLastTime >= 0.1) {//每隔100ms
                audio->mLastTime = audio->mClockTime;
                //回调应用层
                audio->mCallJava->onCallTimeChanged(CHILD_THREAD, audio->mClockTime, audio->mDuration);
            }
            (*audio->pcmBufferQueue)->Enqueue(audio->pcmBufferQueue, (char *) audio->mResampleBuffer, bufferSize);
        }
    }
}

void Y10Audio::initOpenSLES() {
    LOGI("initOpenSL");
    SLresult result;
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    //第二步，创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, 0};

    // 第三步，配置PCM格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            getCurrentSampleRateForOpensles(mSampleRate),//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_MUTESOLO};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 3, ids, req);

    //初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);

    //得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerImpl);

    //初始化音量接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmVolumeImpl);
    setVolume(mVolumePercent);

    //初始化声道接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_MUTESOLO, &pcmMuteImpl);

    //注册回调缓冲区 获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);

    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);

    //获取播放状态接口
    (*pcmPlayerImpl)->SetPlayState(pcmPlayerImpl, SL_PLAYSTATE_PLAYING);
    pcmBufferCallBack(pcmBufferQueue, this);

}

void *decodePlay(void *data) {
    Y10Audio *audio = (Y10Audio *) data;
    audio->initOpenSLES();
    pthread_exit(&audio->mPlayThread);
}

void Y10Audio::start() {
    pthread_create(&mPlayThread, NULL, decodePlay, this);
}

void Y10Audio::resume() {
    if (pcmPlayerImpl != NULL) {
        (*pcmPlayerImpl)->SetPlayState(pcmPlayerImpl, SL_PLAYSTATE_PLAYING);
    }
}

void Y10Audio::pause() {
    if (pcmPlayerImpl != NULL) {
        (*pcmPlayerImpl)->SetPlayState(pcmPlayerImpl, SL_PLAYSTATE_PAUSED);
    }
}

void Y10Audio::release() {
    LOGI("release audio");
    if(pcmPlayerImpl != NULL) {
        (*pcmPlayerImpl)->SetPlayState(pcmPlayerImpl,  SL_PLAYSTATE_STOPPED);
    }

    if (mY10Queue != NULL) {
        delete (mY10Queue);
        mY10Queue = NULL;
    }

    if (pcmPlayerObject != NULL) {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject = NULL;
        pcmPlayerImpl = NULL;
        pcmBufferQueue = NULL;
    }

    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    if (mResampleBuffer != NULL) {
        free(mResampleBuffer);
        mResampleBuffer = NULL;
    }

    if (mAVCodecContext != NULL) {
        avcodec_close(mAVCodecContext);
        avcodec_free_context(&mAVCodecContext);
        mAVCodecContext = NULL;
    }

    if (mPlayStatus != NULL) {
        mPlayStatus = NULL;
    }

    if (mCallJava != NULL) {
        mCallJava = NULL;
    }

}

void Y10Audio::setVolume(int percent) {
    mVolumePercent = percent;
    if (pcmVolumeImpl != NULL) {
        if (percent > 30) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -20);
        } else if (percent > 25) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -22);
        } else if (percent > 20) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -25);
        } else if (percent > 15) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -28);
        } else if (percent > 10) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -30);
        } else if (percent > 5) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -34);
        } else if (percent > 3) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -37);
        } else if (percent > 0) {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -40);
        } else {
            (*pcmVolumeImpl)->SetVolumeLevel(pcmVolumeImpl, (100 - percent) * -100);
        }
    }
}

SLuint32 Y10Audio::getCurrentSampleRateForOpensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void Y10Audio::setMute(int mute) {
    LOGI("mute:%d",mute);
    if (pcmMuteImpl != NULL) {
        if (mute == 0) {//右
            (*pcmMuteImpl)->SetChannelMute(pcmMuteImpl, 1, SL_BOOLEAN_FALSE);
            (*pcmMuteImpl)->SetChannelMute(pcmMuteImpl, 0, SL_BOOLEAN_TRUE);
        } else if (mute == 1) {//左
            (*pcmMuteImpl)->SetChannelMute(pcmMuteImpl, 1, SL_BOOLEAN_TRUE);
            (*pcmMuteImpl)->SetChannelMute(pcmMuteImpl, 0, SL_BOOLEAN_FALSE);
        } else if (mute == 2) {//立体
            (*pcmMuteImpl)->SetChannelMute(pcmMuteImpl, 1, SL_BOOLEAN_FALSE);
            (*pcmMuteImpl)->SetChannelMute(pcmMuteImpl, 0, SL_BOOLEAN_FALSE);
        }
    }
}
