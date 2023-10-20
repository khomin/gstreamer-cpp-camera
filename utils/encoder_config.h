#ifndef ENC_CONFIG_H
#define ENC_CONFIG_H

#include <stdio.h>
#include <string>
#include "utils/codec_type.h"

struct EncoderConfig {
    uint64_t  width = 0;
    uint64_t  height = 0;
    uint64_t  framerate = 0;
    uint64_t  bitrate = 0;
    std::string pixelFormat;
    std::string codec;
    std::string codecOptions;

    static EncoderConfig make(CodecType type, uint64_t  width, uint64_t  height, uint64_t  frameRate, uint64_t  bitrate) {
        switch(type) {
        case CodecType::Codec3gp:
            return EncoderConfig{width, height, frameRate, bitrate, "","",""};
        case CodecType::Codecmp4:
            return EncoderConfig{width, height, frameRate, bitrate, "","",""};
        case CodecType::CodecAvc:{}
            return EncoderConfig{width, height, frameRate, bitrate, "I420","x264enc","tune=zerolatency key-int-max=26"};
        case CodecType::CodecHevc:{}
            return EncoderConfig{width, height, frameRate, bitrate, "","",""};
        case CodecType::CodecVp8:{}
            return EncoderConfig{width, height, frameRate, bitrate, "I420","vp8enc",""};
        case CodecType::CodecVp9:
            return EncoderConfig{width, height, frameRate, bitrate, "I420","vp9enc",""};
        }
    }
};

#endif // ENC_CONFIG_H

//auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig{426,240,30,"I420","vp8enc",""});
//bitrate=30000 bframes=6 threads=15 subme=10 key-int-max=20
//auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig{2560 /2,1600 /2,10,200000,"I420","x264enc","tune=zerolatency speed-preset=ultrafast"});
