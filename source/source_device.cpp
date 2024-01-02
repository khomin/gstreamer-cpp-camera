#include "source_device.h"
#include "config.h"
#include <iostream>
#include <thread>

SourceDevice::SourceDevice(SourceDeviceType type, OptionType option) {
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
            CMD,
#if __APPLE__
            type == SourceDeviceType::Screen ? CMD_SCREEN_MACOS : CMD_CAMERA_MACOS,
#elif _WIN32
            type == SourceDeviceType::Screen ? CMD_SCREEN_WIN : CMD_CAMERA_WIN,
#else
            type == SourceDeviceType::Screen ? CMD_SCREEN_LINUX : CMD_CAMERA_LINUX,
#endif
            option == OptionType::TimeOverlay ? CMD_TIME_OVERLAY : ""
    );
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), NULL);
    if (m_pipe == NULL) {
        std::cerr << TAG << "pipe failed" << std::endl;
        m_error = true;
    }
    /* we use appsink in push mode, it sends us a signal when data is available
    * and we pull out the data in the signal callback. We want the appsink to
    * push as fast as it can, hence the sync=false */
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SourceDevice::on_sample), this);
    gst_object_unref (sink_out);
    std::cout << TAG << ": created" << std::endl;
}

SourceDevice::~SourceDevice() {
    std::lock_guard<std::mutex> lk(m_lock);
    auto bus = gst_element_get_bus (m_pipe);
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_signal_handlers_disconnect_by_data(sink_out, this);
    gst_object_unref (sink_out);
    gst_object_unref (bus);
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceDevice::start() {
    if(m_pipe != NULL) {
        gst_element_set_state (m_pipe, GST_STATE_PLAYING);
    }
}

void SourceDevice::pause() {
    gst_element_set_state (m_pipe, GST_STATE_PAUSED);
}

void SourceDevice::onConfig(std::function<void(int,int)> cb) {
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
