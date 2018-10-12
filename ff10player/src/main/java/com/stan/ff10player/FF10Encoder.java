package com.stan.ff10player;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * 目前只支持编码AAC
 */
public class FF10Encoder {
    //mediacodec
    private MediaFormat mEncoderFormat = null;
    private MediaCodec mEncoder = null;
    private MediaCodec.BufferInfo mBufferInfo = null;
    private FileOutputStream mOutputStream = null;
    private byte[] mOutByteBuffer = null;
    private int mPcmSize;
    private int mSampleRate;
    private double mRecordTime;
    private FF10Player mPlayer;

    public FF10Encoder(FF10Player player) {
        mPlayer = player;
        mSampleRate = player.getSampleRate();
    }

    public void pcm2aac(int size, byte[] buffer) {
        if (buffer != null && mEncoder != null) {
            int sampleRate = mSampleRate;
            mRecordTime += (double) size / (double)(sampleRate * 2 * 2);
            int inputBufferindex = mEncoder.dequeueInputBuffer(0);
            if (inputBufferindex >= 0) {
                ByteBuffer byteBuffer = mEncoder.getInputBuffers()[inputBufferindex];
                byteBuffer.clear();
                byteBuffer.put(buffer);
                mEncoder.queueInputBuffer(inputBufferindex, 0, size, 0, 0);
            }

            int index = mEncoder.dequeueOutputBuffer(mBufferInfo, 0);
            while (index >= 0) {
                try {
                    mPcmSize = mBufferInfo.size + 7;
                    mOutByteBuffer = new byte[mPcmSize];

                    ByteBuffer byteBuffer = mEncoder.getOutputBuffers()[index];
                    byteBuffer.position(mBufferInfo.offset);
                    byteBuffer.limit(mBufferInfo.offset + mBufferInfo.size);

                    addADTStoPacket(mOutByteBuffer, mPcmSize, mSampleRate);

                    byteBuffer.get(mOutByteBuffer, 7, mBufferInfo.size);
                    byteBuffer.position(mBufferInfo.offset);
                    mOutputStream.write(mOutByteBuffer, 0, mPcmSize);

                    mEncoder.releaseOutputBuffer(index, false);
                    index = mEncoder.dequeueOutputBuffer(mBufferInfo, 0);
                    mOutByteBuffer = null;
                    Log.d("yyl","编码...");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void releaseMediaCodec() {
        if(mEncoder == null) {
            return;
        }
        mRecordTime = 0;
        try {
            mOutputStream.close();
            mOutputStream = null;
            mEncoder.stop();
            mEncoder.release();
            mEncoder = null;
            mEncoderFormat = null;
            mBufferInfo = null;
            FF10Player.initmediacodec = true;
            Log.i("yyl","录制完成");
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if(mOutputStream != null) {
                try {
                    mOutputStream.close();
                    mOutputStream = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void initMediaCodec(int sampleRate, File outputFile) {
        mRecordTime = 0;
        mSampleRate = getADTSsamplerate(sampleRate);
        mEncoderFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, 2);
        mEncoderFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
        mEncoderFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        mEncoderFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096);
        try {
            mEncoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
            if (mEncoder == null) {
                return;
            }
            mBufferInfo = new MediaCodec.BufferInfo();
            mEncoder.configure(mEncoderFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mOutputStream = new FileOutputStream(outputFile);
            mEncoder.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void addADTStoPacket(byte[] packet, int packetLen, int sampleRate) {
        int profile = 2; // AAC LC
        int freqIdx = sampleRate; // samplerate
        int chanCfg = 2; // CPE
        packet[0] = (byte) 0xFF; // 0xFFF(12bit) 这里只取了8位，所以还差4位放到下一个里面
        packet[1] = (byte) 0xF9; // 第一个4bit位放F
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    private int getADTSsamplerate(int samplerate) {
        int rate = 4;
        switch (samplerate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }
}
