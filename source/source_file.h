#ifndef SOURCE_WAV_H
#define SOURCE_WAV_H

#include "source/source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>
#include <atomic>

class SourceFile : public SourceBase {
public:
    enum Type { video, audio };

    explicit SourceFile(std::string path, Type type, bool loop = false);
    SourceFile() = delete;
    virtual ~SourceFile();

    void start() override;
    void pause() override;
private:
    static gboolean on_bus_cb(GstBus * bus, GstMessage * message, gpointer data);

    bool m_loop = false;
    std::atomic<bool> m_running;
    Type m_type;

    static GstFlowReturn on_sample(GstElement * elt, SourceFile* data);
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

//    static constexpr const char* CMD = "filesrc location=%s \
//            ! decodebin name=demux demux. ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! appsink name=sink_out demux. \
//            ! queue ! audioconvert ! autoaudiosink";

//    static constexpr const char* CMD = "filesrc location=%s \
//        ! decodebin name=demux demux. ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! appsink name=sink_out";

//    static constexpr const char* CMD = "filesrc location=%s \
//        ! decodebin name=demux demux. ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! videoconvert !  xvimagesink";

    static constexpr const char* CMD = "filesrc location=%s \
        ! decodebin name=demux demux. ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! fakesink";

    static constexpr auto TAG = "SourceFile: ";
};

#endif // SOURCE_WAV_H
