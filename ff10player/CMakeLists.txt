cmake_minimum_required(VERSION 3.4.1)
set(CPPDIR ${CMAKE_SOURCE_DIR}/src/main/cpp)
set(SOUNDTOUCH_CPPDIR ${CMAKE_SOURCE_DIR}/src/main/cpp/soundTouch)
set(FFMPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp/include/ffmpeg)
set(FFMPEG_SO_DIR ${CMAKE_SOURCE_DIR}/src/main/jni/${ANDROID_ABI})
set(SOUNDTOUCH_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp/include/soundTouch)
include_directories(${FFMPEG_INCLUDE_DIR})
include_directories(${SOUNDTOUCH_INCLUDE_DIR})

add_library( # Sets the name of the library.
        native-lib
        SHARED
        ${CPPDIR}/native-lib.cpp
        ${CPPDIR}/CallJava.cpp
        ${CPPDIR}/Y10Audio.cpp
        ${CPPDIR}/Y10FFmpeg.cpp
        ${CPPDIR}/PlayStatus.cpp
        ${CPPDIR}/Y10Queue.cpp
        ${CPPDIR}/Y10BufferQueue.cpp
        ${CPPDIR}/PCMBean.cpp
        # soundTouch
        ${SOUNDTOUCH_CPPDIR}/AAFilter.cpp
        ${SOUNDTOUCH_CPPDIR}/BPMDetect.cpp
        ${SOUNDTOUCH_CPPDIR}/cpu_detect_x86.cpp
        ${SOUNDTOUCH_CPPDIR}/FIFOSampleBuffer.cpp
        ${SOUNDTOUCH_CPPDIR}/FIRFilter.cpp
        ${SOUNDTOUCH_CPPDIR}/InterpolateCubic.cpp
        ${SOUNDTOUCH_CPPDIR}/InterpolateLinear.cpp
        ${SOUNDTOUCH_CPPDIR}/InterpolateShannon.cpp
        ${SOUNDTOUCH_CPPDIR}/mmx_optimized.cpp
        ${SOUNDTOUCH_CPPDIR}/PeakFinder.cpp
        ${SOUNDTOUCH_CPPDIR}/RateTransposer.cpp
        ${SOUNDTOUCH_CPPDIR}/SoundTouch.cpp
        ${SOUNDTOUCH_CPPDIR}/sse_optimized.cpp
        ${SOUNDTOUCH_CPPDIR}/TDStretch.cpp
        )

add_library(avcodec-58 SHARED IMPORTED)
set_target_properties(
        avcodec-58
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavcodec-58.so
)

add_library(avdevice-58 SHARED IMPORTED)
set_target_properties(
        avdevice-58
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavdevice-58.so
)

add_library(avfilter-7 SHARED IMPORTED)
set_target_properties(
        avfilter-7
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavfilter-7.so
)

add_library(avformat-58 SHARED IMPORTED)
set_target_properties(
        avformat-58
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavformat-58.so
)

add_library(avutil-56 SHARED IMPORTED)
set_target_properties(
        avutil-56
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavutil-56.so
)

add_library(postproc-55 SHARED IMPORTED)
set_target_properties(
        postproc-55
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libpostproc-55.so
)

add_library(swresample-3 SHARED IMPORTED)
set_target_properties(
        swresample-3
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libswresample-3.so
)

add_library(swscale-5 SHARED IMPORTED)
set_target_properties(
        swscale-5
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libswscale-5.so
)

target_link_libraries( # Specifies the target library.
        native-lib
        avcodec-58
        avdevice-58
        avfilter-7
        avformat-58
        avutil-56
        postproc-55
        swresample-3
        swscale-5
        OpenSLES
        log)