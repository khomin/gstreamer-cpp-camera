#ifndef I_VIDEO_DEVICE_PLATFORM_H
#define I_VIDEO_DEVICE_PLATFORM_H

#include <stdio.h>
#include <vector>

class IVideoDevicePlatform {
public:
    virtual bool onStartSource(std::string type, int width, int height) = 0;
    virtual bool onStopSource() = 0;
    virtual std::pair<int, int> getCameraSize(std::string type) = 0;
};

#endif
