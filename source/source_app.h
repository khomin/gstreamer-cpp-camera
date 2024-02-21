#ifndef SOURCE_APP_H
#define SOURCE_APP_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>

class SourceApp : public SourceBase {
public:
    SourceApp(std::string format, int width, int height, int framerate);
    virtual ~SourceApp();

    void start() override;
    void pause() override;
    void putData(uint8_t *data, uint32_t len);

private:
    static GstFlowReturn on_sample(GstElement * elt, SourceApp* data);
    int m_width = 0;
    int m_height = 0;

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

#endif // SOURCE_APP_H
