#include "PCMBean.h"

PCMBean::PCMBean(SAMPLETYPE *buffer, int bufferSize) {
    this->mBuffer = (char *) malloc(bufferSize);
    this->mBufferSize = bufferSize;
    memcpy(this->mBuffer, buffer, bufferSize);
}

PCMBean::~PCMBean() {
    free(mBuffer);
    mBuffer = NULL;
}
