#include "sink_encode.h"
#include "utils/measure.h"
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>

SinkEncode::SinkEncode(EncoderConfig config) {
    GError *error = NULL;
    m_config = config;
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
            CMD,
            config.pixelFormat.c_str(),
            config.width, config.height,
            config.framerate,
            (config.codec + " " + config.codecOptions).c_str()
    );
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
    if (m_pipe == NULL) {
        std::cerr << TAG << "pipe failed" << std::endl;
        m_error = true;
    }
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_free (message);
        m_error = true;
    }
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set(
        source,
        "is-live", TRUE,
        "stream-type", 0,
        "format", GST_FORMAT_TIME,
//#ifdef GST_APP_LEAKY_TYPE_UPSTREAM
        "leaky-type", GST_APP_LEAKY_TYPE_UPSTREAM, // since 1.20
//#endif
        "do-timestamp", TRUE,
        NULL
      );
    if(source == NULL) {
        m_error = true;
    }
    gst_object_unref (source);
    std::cout << TAG << ": created" << std::endl;
}

SinkEncode::~SinkEncode() {
    std::lock_guard<std::mutex> lock(m_lock);
    auto bus = gst_element_get_bus (m_pipe);
    g_signal_handlers_disconnect_by_func(bus, reinterpret_cast<gpointer>(SinkEncode::on_sample), this);
    m_on_encoded = nullptr;
    gst_object_unref (bus);
    std::cout << TAG << ": destroyed, id: " << m_signal_id << std::endl;
}

void SinkEncode::start() {
    std::lock_guard<std::mutex> lock(m_lock);
    if(m_on_encoded != NULL) {
        auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
        g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
        m_signal_id = g_signal_connect (sink_out, "new-sample", G_CALLBACK (SinkEncode::on_sample), this);
        std::cout << TAG << ": GOT signal id: " << m_signal_id << std::endl;
        if(sink_out == NULL) {
            m_error = true;
        }
        gst_object_unref (sink_out);
        startPipe();
    }
}

void SinkEncode::putData(uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(m_lock);
    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);
    auto source_to_out = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if (ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret << std::endl;
    }
    gst_object_unref(source_to_out);
}

void SinkEncode::putSample(GstSample* sample) {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source), sample);
    if(ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source);
    Measure::instance()->onEncodePutSample();
}

void SinkEncode::setOnEncoded(OnEncoded cb) {
    std::lock_guard<std::mutex> lock(m_lock);
    m_on_encoded = cb;
}

GstFlowReturn SinkEncode::on_sample(GstElement * elt, SinkEncode* data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        buffer = gst_sample_get_buffer(sample);

        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);
            Measure::instance()->onEncodeSampleReady();
            if(data != NULL && data->m_on_encoded != NULL) {
                data->m_on_encoded((uint8_t*)mapInfo.data, mapInfo.size, buffer->pts, buffer->dts);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
