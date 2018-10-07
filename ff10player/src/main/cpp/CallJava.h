//
// Created by stan on 18/10/1.
//

#ifndef INC_10FFMPEGPLAYER_CALLJAVA_H
#define INC_10FFMPEGPLAYER_CALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
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

public:
    CallJava(JavaVM *javaVM, JNIEnv *env, jobject *obj);

    ~CallJava();

    void onCallPrepared(int type);
    void onCallLoad(int type, bool load);
    void onCallTimeChanged(int type, int curr, int total);
    void onCallError(int type, int code,const char *msg);
    void onCallComplete(int type);
};

#endif //INC_10FFMPEGPLAYER_CALLJAVA_H
