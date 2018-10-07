cmake_minimum_required(VERSION 3.4.1)
set(CPPDIR ${CMAKE_SOURCE_DIR}/src/main/cpp)
set(FFMPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp/include/ffmpeg)
set(FFMPEG_SO_DIR ${CMAKE_SOURCE_DIR}/src/main/jni/${ANDROID_ABI})
include_directories(${FFMPEG_INCLUDE_DIR})

add_library( # Sets the name of the library.
        native-lib
        SHARED
        ${CPPDIR}/native-lib.cpp
        ${CPPDIR}/CallJava.cpp
        ${CPPDIR}/Y10Audio.cpp
        ${CPPDIR}/Y10FFmpeg.cpp
        ${CPPDIR}/PlayStatus.cpp
        ${CPPDIR}/Y10Queue.cpp
        )

add_library(avcodec-57 SHARED IMPORTED)
set_target_properties(
        avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavcodec-57.so
)

add_library(avdevice-57 SHARED IMPORTED)
set_target_properties(
        avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavdevice-57.so
)

add_library(avfilter-6 SHARED IMPORTED)
set_target_properties(
        avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavfilter-6.so
)

add_library(avformat-57 SHARED IMPORTED)
set_target_properties(
        avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavformat-57.so
)

add_library(avutil-55 SHARED IMPORTED)
set_target_properties(
        avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libavutil-55.so
)

add_library(postproc-54 SHARED IMPORTED)
set_target_properties(
        postproc-54
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libpostproc-54.so
)

add_library(swresample-2 SHARED IMPORTED)
set_target_properties(
        swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libswresample-2.so
)

add_library(swscale-4 SHARED IMPORTED)
set_target_properties(
        swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_SO_DIR}/libswscale-4.so
)

target_link_libraries( # Specifies the target library.
        native-lib
        avcodec-57
        avdevice-57
        avfilter-6
        avformat-57
        avutil-55
        postproc-54
        swresample-2
        swscale-4
        OpenSLES
        log)