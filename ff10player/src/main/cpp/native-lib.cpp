#include <jni.h>
#include <string>
#include "AndroidLog.h"
#include "Y10FFmpeg.h"
#include <unistd.h>

#define RESULT_SUCCESS 0
#define RESULT_ERROR -1
extern "C" {
#include "include/ffmpeg/libavformat/avformat.h"
}
using namespace std;
//解码开始
Y10FFmpeg *ffmpeg = NULL;
CallJava *callJava = NULL;
JavaVM *javaVM = NULL;
PlayStatus *playStatus = NULL;
pthread_t startThread;
pthread_t stopThread;
bool stopping = false;//防止重复退出,点击两次stop会生成两个线程,所以要判断下

void *startThreadCallback(void *data) {
    Y10FFmpeg *fFmpeg = (Y10FFmpeg *)data;
    fFmpeg->start();
    pthread_exit(&startThread);
}

void *stopThreadCallback(void *data) {
    if(stopping) {
        return NULL;
    }
    stopping = true;
    if (ffmpeg != NULL) {
        ffmpeg->release();
        delete (ffmpeg);
        ffmpeg = NULL;
        if (callJava != NULL) {
            delete (callJava);
            callJava = NULL;
        }
        if (playStatus != NULL) {
            delete (playStatus);
            playStatus = NULL;
        }
    }
    stopping = false;
    pthread_exit(&startThread);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    jint result = -1;
    javaVM = jvm;
    JNIEnv *env;
    if (jvm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("onLoad failed");
        return result;
    }
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nPrepare(JNIEnv *env, jobject instance, jstring jstr) {
    const char *url = env->GetStringUTFChars(jstr, 0);
    if (ffmpeg == NULL) {
        if (callJava == NULL) {
            callJava = new CallJava(javaVM, env, &instance);
        }
        callJava->onCallPrepared(MAIN_THREAD);
        playStatus = new PlayStatus();
        ffmpeg = new Y10FFmpeg(playStatus, callJava, url);
    } else {
        //已经在播放要先释放
//        ffmpeg->release();
    }
    ffmpeg->prepare();
    return RESULT_SUCCESS;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nStart(JNIEnv *env, jobject instance) {
    if (ffmpeg != NULL) {
        pthread_create(&startThread, NULL, startThreadCallback, ffmpeg);
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nResume(JNIEnv *env, jobject instance) {
    if(ffmpeg != NULL) {
        ffmpeg->resume();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nPause(JNIEnv *env, jobject instance) {
    if(ffmpeg != NULL) {
        ffmpeg->pause();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nStop(JNIEnv *env, jobject instance) {
    pthread_create(&stopThread, NULL, stopThreadCallback, NULL);
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nSeek(JNIEnv *env, jobject instance, jint secs) {
    if(ffmpeg != NULL) {
        ffmpeg->seek(secs);
    }
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nGetDuration(JNIEnv *env, jobject instance) {
    if(ffmpeg != NULL) {
        return ffmpeg->mDuration;
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nSetVolume(JNIEnv *env, jobject instance, jint value) {
    if(ffmpeg != NULL) {
        ffmpeg->setVolume(value);
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nSetMute(JNIEnv *env, jobject instance, jint mute) {
    if(ffmpeg != NULL) {
        ffmpeg->setMute(mute);
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nSetPitch(JNIEnv *env, jobject instance, jfloat pitch) {
    if(ffmpeg != NULL) {
        ffmpeg->setPitch(pitch);
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nSetSpeed(JNIEnv *env, jobject instance, jfloat speed) {
    if(ffmpeg != NULL) {
        ffmpeg->setSpeed(speed);
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nGetSampleRate(JNIEnv *env, jobject instance) {
    if(ffmpeg != NULL) {
        return ffmpeg->getSampleRate();
    }
    return 0;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_stan_ff10player_FF10Player_nIsPlaying(JNIEnv *env, jobject instance) {
    if(ffmpeg != NULL) {
        return ffmpeg->isPlaying();
    }
    return false;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nGetCurrentPosition(JNIEnv *env, jobject instance) {
    if(ffmpeg != NULL) {
        return ffmpeg->getCurrentPosition();
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stan_ff10player_FF10Player_nStopStartRecord(JNIEnv *env, jobject instance, jboolean stop) {
    if(ffmpeg != NULL) {
        ffmpeg->startStopRecord(stop);
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_stan_ff10player_FF10Player_nCutAudio(JNIEnv *env, jobject instance, jint start, jint end, jboolean showPcm) {
    if(ffmpeg != NULL) {
        return ffmpeg->cutAudio(start, end, showPcm);
    }
    return false;
}