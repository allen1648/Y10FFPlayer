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

extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nPrepare(JNIEnv *env, jobject instance, jstring jstr) {
    const char *url = env->GetStringUTFChars(jstr, 0);
    if (ffmpeg == NULL) {
        if (callJava == NULL) {
            callJava = new CallJava(javaVM, env, &instance);
        }
        playStatus = new PlayStatus();
        ffmpeg = new Y10FFmpeg(playStatus, callJava, url);
    }
    ffmpeg->prepare();
    return RESULT_SUCCESS;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_stan_ff10player_FF10Player_nStart(JNIEnv *env, jobject instance) {
    if (ffmpeg != NULL) {
        ffmpeg->start();
    }
    return 0;
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
    return 0;
}