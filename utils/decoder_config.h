#ifndef DEC_CONFIG_H
#define DEC_CONFIG_H

#include <stdio.h>
#include <string>

struct DecoderConfig {
    int width = 0;
    int height = 0;
    int framerate = 0;
    int bitrate = 0;
    std::string codecInVideo;
    std::string pixelFormat;
    std::string decoder;
};

#endif // DEC_CONFIG_H
