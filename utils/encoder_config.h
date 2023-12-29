#ifndef ENC_CONFIG_H
#define ENC_CONFIG_H

#include <string>
#include "utils/codec_type.h"

struct EncoderConfig {
    uint64_t width = 0;
    uint64_t height = 0;
    uint64_t framerate = 0;
    std::string pixelFormat;
    std::string codec;
    std::string codecOptions;

    static EncoderConfig make(CodecType type, uint64_t width, uint64_t height, uint64_t frameRate, uint64_t bitrate) {
        auto buf_len = 1024; // attention possible overflow
        auto cmd_str = new char[buf_len];
        EncoderConfig config;

        switch (type) {
            case CodecType::Codec3gp:
                config = EncoderConfig{width, height, frameRate, "", "", ""};
                break;
            case CodecType::Codecmp4:
                config = EncoderConfig{width, height, frameRate, "", "", ""};
                break;
            case CodecType::CodecAvc:
                sprintf(cmd_str,
                        "bitrate=%lu tune=zerolatency sliced-threads=true speed-preset=ultrafast key-int-max=50", bitrate);
                config = EncoderConfig{width, height, frameRate, "I420", "x264enc", cmd_str};
                break;
            case CodecType::CodecHevc:
                config = EncoderConfig{width, height, frameRate, "I420", "x265enc",
                                       "tune=zerolatency sliced-threads=true speed-preset=ultrafast key-int-max=50"};
                break;
            case CodecType::CodecVp8:
                sprintf(cmd_str,
                        "end-usage=cbr cpu-used=10 threads=4 target-bitrate=%lu", bitrate * 1000);
                config = EncoderConfig{width, height, frameRate, "I420", "vp8enc",cmd_str};
                break;
            case CodecType::CodecVp9:
                sprintf(cmd_str,
                        "end-usage=cbr cpu-used=10 threads=4 target-bitrate=%lu",
                        bitrate * 1000);
                config = EncoderConfig{width, height, frameRate, "I420", "vp9enc",cmd_str};
                break;
        }
        delete[] cmd_str;
        return  config;
    }
};

#endif // ENC_CONFIG_H