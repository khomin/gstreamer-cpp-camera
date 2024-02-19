#ifndef I_VIDEO_DEVICE_PLATFORM_H
#define I_VIDEO_DEVICE_PLATFORM_H

#include <stdio.h>
#include <vector>

class IVideoDevicePlatform {
public:
    virtual bool onStartSource(std::string type) = 0;
    virtual bool onStopSource() = 0;
    virtual std::vector<std::pair<int, int>> getCameraSize() = 0;
};

#endif
