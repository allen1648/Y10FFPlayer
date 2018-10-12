#ifndef INC_10FFMPEGPLAYER_Y10AUDIO_H
#define INC_10FFMPEGPLAYER_Y10AUDIO_H

#include "Y10Queue.h"
#include "CallJava.h"
#include "SoundTouch.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};
using namespace soundtouch;
class Y10Audio {
public:
    int mStreamIndex = -1;
    int ret = 0;

    /* 解码一个AVFrame后所需DataSize(会有多个音频帧) */
    int mDataSize = 0;

    int mSampleRate = 0;
    int mDuration = 0;//单位秒
    int mVolumePercent = 100;
    int mResampleNumber = 0;//重采样个数, swr_convert()返回
    int mSoundTouchSampleNum = 0;
    bool mReadSoundTouchBufferUnFinished = true;
    bool mRecordingPcm = false;
    uint8_t *mResampleBuffer = NULL;//存储重采样的流
    uint8_t *mOutBuffer = NULL;//
    AVCodecContext *mAVCodecContext = NULL;
    AVCodecParameters *mCodecpar = NULL;
    Y10Queue *mY10Queue = NULL;
    PlayStatus *mPlayStatus = NULL;
    AVPacket *mAVPacket = NULL;
    AVFrame *mAVFrame = NULL;
    pthread_t mPlayThread;
    CallJava *mCallJava = NULL;
    AVRational mTimeBase;//分子/分母 就是一个Frame的时间
    double mClockTime;//总的播放时长,暂时认定为duration
    double mNowTime = 0;//当前frame播放时间
    double mLastTime = 0; //上一次调用时间
    float mPitch = 1.0f;//声调
    float mSpeed = 1.0f;//播放速度

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
    SLMuteSoloItf pcmMuteImpl = NULL;//声道

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

    //soundTouch
    SoundTouch *mSoundTouch = NULL;
    SAMPLETYPE *mSoundTouchBuffer = NULL;

public:
    Y10Audio(PlayStatus *playStatus, int sampleRate, CallJava *callJava);
    ~Y10Audio();
    void initOpenSLES();
    void start();
    void resume();
    void pause();
    void release();
    void setVolume(int value);
    void setMute(int mute);
    void setPitch(float pitch);
    void setSpeed(float speed);
    int resampleAudio(void **out);
    int resampleWithSoundTouch();
    SLuint32 getCurrentSampleRateForOpensles(int sample_rate);
    bool isPlaying();

    int getCurrentPosition();

    /* 开启/关闭录音 */
    void startStopRecord(bool b);
};


#endif //INC_10FFMPEGPLAYER_Y10AUDIO_H
