//
// Created by stan on 18/10/1.
//

#ifndef INC_10FFMPEGPLAYER_ANDROIDLOG_H
#define INC_10FFMPEGPLAYER_ANDROIDLOG_H

#include <android/log.h>

#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"yyl",FORMAT,##__VA_ARGS__);
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"yyl",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"yyl",FORMAT,##__VA_ARGS__);
#define LOG_DEBUG true
#endif //INC_10FFMPEGPLAYER_ANDROIDLOG_H
