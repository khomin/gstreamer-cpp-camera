#include "source_device.h"
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

SourceDevice::SourceDevice(int width, int height, SourceDeviceType type, OptionType option) {
    GstBus *bus;
    GSource *bus_source;
    GstElement *src;
    m_dev_type = type;
    this->width = width;
    this->height = height;
    auto queue = gst_element_factory_make("queue", nullptr);
    auto capsFilterIn = gst_element_factory_make("capsfilter", nullptr);
    auto videoconvert = gst_element_factory_make("videoconvert", nullptr);
    auto videoscale = gst_element_factory_make("videoscale", nullptr);
    auto capsFilterOut = gst_element_factory_make("capsfilter", nullptr);
    auto appsink = gst_element_factory_make("appsink", "sink_out");
    auto overlay = gst_element_factory_make("timeoverlay", "overlay");
#if __APPLE__
    type == SourceDeviceType::Screen
#elif _WIN32
    type == SourceDeviceType::Screen ? CMD_SCREEN_WIN : CMD_CAMERA_WIN,
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
                                     "framerate", GST_TYPE_FRACTION, 20, 1,
                                     "format", G_TYPE_STRING, "RGBA",
                                     NULL),
                 NULL);
#else
    // LINUX
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
                                     "framerate", GST_TYPE_FRACTION, 20, 1,
                                     "format", G_TYPE_STRING, "RGB",
                                     NULL), NULL);


    g_object_set(queue,
                 "leaky", 2,
                 "max-size-buffers", 5,
                 NULL);

    m_pipe = gst_pipeline_new("pipeline");

    if (option == OptionType::TimeOverlay) {
        gst_bin_add_many(GST_BIN (m_pipe), src, capsFilterIn, overlay, videoconvert, videoscale,
                         capsFilterOut, appsink,
                         NULL);
        gst_element_link_many(src, capsFilterIn, overlay, videoconvert, videoscale, capsFilterOut,
                              appsink, NULL);
    } else {
        gst_bin_add_many(GST_BIN (m_pipe), src, capsFilterIn, videoconvert, videoscale,
                         capsFilterOut, queue, appsink, NULL);
        gst_element_link_many(src, capsFilterIn, videoconvert, videoscale, capsFilterOut, queue,
                              appsink, NULL);
    }
    /* instruct the bus to emit signals for each received message, and connect to the interesting signals */
    bus = gst_element_get_bus(m_pipe);
    bus_source = gst_bus_create_watch(bus);
    g_source_set_callback(bus_source, (GSourceFunc) gst_bus_async_signal_func, NULL, NULL);
    g_source_unref(bus_source);
    g_signal_connect (G_OBJECT(bus), "message::error", G_CALLBACK(SinkBase::on_error), this);

    auto source = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
//#ifdef __ANDROID__
    if (type != SourceDeviceType::Test) {
        g_object_set(
                source,
                "format", GST_FORMAT_TIME,
                "do-timestamp", TRUE,
//                "block", TRUE,
//                "is-live", TRUE,
                NULL
        );
    }
//#endif
    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");
    g_object_set(sink_out,
                 "emit-signals", TRUE,
                 "max-buffers", 1,
                 "drop", true,
                 NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK(SourceDevice::on_sample), this);

    /* free resources */
    gst_object_unref(bus);
    gst_object_unref(sink_out);
    gst_object_unref(source);
}

SourceDevice::~SourceDevice() {
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_device_platform_interface != nullptr) {
        m_device_platform_interface->onStopSource();
    }
    auto bus = gst_element_get_bus(m_pipe);
    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");
    g_signal_handlers_disconnect_by_data(sink_out, this);
    gst_object_unref(sink_out);
    gst_object_unref(bus);
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceDevice::start() {
    std::lock_guard<std::mutex> lk(m_lock);
    startPipe();
    if (m_device_platform_interface != nullptr) {
        m_device_platform_interface->onStartSource(
                m_dev_type == SourceDeviceType::Camera1 ? "1" : "2", width, height);
    }
}

void SourceDevice::pause() {
    pausePipe();
}

void SourceDevice::onConfigChanged(std::function<void(int, int)> cb) {
    m_config_changed = cb;
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

void SourceDevice::setDevicePlatformInterface(IVideoDevicePlatform *v) {
    m_device_platform_interface = v;
}

GstFlowReturn SourceDevice::on_sample(GstElement *elt, SourceDevice *data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample(GST_APP_SINK (elt));
    if (sample != nullptr) {
        int imW = 0, imH = 0;
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *capStr = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(capStr, "width", &imW);
        gst_structure_get_int(capStr, "height", &imH);

        if (data != nullptr) {
            if (data->m_first_frame) {
                data->m_first_frame = false;
                if (data->m_config_changed != nullptr) {
                    data->m_config_changed(imW, imH);
                }
            }
        }
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

std::pair<int, int> SourceDevice::getSize() {
    return std::pair(width,height);
}
