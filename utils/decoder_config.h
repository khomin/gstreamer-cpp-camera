#ifndef DEC_CONFIG_H
#define DEC_CONFIG_H

#include <stdio.h>
#include <string>
#include "utils/codec_type.h"

struct DecoderConfig {
    uint64_t  width = 0;
    uint64_t  height = 0;
    uint64_t  framerate = 0;
    uint64_t bitrate = 0;
    std::string codecInVideo;
    std::string pixelFormat;
    std::string decoder;

    static DecoderConfig make(CodecType type, uint64_t  width, uint64_t  height, uint64_t  frameRate, uint64_t  bitrate) {
        switch(type) {
        case CodecType::Codec3gp:
            return DecoderConfig{width, height, frameRate, bitrate, "","",""};
        case CodecType::Codecmp4:
            return DecoderConfig{width, height, frameRate, bitrate, "x-h264","I420","avdec_h264"};
        case CodecType::CodecAvc:{}
            return DecoderConfig{width, height, frameRate, bitrate, "x-h264","I420","h264parse ! avdec_h264"};
        case CodecType::CodecHevc:{}
            return DecoderConfig{width, height, frameRate, bitrate, "","",""};
        case CodecType::CodecVp8:{}
            return DecoderConfig{width, height, frameRate, bitrate, "x-vp8","I420","vp8dec"};
        case CodecType::CodecVp9:
            return DecoderConfig{width, height, frameRate, bitrate, "x-vp9","I420","vp9dec"};
        }
    }
};

#endif // DEC_CONFIG_H

//byte-stream=true tune=zerolatency
//auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig{426,240,30,"x-vp8","I420","vp8dec"});
