#include "source_device.h"
#include "fmt/core.h"
#include <iostream>
#include <thread>

SourceDevice::SourceDevice(SourceDeviceType type, OptionType option) {
    auto cmdF = fmt::format(
            cmd,
#if __APPLE__
            type == SourceDeviceType::Screen ? cmd_screen_macos : cmd_camera_macos,
#elif _WIN32
            type == SourceDeviceType::Screen ? cmd_screen_win : cmd_camera_win,
#else
            type == SourceDeviceType::Screen ? cmd_screen_linux : cmd_camera_linux,
#endif
            option == OptionType::TimeOverlay ? show_time_overlay : ""
    );
    m_pipe = gst_parse_launch(cmdF.c_str(), NULL);
    if (m_pipe == NULL) {
        std::cerr << tag << "pipe failed" << std::endl;
        m_error = true;
    }
    /* we use appsink in push mode, it sends us a signal when data is available
    * and we pull out the data in the signal callback. We want the appsink to
    * push as fast as it can, hence the sync=false */
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SourceDevice::on_sample), this);
    gst_object_unref (sink_out);
    std::cout << tag << ": created" << std::endl;
}

SourceDevice::~SourceDevice() {
    std::lock_guard<std::mutex> lk(m_lock);
    auto bus = gst_element_get_bus (m_pipe);
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_signal_handlers_disconnect_by_data(sink_out, this);
    gst_object_unref (sink_out);
    gst_object_unref (bus);
    std::cout << tag << ": destroyed" << std::endl;
}

void SourceDevice::start() {
    if(m_pipe != NULL) {
        gst_element_set_state (m_pipe, GST_STATE_PLAYING);
    }
}

void SourceDevice::pause() {
    gst_element_set_state (m_pipe, GST_STATE_PAUSED);
}

void SourceDevice::onConfig(std::function<void(uint32_t ,uint32_t)> cb) {
    m_config_changed = cb;
}

GstFlowReturn SourceDevice::on_sample(GstElement * elt, SourceDevice* data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));
    if(sample != nullptr) {
        int imW = 0, imH = 0;
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *capStr = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(capStr,"width", &imW);
        gst_structure_get_int(capStr, "height", &imH);

        if (data != nullptr) {
            if(data->m_width != imW || data->m_height != imH) {
                data->m_width = imW;
                data->m_height = imH;
                if(data->m_config_changed != nullptr) {
                    data->m_config_changed(imW, imH);
                }
            }
        }

        buffer = gst_sample_get_buffer(sample);
        if (buffer != nullptr) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            if (data != nullptr) {
                auto sinks = data->getSinks();
                for (auto it: sinks) {
                    if (it != nullptr && it->isRunning()) {
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
