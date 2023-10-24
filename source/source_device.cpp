#include "source_device.h"
#include "utils/stringf.h"
#include <iostream>
#include <thread>

GstFlowReturn on_sample (GstElement * elt, SourceDevice* data);

SourceDevice::SourceDevice() : SourceDevice(SourceDeviceType::Screen) {}

SourceDevice::SourceDevice(SourceDeviceType type, OptionType option) : m_type(type) {
       std::string cmd = StringFormatter::format(
                                type == SourceDeviceType::Screen ? cmd_screen : cmd_webc,
#if __APPLE__
                                cmd_screen_macos,
#elif _WIN32
                                cmd_screen_win,
#else
                                cmd_screen_linux,
#endif
                                option == OptionType::TimeOverlay ? show_timeoverlay : ""
                                );
    m_pipe = gst_parse_launch(cmd.c_str(), NULL);
    if (m_pipe == NULL) {
        std::cerr << tag << "pipe failed" << std::endl;
        m_error = true;
    }
    /* we use appsink in push mode, it sends us a signal when data is available
    * and we pull out the data in the signal callback. We want the appsink to
    * push as fast as it can, hence the sync=false */
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), this);
    gst_object_unref (sink_out);
    std::cout << tag << ": created" << std::endl;
}

SourceDevice::~SourceDevice() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
        auto src = gst_bin_get_by_name (GST_BIN (m_pipe), "src");
        auto rb = gst_bin_remove(GST_BIN(m_pipe),GST_ELEMENT(sink_out));
        auto rb2 = gst_bin_remove(GST_BIN(m_pipe),GST_ELEMENT(src));
        gst_object_unref (sink_out);
        gst_object_unref (src);
        gst_object_unref(m_pipe);
    }
    std::cout << tag << ": destroyed" << std::endl;
}

void SourceDevice::start() {
    if(m_pipe != NULL) {
        gst_element_set_state (m_pipe, GST_STATE_PAUSED);
        gst_element_set_state (m_pipe, GST_STATE_PLAYING);
    }
}

void SourceDevice::pause() {
    gst_element_set_state (m_pipe, GST_STATE_PAUSED);
}

GstFlowReturn on_sample(GstElement * elt, SourceDevice* data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));
    if(sample != NULL) {
        buffer = gst_sample_get_buffer(sample);
        if (buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            if (data != NULL) {
                auto sinks = data->sinks;
                for (auto &it: sinks) {
                    if(it->isRunning()) {
                        it->putSample(sample);
                    }
                }
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
