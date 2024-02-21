#include "source_app.h"
#include "config.h"
#include <iostream>
#include <thread>

//    static constexpr const char* CMD = "%s %s ! videoconvert ! videorate ! videoscale ! video/x-raw,format=RGB,framerate=20/1,width=1920,height=1200 ! appsink name=sink_out";
static constexpr const char *CMD = "%s %s ! videoconvert ! videorate ! videoscale ! video/x-raw,width=(int)2304, height=(int)1728,framerate=20/1,format=RGB ! appsink name=sink_out";
static constexpr const char *CMD_SCREEN_MACOS = "avfvideosrc name=src capture-screen=true capture-screen-cursor=true";
static constexpr const char *CMD_CAMERA_MACOS = "avfvideosrc name=src";
//static constexpr const char* CMD_SCREEN_LINUX = "ximagesrc name=src";
//static constexpr const char* CMD_CAMERA_LINUX = "v4l2src name=src";
static constexpr const char *CMD_SCREEN_WIN = "dx9screencapsrc name=src";
static constexpr const char *CMD_CAMERA_WIN = "ksvideosrc name=src";
//static constexpr const char* CMD_CAMERA_ANDROID = "appsrc name=source_to_out ! video/x-raw, width=(int)2304, height=(int)1728, framerate=(fraction)20/1, format=(string)RGBA";

SourceApp::SourceApp(std::string format, int width, int height, int framerate) {
    GstBus *bus;
    GSource *bus_source;
    GstElement *src;
    auto queue1 = gst_element_factory_make("queue", nullptr);
    auto queue2 = gst_element_factory_make("queue", nullptr);
    auto capsFilterIn = gst_element_factory_make("capsfilter", nullptr);
    auto videoconvert = gst_element_factory_make("videoconvert", nullptr);
    auto videoscale = gst_element_factory_make("videoscale", nullptr);
    auto capsFilterOut = gst_element_factory_make("capsfilter", nullptr);
    auto appsink = gst_element_factory_make("appsink", "sink_out");
    auto overlay = gst_element_factory_make("timeoverlay", "overlay");

    src = gst_element_factory_make("appsrc", "source_to_out");
    g_object_set(capsFilterIn, "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "width", G_TYPE_INT, width,
                                     "height", G_TYPE_INT, height,
                                     "framerate", GST_TYPE_FRACTION, framerate, 1,
                                     "format", G_TYPE_STRING, format.c_str(),
                                     NULL),
                 NULL);
    g_object_set(capsFilterOut, "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "width", G_TYPE_INT, width,
                                     "height", G_TYPE_INT, height,
                                     "framerate", GST_TYPE_FRACTION, framerate, 1,
                                     "format", G_TYPE_STRING, format.c_str(),
                                     NULL), NULL);
    g_object_set(queue1,
                 "leaky", 2,
//                 "max-size-buffers", 5,
                 NULL);
    g_object_set(queue2,
                 "leaky", 2,
//                 "max-size-buffers", 5,
                 NULL);
    m_pipe = gst_pipeline_new("pipeline");

    gst_bin_add_many(GST_BIN (m_pipe), src, capsFilterIn,
                     queue1,
                     videoconvert, videoscale, capsFilterOut,
                     queue2,
                     appsink, NULL);
    gst_element_link_many(src, capsFilterIn,
                          queue1,
                          videoconvert, videoscale, capsFilterOut,
                          queue2,
                          appsink, NULL);

    /* instruct the bus to emit signals for each received message, and connect to the interesting signals */
    bus = gst_element_get_bus(m_pipe);
    bus_source = gst_bus_create_watch(bus);
    g_source_set_callback(bus_source, (GSourceFunc) gst_bus_async_signal_func, NULL, NULL);
    g_source_unref(bus_source);
    g_signal_connect (G_OBJECT(bus), "message::error", G_CALLBACK(SinkBase::on_error), this);

    auto source = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    g_object_set(
            source,
            "format", GST_FORMAT_TIME,
            "do-timestamp", TRUE,
//            "block", TRUE,
//            "is-live", TRUE,
            "leaky-type", GST_APP_LEAKY_TYPE_UPSTREAM, // since 1.20
//            "max-latency", 50,
            NULL
    );
    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");
    g_object_set(sink_out,
                 "emit-signals", TRUE,
                 "max-buffers", 1,
                 "drop", true,
                 NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK(SourceApp::on_sample), this);

    /* free resources */
    gst_object_unref(bus);
    gst_object_unref(sink_out);
    gst_object_unref(source);
}

SourceApp::~SourceApp() {
    std::lock_guard<std::mutex> lk(m_lock);
    auto bus = gst_element_get_bus(m_pipe);
    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");
    g_signal_handlers_disconnect_by_data(sink_out, this);
    gst_object_unref(sink_out);
    gst_object_unref(bus);
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceApp::start() {
    std::lock_guard<std::mutex> lk(m_lock);
    startPipe();
}

void SourceApp::pause() {
    pausePipe();
}

void SourceApp::putData(uint8_t *data, uint32_t len) {
    std::lock_guard<std::mutex> lk(m_lock);
    auto source_to_out = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if (ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret << std::endl;
    }
    gst_object_unref(source_to_out);
}

GstFlowReturn SourceApp::on_sample(GstElement *elt, SourceApp *data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample(GST_APP_SINK (elt));
    if (sample != nullptr) {
        int imW = 0, imH = 0;
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *capStr = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(capStr, "width", &imW);
        gst_structure_get_int(capStr, "height", &imH);

        buffer = gst_sample_get_buffer(sample);
        if (buffer != nullptr) {
            if (data != nullptr) {
                auto sinks = data->getSinks();
                for (auto it: sinks) {
                    if (it != nullptr && it->isRunning()) {
                        it->putSample(sample);
                    }
                }
            }
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}