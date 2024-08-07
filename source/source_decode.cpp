#include "source_decode.h"
#include "utils/measure.h"
#include <gst/pbutils/codec-utils.h>
#include "config.h"
#include <iostream>

SourceDecode::SourceDecode(DecoderConfig config) {
    GError *error = NULL;
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
            CMD,
            config.codecInVideo.c_str(),
            config.pixelFormat.c_str(),
            config.width, config.height,
            config.framerate,
            config.bitrate,
            config.decoder.c_str());
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
    if (m_pipe == NULL) {
        std::cerr << TAG << "pipe failed" << std::endl;
    }
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_free (message);
    }
    std::cout << TAG << ": created" << std::endl;
}

SourceDecode::~SourceDecode() {
    std::lock_guard<std::mutex> lock(m_lock);
    if (m_pipe) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_pipe));
        m_pipe = nullptr;
    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceDecode::start() {
    std::lock_guard<std::mutex> lk(m_lock);
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    if (source == NULL) {
        std::cout << "value is NULL" << std::endl;
    }
    g_object_set(
        source,
        "is-live", TRUE,
        "stream-type", GstAppStreamType::GST_APP_STREAM_TYPE_STREAM,
        "format", GST_FORMAT_TIME,
        "do-timestamp", TRUE,
        NULL
    );
    gst_object_unref (source);

    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    if (sink_out == NULL) {
        std::cout << "value is NULL" << std::endl;
    } else {
        g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
        g_signal_connect (sink_out, "new-sample", G_CALLBACK (SourceDecode::on_sample), this);
        gst_object_unref (sink_out);
    }
    gst_element_set_state(m_pipe, GST_STATE_PLAYING);
}

void SourceDecode::pause() {}

void SourceDecode::putData(uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(m_lock);
    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);
    auto source_to_out = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if (ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret << std::endl;
    }
    gst_object_unref(source_to_out);
    Measure::instance()->onDecodePutSample();
}

GstFlowReturn SourceDecode::on_sample(GstElement * elt, SourceDecode* data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        buffer = gst_sample_get_buffer(sample);
        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);
            Measure::instance()->onDecodeSampleReady();
            auto sinks = data->getSinks();
            for(auto it : sinks) {
                if (it != nullptr) {
                    it->putSample(sample);
                }
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
