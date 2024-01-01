#ifndef SOURCE_DEV_H
#define SOURCE_DEV_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>

class SourceDevice : public SourceBase {
public:
    enum class SourceDeviceType { Webc, Screen };
    enum class OptionType { None, TimeOverlay };

    explicit SourceDevice(SourceDeviceType type, OptionType option = OptionType::None);
    SourceDevice() = delete;
    virtual ~SourceDevice();

    void start() override;
    void pause() override;
    void onConfig(std::function<void(uint32_t ,uint32_t)> cb);

private:
    static GstFlowReturn on_sample(GstElement * elt, SourceDevice* data);

    std::function<void(uint32_t ,uint32_t)> m_config_changed = NULL;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    static constexpr auto TAG = "SourceDevice: ";
    static constexpr const char* CMD = "%s %s ! videoconvert ! videorate ! videoscale ! video/x-raw,format=RGB,framerate=20/1,width=1920,height=1200 ! appsink name=sink_out";
    static constexpr const char* CMD_SCREEN_MACOS = "avfvideosrc name=src capture-screen=true capture-screen-cursor=true";
    static constexpr const char* CMD_CAMERA_MACOS = "avfvideosrc name=src";
    static constexpr const char* CMD_SCREEN_LINUX = "ximagesrc name=src";
    static constexpr const char* CMD_CAMERA_LINUX = "v4l2src name=src";
    static constexpr const char* CMD_SCREEN_WIN = "dx9screencapsrc name=src";
    static constexpr const char* CMD_CAMERA_WIN = "ksvideosrc name=src";
    static constexpr const char* CMD_TIME_OVERLAY = "! timeoverlay draw-shadow=false draw-outline=false deltay=50 font-desc=\"Sans, 30\" color=0xFFFFFFFF";
};

#endif // SOURCE_DEV_H
