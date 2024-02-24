#ifndef ENC_CONFIG_H
#define ENC_CONFIG_H

#include <string>
#include <vector>
#include "utils/codec_type.h"
#include "config.h"

struct EncoderConfig {
    int width = 0;
    int height = 0;
    int framerate = 0;
    int keyInt = 0;
    std::string pixelFormat;
    std::string codec;
    std::string codecOptions;

    static EncoderConfig make(CodecType type, int width, int height, int frameRate, int bitrate, int keyInt) {
        auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
        EncoderConfig config;

        switch (type) {
            case CodecType::Codec3gp:
                config = EncoderConfig{width, height, frameRate, keyInt, "", "", ""};
                break;
            case CodecType::Codecmp4:
                config = EncoderConfig{width, height, frameRate, keyInt, "", "", ""};
                break;
            case CodecType::CodecAvc:
                sprintf((char *) cmdBuf.data(),
                        "bitrate=%d tune=zerolatency key-int-max=%d speed-preset=ultrafast threads=20 byte-stream=true",
                        bitrate, keyInt
                );
                config = EncoderConfig{width, height, frameRate, keyInt, "I420", "x264enc", (char*)cmdBuf.data()};
                break;
            case CodecType::CodecHevc:
                sprintf((char *) cmdBuf.data(),
                        "bitrate=%d tune=zerolatency key-int-max=%d",
                        bitrate, keyInt
                );
                config = EncoderConfig{width, height, frameRate, keyInt, "I420", "x265enc", (char*)cmdBuf.data()};
                break;
            case CodecType::CodecVp8:
                sprintf((char*)cmdBuf.data(),
                        "end-usage=cbr cpu-used=10 threads=4 target-bitrate=%d", bitrate);
                config = EncoderConfig{width, height, frameRate, keyInt, "I420", "vp8enc",(char*)cmdBuf.data()};
                break;
            case CodecType::CodecVp9:
                sprintf((char*)cmdBuf.data(),
                        "end-usage=cbr cpu-used=10 threads=4 target-bitrate=%d",
                        bitrate);
                config = EncoderConfig{width, height, frameRate, keyInt, "I420", "vp9enc",(char*)cmdBuf.data()};
                break;
            case CodecType::Unknown:
                break;
        }
        return  config;
    }
};

#endif // ENC_CONFIG_H