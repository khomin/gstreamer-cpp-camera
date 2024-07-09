#include "sink_audio.h"
#include "utils/measure.h"
#include "config.h"
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <vector>
#include <thread>

SinkAudio::SinkAudio() {
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
        CMD,
        #if defined(__ANDROID__)
            "openslessink stream-type=voice"
        #elif defined(__linux__)
            "autoaudiosink"
        #elif defined(__APPLE__)
        #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
            "autoaudiosink"
        #elif TARGET_OS_MAC
            "autoaudiosink"
        #else
        #error "Unknown Apple platform"
        #endif
        #elif defined(_WIN32) || defined(_WIN64)
            "autoaudiosink"
        #else
        #error "Unknown platform"
        #endif
    );
    GError *error = NULL;
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
    if (!m_pipe) {
        std::cerr << TAG << "pipe failed" << std::endl;
    }
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_free (message);
    }
    std::cout << TAG << ": created" << std::endl;
}

SinkAudio::~SinkAudio() {
    std::lock_guard<std::mutex> lock(m_lock);
    if (m_pipe) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_pipe));
        m_pipe = nullptr;
    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkAudio::start() {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    gst_element_set_state(m_pipe, GST_STATE_PLAYING);
    gst_object_unref (source);
    std::cout << TAG << ": started" << std::endl;
}

void SinkAudio::pause() {
    std::lock_guard<std::mutex> lock(m_lock);
    gst_element_set_state(m_pipe, GST_STATE_PAUSED);
}

void SinkAudio::putSample(GstSample* sample) {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source), sample);
    if(ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source);
    Measure::instance()->onEncodePutSample();
}

void SinkAudio::putData(uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(m_lock);

    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);

    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if(ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);
}
