#ifndef SOURCE_DEV_H
#define SOURCE_DEV_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>

class SourceDevice : public SourceBase {
public:
    enum class SourceDeviceType { Webc, Screen };
    enum class OptionType { None, TimeOverlay };

    SourceDevice(SourceDeviceType type, OptionType option = OptionType::None);
    SourceDevice();
    ~SourceDevice();

    void start() override;
    void pause() override;
    void stop() override;

private:
    SourceDeviceType m_type = SourceDeviceType::Screen;

    static constexpr const char* cmd_screen = "%s %s ! videoconvert ! video/x-raw,format=RGB ! videorate ! video/x-raw,framerate=30/1 ! appsink name=sink_out";
    static constexpr const char* cmd_webc = "%s %s ! videoconvert ! video/x-raw,format=RGB ! videorate ! video/x-raw,framerate=30/1 ! appsink name=sink_out";

    static constexpr const char* cmd_screen_macos = "avfvideosrc capture-screen=true capture-screen-cursor=true capture-screen-mouse-clicks=true";
    static constexpr const char* cmd_screen_linux = "ximagesrc";
    static constexpr const char* cmd_screen_win = "TODO";

    static constexpr const char* cmd_webc_macos = "avfvideosrc";
    static constexpr const char* cmd_webc_linux = "v4l2src";
    static constexpr const char* cmd_webc_win = "TODO";

    static constexpr const char* show_timeoverlay = "! timeoverlay draw-shadow=false draw-outline=false deltay=20 font-desc=\"Sans, 30\" color=0xFFFFFFFF";
};

#endif // SOURCE_DEV_H
