#include "source_audio.h"
#include "config.h"
#include <iostream>
#include <thread>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

SourceAudio::SourceAudio() {
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
        CMD,
        #if defined(__ANDROID__)
            "openslessrc"
        #elif defined(__linux__)
            "autoaudiosrc"
        #elif defined(__APPLE__)
        #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
            "osxaudiosrc"
        #elif TARGET_OS_MAC
            "osxaudiosrc"
        #else
        #error "Unknown Apple platform"
        #endif
        #elif defined(_WIN32) || defined(_WIN64)
            "autoaudiosrc"
        #else
        #error "Unknown platform"
        #endif
    );
    GError *error = NULL;
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
    if (!m_pipe) {
        std::cerr << TAG << "pipe failed" << std::endl;
        m_error = true;
    }
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_free (message);
    }
    /* we use appsink in push mode, it sends us a signal when data is available
    * and we pull out the data in the signal callback. We want the appsink to
    * push as fast as it can, hence the sync=false */
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    if(!sink_out) {
        std::cout << TAG << ": sink is null" << std::endl;
    }
    g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SourceAudio::on_sample), this);
    gst_object_unref (sink_out);
    std::cout << TAG << ": created" << std::endl;
}

SourceAudio::~SourceAudio() {
    std::lock_guard<std::mutex> lk(m_lock);
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_signal_handlers_disconnect_by_data(sink_out, this);
    gst_object_unref (sink_out);
    if (m_pipe) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_pipe));
        m_pipe = nullptr;
    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceAudio::start() {
    if(m_pipe != NULL) {
        gst_element_set_state (m_pipe, GST_STATE_PLAYING);
        std::cout << TAG << ": start" << std::endl;
    }
}

void SourceAudio::pause() {
    gst_element_set_state (m_pipe, GST_STATE_PAUSED);
}

GstFlowReturn SourceAudio::on_sample(GstElement * elt, SourceAudio* data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));
    if(sample != nullptr) {
        buffer = gst_sample_get_buffer(sample);
        if (buffer != nullptr) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            if (data != nullptr) {
                auto sinks = data->getSinks();
                for (auto it: sinks) {
                    if (it != nullptr) {
                        // if you need caps info
                        //GstCaps *caps = gst_sample_get_caps(sample);
                        //const GstStructure *capStr = gst_caps_get_structure(caps, 0);
                        //std::string capsStr2 = gst_structure_to_string(capStr);
                        //std::cout << TAG << ": caps: " << capsStr2.c_str() << std::endl;
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
