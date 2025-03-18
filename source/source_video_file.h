#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H

#include "source/source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>
#include <atomic>

class SourceVideoFile : public SourceBase {
public:
    explicit SourceVideoFile(std::string path,
                             int width, int height,
                             int framerate,
                             float volume,
                             bool loop = false);
    SourceVideoFile() = delete;
    virtual ~SourceVideoFile();

    void start(uint64_t position = 0) override;
    void pause() override;

    void setVolume(float value);

    bool seekTo(uint64_t sec);
    uint64_t getPlaybackPosition();
private:
    static gboolean on_bus_cb(GstBus * bus, GstMessage * message, gpointer data);

    bool m_loop = false;
    std::atomic<bool> m_running;

    static GstFlowReturn on_sample(GstElement * elt, SourceVideoFile* data);
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

//    static constexpr const char* CMD = "filesrc location=%s ! decodebin name=demux demux. ! videoconvert ! fakesink name=sink_out";

    static constexpr const char* CMD_WITH_AUDIO = "filesrc location=%s \
            ! decodebin name=demux \
            demux. ! videoconvert ! videoscale ! videorate  \
            ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out sync=false drop=true \
            demux. ! audioconvert ! autoaudiosink sync=false";

    static constexpr const char* CMD_NO_AUDIO = "filesrc location=%s \
            ! decodebin name=demux \
            demux. ! videoconvert ! videoscale ! videorate  \
            ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out sync=false drop=true";

//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux name=demux \
//        demux.audio_0 ! avdec_aac ! audioconvert ! autoaudiosink sync=false \
//        demux.video_0 ! avdec_h264 ! videoconvert ! videoscale ! videorate ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out sync=false";

//    // works video
//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux name=demux \
//        demux.video_0 ! avdec_h264 ! videoconvert ! videoscale ! videorate ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out";


//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux ! h264parse ! avdec_h264 ! videoconvert ! videoscale ! videorate \
//        ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 \
//        ! appsink name=sink_out drop=true sync=false";


//    static constexpr const char* CMD = "filesrc location=%s \
//        ! decodebin name=demux demux. ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! appsink name=sink_out";

//    static constexpr const char* CMD = "filesrc location=%s \
//        ! decodebin name=demux demux. ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! videoconvert !  xvimagesink";

//    filesrc location=/home/khomin/Desktop/test-images/demo.mp4 ! qtdemux ! h264parse ! avdec_h264 ! fakesink

    // 800mb
//    static constexpr const char* CMD = "filesrc location=%s \
//        ! decodebin name=demux demux. \
//        ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out drop=true sync=false \
//        demux. ! queue ! audioconvert ! volume name=volume_control ! autoaudiosink";

//    leaky=downstream max-size-buffers=1

// try2
//    static constexpr const char* CMD = "filesrc location=%s \
//        ! decodebin name=demux demux. \
//        ! queue leaky=downstream max-size-buffers=1 ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out drop=true sync=false \
//        demux. ! queue leaky=downstream max-size-buffers=1 ! audioconvert ! volume name=volume_control ! autoaudiosink";

//    // 306mb
//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux ! h264parse ! avdec_h264 \
//        ! queue leaky=downstream max-size-buffers=1 ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out drop=true sync=false";

    // good 316mb
//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux ! h264parse ! avdec_h264 ! videoconvert ! videoscale ! videorate \
//        ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 \
//        ! appsink name=sink_out drop=true sync=false";

//    static constexpr const char* CMD = "filesrc location=%s \
//           ! qtdemux name=demux demux. \
//           ! h264parse ! avdec_h264 ! videoconvert ! videoscale ! videorate \
//           ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 \
//           ! appsink name=sink_out drop=true sync=false \
//           demux. ! queue ! audioconvert ! volume name=volume_control ! autoaudiosink";

//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux name=demux \
//        demux. ! queue ! audioconvert ! volume name=volume_control ! autoaudiosink \
//        demux. ! queue ! h264parse ! avdec_h264 ! videoconvert ! videoscale ! videorate ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out drop=true sync=false";

    // audiod doesn't work
//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux name=demux demux. \
//        ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out drop=true sync=false \
//        demux. ! queue ! audioconvert ! volume name=volume_control ! autoaudiosink";

    // 830mb
//    static constexpr const char* CMD = "filesrc location=%s \
//        ! decodebin max-size-time=2000000000 name=demux demux. \
//        ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out drop=true sync=false \
//        demux. ! queue ! audioconvert ! volume name=volume_control ! autoaudiosink";

//    static constexpr const char* CMD = "filesrc location=%s \
//        ! qtdemux ! h264parse ! avdec_h264 \
//        ! queue leaky=downstream max-size-buffers=1 ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! fakesink name=sink_out drop=true sync=false";

    static constexpr auto TAG = "SourceFile: ";
};

#endif // SOURCE_FILE_H
