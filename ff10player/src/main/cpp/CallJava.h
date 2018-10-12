//
// Created by stan on 18/10/1.
//

#ifndef INC_10FFMPEGPLAYER_CALLJAVA_H
#define INC_10FFMPEGPLAYER_CALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
#include <zconf.h>
#include "AndroidLog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1


class CallJava {

public:
    JavaVM *mJavaVM = NULL;
    JNIEnv *mJniEnv = NULL;
    jobject mJobj;

    jmethodID mJmethodPrepared;
    jmethodID mJmethodLoad;
    jmethodID mJmethodTimeChanged;
    jmethodID mJmethodError;
    jmethodID mJmethodComplete;
    jmethodID mJmethodStop;
    jmethodID mJmethodNext;
    jmethodID mJmethodPcm2aac;
    jmethodID mJmethodPcmInfo;
    jmethodID mJmethodPcmRate;

public:
    CallJava(JavaVM *javaVM, JNIEnv *env, jobject *obj);

    ~CallJava();

    void onCallPrepared(int type);
    void onCallLoad(int type, bool load);
    void onCallTimeChanged(int type, int curr, int total);
    void onCallError(int type, int code,const char *msg);
    void onCallComplete(int type);
    void onCallStop(int type);
    void onCallNext(int type);
    void onCallPcm2aac(int type, int size, void* buffer);
    void onCallPcmInfo(int type, void * buffer, int bufferSize);
    void onCallPcmRate(int type, int sampleRate);
};

#endif //INC_10FFMPEGPLAYER_CALLJAVA_H
