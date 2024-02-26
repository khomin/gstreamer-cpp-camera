#include "source_device.h"
#include "config.h"
#include <iostream>
#include <thread>

std::vector<std::shared_ptr<SinkBase>> sinks;
std::mutex sinks_lock;

SourceDevice::SourceDevice(int width, int height, int framerate, SourceDeviceType type, OptionType option) {
    GstBus *bus;
    GSource *bus_source;
    GstElement *src;
    m_dev_type = type;
    this->width = width;
    this->height = height;
    auto capsFilterIn = gst_element_factory_make("capsfilter", nullptr);
    auto videoconvert = gst_element_factory_make("videoconvert", nullptr);
    auto videoscale = gst_element_factory_make("videoscale", nullptr);
    auto videorate = gst_element_factory_make("videorate", nullptr);
    auto capsFilterOut = gst_element_factory_make("capsfilter", nullptr);
    auto appsink = gst_element_factory_make("appsink", "sink_out");
    auto overlay = gst_element_factory_make("timeoverlay", "overlay");

#if __APPLE__
    if (type == SourceDeviceType::Screen) {
        src = gst_element_factory_make("avfvideosrc", "source_to_out");
        g_object_set(src,"capture-screen", TRUE, "capture-screen-cursor", TRUE, NULL);
    } else if (type == SourceDeviceType::Camera1 || type == SourceDeviceType::Camera2) {
        src = gst_element_factory_make("avfvideosrc", "source_to_out");
    } else if (type == SourceDeviceType::Test) {
        src = gst_element_factory_make("videotestsrc", "source_to_out");
    }
#elif _WIN32
    if (type == SourceDeviceType::Screen) {
        src = gst_element_factory_make("dx9screencapsrc", "source_to_out");
    } else if (type == SourceDeviceType::Camera1 || type == SourceDeviceType::Camera2) {
        src = gst_element_factory_make("ksvideosrc", "source_to_out");
    } else if (type == SourceDeviceType::Test) {
        src = gst_element_factory_make("videotestsrc", "source_to_out");
    }
#elif __ANDROID__
    if (type == SourceDeviceType::Screen || type == SourceDeviceType::Camera1 ||
        type == SourceDeviceType::Camera2) {
        src = gst_element_factory_make("appsrc", "source_to_out");
    } else if (type == SourceDeviceType::Test) {
        src = gst_element_factory_make("videotestsrc", "source_to_out");
    }
    g_object_set(capsFilterIn, "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "width", G_TYPE_INT, width,
                                     "height", G_TYPE_INT, height,
                                     "framerate", GST_TYPE_FRACTION, framerate, 1,
                                     "format", G_TYPE_STRING, "RGBA",
                                     NULL),
                 NULL);
    if (type != SourceDeviceType::Test) {
        g_object_set(
                src,
                "format", GST_FORMAT_TIME,
                "do-timestamp", TRUE,
                "leaky-type", GST_APP_LEAKY_TYPE_UPSTREAM, // since 1.20
                "is-live", TRUE,
                NULL
        );
    }
#elif __linux__
    if (type == SourceDeviceType::Screen) {
        src = gst_element_factory_make("ximagesrc", "source_to_out");
    } else if (type == SourceDeviceType::Camera1 || type == SourceDeviceType::Camera2) {
        src = gst_element_factory_make("v4l2src", "source_to_out");
    } else if (type == SourceDeviceType::Test) {
        src = gst_element_factory_make("videotestsrc", "source_to_out");
    }
#endif
    if (option == OptionType::TimeOverlay) {
        g_object_set(overlay,
                     "font-desc", "Sans, 30", "halignment", /* position */ 1,
                     "deltay", 50,
                     "valignment", /* position */ 3,
                     NULL);
    }
    g_object_set(capsFilterOut, "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "width", G_TYPE_INT, width,
                                     "height", G_TYPE_INT, height,
                                     "framerate", GST_TYPE_FRACTION, framerate, 1,
                                     "format", G_TYPE_STRING, "RGB",
                                     NULL), NULL);

    m_pipe = gst_pipeline_new("pipeline");

    if (option == OptionType::TimeOverlay) {
        gst_bin_add_many(GST_BIN (m_pipe), src, capsFilterIn, overlay, videoconvert, videoscale,
                         capsFilterOut, appsink,
                         NULL);
        gst_element_link_many(src, capsFilterIn, overlay, videoconvert, videoscale, capsFilterOut,
                              appsink, NULL);
    } else {
        gst_bin_add_many(GST_BIN (m_pipe), src, capsFilterIn,
                         videorate,
                         videoconvert, videoscale,
                         capsFilterOut,
                         appsink, NULL);
        if (gst_element_link_many(src, capsFilterIn,
                                  videorate,
                                    videoconvert, videoscale,
                                  capsFilterOut,
                                  appsink, NULL) != TRUE) {
            g_printerr("Elements could not be linked.\n");
        }
    }
    /* instruct the bus to emit signals for each received message, and connect to the interesting signals */
    bus = gst_element_get_bus(m_pipe);
    bus_source = gst_bus_create_watch(bus);
    g_source_set_callback(bus_source, (GSourceFunc) gst_bus_async_signal_func, NULL, NULL);
    g_source_unref(bus_source);
    g_signal_connect (G_OBJECT(bus), "message::error", G_CALLBACK(SinkBase::on_error), this);

    /* free resources */
    gst_object_unref(bus);
}

SourceDevice::~SourceDevice() {
    std::lock_guard<std::mutex> lk(m_lock);
    sinks_lock.lock();
    sinks.clear();
    sinks_lock.unlock();
    auto bus = gst_element_get_bus(m_pipe);
    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");
    g_signal_handlers_disconnect_by_data(sink_out, this);
    gst_object_unref(sink_out);
    gst_object_unref(bus);
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceDevice::start() {
    std::lock_guard<std::mutex> lk(m_lock);
    sinks_lock.lock();
    sinks = getSinks();
    sinks_lock.unlock();
    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");
    g_object_set(sink_out,
                 "emit-signals", TRUE,
                 "max-buffers", 1,
                 "drop", true,
                 NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK(SourceDevice::on_sample), nullptr);
    startPipe();
    gst_object_unref(sink_out);
}

void SourceDevice::pause() {
    pausePipe();
}

void SourceDevice::putVideoFrame(uint8_t *data, uint32_t len, int width, int height) {
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

GstFlowReturn SourceDevice::on_sample(GstElement *elt, void *data) {
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
            sinks_lock.lock();
            for (const auto& it: sinks) {
                if (it != nullptr && it->isRunning()) {
                    it->putSample(sample);
                }
            }
            sinks_lock.unlock();
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}

std::pair<int, int> SourceDevice::getSize() {
    return std::pair(width, height);
}

SourceDevice::SourceDeviceType SourceDevice::getType() {
    return m_dev_type;
}
