#ifndef ENC_CONFIG_H
#define ENC_CONFIG_H

#include <stdio.h>
#include <string>

struct EncoderConfig {
    int width = 0;
    int height = 0;
    int framerate = 0;
    int bitrate = 0;
    std::string pixelFormat;
    std::string codec;
    std::string codecOptions;
};

#endif // ENC_CONFIG_H
