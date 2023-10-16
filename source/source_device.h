#ifndef SOURCE_DEV_H
#define SOURCE_DEV_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>

class SourceDevice : public SourceBase {
public:
    enum class SourceDeviceType { Webc, Screen };

    SourceDevice(SourceDeviceType type);
    SourceDevice();

    void start() override;
    void pause() override;
    void stop() override;

    SourceDeviceType m_type = SourceDeviceType::Screen;

    static constexpr const char* cmd_webc = "avfvideosrc ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink_raw_image";
    static constexpr const char* cmd_screen = "avfvideosrc capture-screen=true capture-screen-cursor=true capture-screen-mouse-clicks=true ! timeoverlay draw-shadow=false draw-outline=false deltay=20 font-desc=\"Sans, 5\" color=0xFFFFFFFF ! videoconvert ! video/x-raw,format=RGB ! videorate ! video/x-raw,framerate=24/1 ! appsink name=sink_raw_image";
};

#endif // SOURCE_DEV_H
