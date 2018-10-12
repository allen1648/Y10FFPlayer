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
    mJmethodComplete = env->GetMethodID(jclz, "onCallComplete", "()V");
    mJmethodPcm2aac = env->GetMethodID(jclz, "pcm2aac", "(I[B})V");

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

void CallJava::onCallError(int type, int code,const char *msg) {
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

void CallJava::onCallComplete(int type) {
    if (type == MAIN_THREAD) {
        mJniEnv->CallVoidMethod(mJobj, mJmethodComplete);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallComplete wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(mJobj, mJmethodComplete);
        mJavaVM->DetachCurrentThread();
    }
}

void CallJava::onCallStop(int type) {
    if (type == MAIN_THREAD) {
        mJniEnv->CallVoidMethod(mJobj, mJmethodStop);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallStop wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(mJobj, mJmethodStop);
        mJavaVM->DetachCurrentThread();
    }
}

void CallJava::onCallNext(int type) {
    if (type == MAIN_THREAD) {
        mJniEnv->CallVoidMethod(mJobj, mJmethodStop);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallStop wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(mJobj, mJmethodStop);
        mJavaVM->DetachCurrentThread();
    }
}

void CallJava::onCallPcm2aac(int type, int size, void* buffer) {
    if (type == MAIN_THREAD) {
        jbyteArray jbuffer = mJniEnv->NewByteArray(size);
        mJniEnv->SetByteArrayRegion(jbuffer, 0, size, (const jbyte *) (buffer));
        mJniEnv->CallVoidMethod(mJobj, mJmethodPcm2aac, size, jbuffer);
        mJniEnv->DeleteLocalRef(jbuffer);
    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (mJavaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("call onCallPcm2aac wrong");
            }
            return;
        }
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, (const jbyte *)(buffer));
        jniEnv->CallVoidMethod(mJobj, mJmethodPcm2aac, size, jbuffer);
        jniEnv->DeleteLocalRef(jbuffer);
        mJavaVM->DetachCurrentThread();
    }
}
