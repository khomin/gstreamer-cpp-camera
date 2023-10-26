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

    explicit SourceDevice(SourceDeviceType type, OptionType option = OptionType::None);
    SourceDevice() = delete;
    ~SourceDevice();

    void start() override;
    void pause() override;

private:
    static GstFlowReturn on_sample(GstElement * elt, SourceDevice* data);
    SourceDeviceType m_type = SourceDeviceType::Screen;

    static constexpr auto tag = "SourceDevice: ";
    static constexpr const char* cmd = "%s %s ! videoconvert ! videorate ! video/x-raw,format=RGB,framerate=20/1 ! appsink name=sink_out max-buffers=1 drop=true";
    static constexpr const char* cmd_screen_macos = "avfvideosrc name=src capture-screen=true capture-screen-cursor=true";
    static constexpr const char* cmd_camera_macos = "avfvideosrc name=src";
    static constexpr const char* cmd_screen_linux = "ximagesrc name=src";
    static constexpr const char* cmd_camera_linux = "ximagesrc name=src";
    static constexpr const char* cmd_screen_win = "dx9screencapsrc name=src";
    static constexpr const char* cmd_camera_win = "ksvideosrc name=src";
    static constexpr const char* show_time_overlay = "! timeoverlay draw-shadow=false draw-outline=false deltay=50 font-desc=\"Sans, 30\" color=0xFFFFFFFF";
};

#endif // SOURCE_DEV_H
