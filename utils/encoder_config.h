#ifndef ENC_CONFIG_H
#define ENC_CONFIG_H

#include <string>
#include "utils/codec_type.h"
#include "utils/stringf.h"

struct EncoderConfig {
    uint64_t  width = 0;
    uint64_t  height = 0;
    uint64_t  framerate = 0;
    std::string pixelFormat;
    std::string codec;
    std::string codecOptions;

    static EncoderConfig make(CodecType type, uint64_t  width, uint64_t  height, uint64_t  frameRate, uint64_t  bitrate) {
        switch(type) {
        case CodecType::Codec3gp:
            return EncoderConfig{width, height, frameRate,  "","",""};
        case CodecType::Codecmp4:
            return EncoderConfig{width, height, frameRate,  "","",""};
        case CodecType::CodecAvc:
            return EncoderConfig{width, height, frameRate, "I420", "x264enc",
                                 StringFormatter::format("bitrate=%d", bitrate) + " " +
                                 "tune=zerolatency sliced-threads=true speed-preset=ultrafast key-int-max=50"};
        case CodecType::CodecHevc:
            return EncoderConfig{width, height, frameRate, "I420", "x265enc",
                                 "tune=zerolatency sliced-threads=true speed-preset=ultrafast key-int-max=50"};
            case CodecType::CodecVp8:
                return EncoderConfig{width, height, frameRate, "I420", "vp8enc",
                                     StringFormatter::format(
                                             "end-usage=cbr cpu-used=10 threads=4 target-bitrate=%d", bitrate * 1000
                                     )};
            case CodecType::CodecVp9:
                return EncoderConfig{width, height, frameRate, "I420", "vp9enc",
                                     StringFormatter::format(
                                             "end-usage=cbr cpu-used=10 threads=4 target-bitrate=%d", bitrate * 1000)};
        }
        // cq-level=5 end-usage=cbr dropframe-threshold=60 static-threshold=0 undershoot=95 keyframe-max-dist=50 buffer-size=6000 buffer-initial-size=4000 buffer-optimal-size=5000
    }
};

#endif // ENC_CONFIG_H