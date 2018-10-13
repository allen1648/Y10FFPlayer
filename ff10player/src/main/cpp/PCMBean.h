#ifndef INC_10FFMPEGPLAYER_PCMBEAN_H
#define INC_10FFMPEGPLAYER_PCMBEAN_H

#include <SoundTouch.h>

using namespace soundtouch;

class PCMBean {
public:
    char *mBuffer;
    int mBufferSize;
public:
    PCMBean(SAMPLETYPE *buffer, int bufferSize);

    ~PCMBean();

};

#endif //INC_10FFMPEGPLAYER_PCMBEAN_H