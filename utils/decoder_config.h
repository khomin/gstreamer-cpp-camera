#ifndef DEC_CONFIG_H
#define DEC_CONFIG_H

#include <stdio.h>
#include <string>
#include "utils/codec_type.h"

struct DecoderConfig {
    int  width = 0;
    int  height = 0;
    int  framerate = 0;
    int bitrate = 0;
    std::string codecInVideo;
    std::string pixelFormat;
    std::string decoder;

    static DecoderConfig make(CodecType type, int  width, int  height, int  frameRate, int  bitrate) {
        switch(type) {
        case CodecType::Codec3gp:
            return DecoderConfig{width, height, frameRate, bitrate, "","",""};
        case CodecType::Codecmp4:
            return DecoderConfig{width, height, frameRate, bitrate, "x-h264","I420","avdec_h264"};
        case CodecType::CodecAvc:
            return DecoderConfig{width, height, frameRate, bitrate, "x-h264","I420","avdec_h264"};
        case CodecType::CodecHevc:
            return DecoderConfig{width, height, frameRate, bitrate, "x-h265","I420","h265parse ! avdec_h265"};
        case CodecType::CodecVp8:
            return DecoderConfig{width, height, frameRate, bitrate, "x-vp8","I420","vp8dec"};
        case CodecType::CodecVp9:
            return DecoderConfig{width, height, frameRate, bitrate, "x-vp9","I420","vp9dec"};
        }
        return DecoderConfig{0, 0, 0, 0, ""};
    }
};

#endif // DEC_CONFIG_H