#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string>

struct Config {
    int width = 0;
    int height = 0;
    int framerate = 0;
    std::string type;
    std::string format;
};

#endif // CONFIG_H
