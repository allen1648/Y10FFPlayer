#ifndef INC_10FFMPEGPLAYER_Y10AUDIO_H
#define INC_10FFMPEGPLAYER_Y10AUDIO_H

#include "Y10Queue.h"
#include "CallJava.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};
#define OPENSL_FUNCTIONS 2
class Y10Audio {
public:
    int mStreamIndex = -1;
    int ret = 0;
    int mDataSize = 0;
    int mSampleRate = 0;
    int mDuration = 0;//单位秒
    int mVolumePercent = 100;
    uint8_t *mResampleBuffer = NULL;//存储重采样的流
    AVCodecContext *mAVCodecContext = NULL;
    AVCodecParameters *mCodecpar = NULL;
    Y10Queue *mY10Queue = NULL;
    PlayStatus *mPlayStatus = NULL;
    AVPacket *mAVPacket = NULL;
    AVFrame *mAVFrame = NULL;
    pthread_t mPlayThread;
    CallJava *mCallJava = NULL;
    AVRational mTimeBase;//分子/分母 就是一个Frame的时间
    double mClockTime;//总的播放时长
    double mNowTime = 0;//当前frame播放时间
    double mLastTime = 0; //上一次调用时间
    //OpenSLES
    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerImpl = NULL;
    SLVolumeItf pcmVolumeImpl = NULL;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

public:
    Y10Audio(PlayStatus *playStatus, int sampleRate, CallJava *callJava);
    ~Y10Audio();
    void initOpenSLES();
    void start();
    void resume();
    void pause();
    void release();
    void setVolume(int value);
    int resampleAudio();
    SLuint32 getCurrentSampleRateForOpensles(int sample_rate);
};


#endif //INC_10FFMPEGPLAYER_Y10AUDIO_H
