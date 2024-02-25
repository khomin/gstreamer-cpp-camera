#ifndef I_VIDEO_DEVICE_PLATFORM_H
#define I_VIDEO_DEVICE_PLATFORM_H

#include <stdio.h>
#include <vector>

class IVideoDevicePlatform {
public:
    virtual std::pair<int, int> getCameraSize(std::string type) = 0;
};

#endif
