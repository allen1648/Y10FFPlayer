//
// Created by stan on 18/10/1.
//

#include "CallJava.h"

CallJava::CallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj) {

    this->mJavaVM = javaVM;
    this->mJniEnv = env;
    this->mJobj = *obj;
    this->mJobj = env->NewGlobalRef(mJobj);

    jclass jclz = mJniEnv->GetObjectClass(mJobj);
    if (!jclz) {
        LOGE("get jclass wrong");
        return;
    }

    mJmethodPrepared = env->GetMethodID(jclz, "onCallPrepared", "()V");
    mJmethodLoad = env->GetMethodID(jclz, "onCallLoaded", "(Z)V");
    mJmethodTimeChanged = env->GetMethodID(jclz, "onCallTimeChanged", "(II)V");
    mJmethodError = env->GetMethodID(jclz, "onCallError", "(ILjava/lang/String;)V");

}

CallJava::~CallJava() {

}

void CallJava::onCallPrepared(int type) {
    LOGE("onCallPrepared");
    if (type == MAIN_THREAD) {
        mJniEnv->CallVoidMethod(mJobj, mJmethodPrepared);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            LOGE("get child thread jnienv failed");
            return;
        }
        jniEnv->CallVoidMethod(mJobj, mJmethodPrepared);
        mJavaVM->DetachCurrentThread();
    }

}

void CallJava::onCallLoad(int type, bool load) {
    if (type == MAIN_THREAD) {
        mJniEnv->CallVoidMethod(mJobj, mJmethodLoad, load);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallLoad worng");
            }
            return;
        }
        jniEnv->CallVoidMethod(mJobj, mJmethodLoad, load);
        mJavaVM->DetachCurrentThread();
    }

}

void CallJava::onCallTimeChanged(int type, int curr, int total) {
    if (type == MAIN_THREAD) {
        mJniEnv->CallVoidMethod(mJobj, mJmethodTimeChanged, curr, total);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallTimeChanged wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(mJobj, mJmethodTimeChanged, curr, total);
        mJavaVM->DetachCurrentThread();
    }
}

void CallJava::onCallError(int type, int code, char *msg) {
    if (type == MAIN_THREAD) {
        jstring jstr = mJniEnv->NewStringUTF(msg);
        mJniEnv->CallVoidMethod(mJobj, mJmethodError, code, jstr);
        mJniEnv->DeleteLocalRef(jstr);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallTimeChanged wrong");
            }
            return;
        }
        jstring jstr = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(mJobj, mJmethodError, code, jstr);
        jniEnv->DeleteLocalRef(jstr);
        mJavaVM->DetachCurrentThread();
    }
}
