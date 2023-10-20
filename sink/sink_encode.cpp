#include "sink_encode.h"
#include "utils/stringf.h"
#include "utils/measure.h"
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>

GstFlowReturn on_sample(GstElement * elt, std::function<void(uint8_t*, uint32_t, uint32_t, uint32_t)>* data);

SinkEncode::SinkEncode(EncoderConfig config) {
    m_config = config;
    std::string cmdf = StringFormatter::format(cmd,
                config.pixelFormat.c_str(),
                config.width, config.height,
                config.framerate,
                (config.codec + " " + config.codecOptions).c_str(),
                config.bitrate
                );
    m_pipe = gst_parse_launch(cmdf.c_str(), NULL);
    if (m_pipe == NULL) {
        std::cerr << tag << "pipe failed" << std::endl;
        m_error = true;
    }
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set(
        source,
        "is-live", TRUE,
        "stream-type", 0,
        "format", GST_FORMAT_TIME,
//        "leaky-type", GST_APP_LEAKY_TYPE_UPSTREAM, // since 1.20
        "do-timestamp", TRUE,
//        "max-buffers", 2, // didn't work
        NULL
      );
    if(source == NULL) {
        m_error = true;
    }
    gst_object_unref (source);

    std::cout << tag << ": created" << std::endl;
}

SinkEncode::~SinkEncode() {
    m_on_encoded = NULL;
    if(m_pipe != NULL) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        gst_object_unref(m_pipe);
    }
    std::cout << tag << ": destroyed" << std::endl;
}

void SinkEncode::start() {
    if(m_on_encoded != NULL) {
        auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
        g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
        g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), &m_on_encoded);
        if(sink_out == NULL) {
            m_error = true;
        }
        gst_object_unref (sink_out);
    }
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
}

void SinkEncode::putSample(GstSample* sample) {
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source), sample);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source);
    Measure::instance()->onEncodePutSample();
}

void SinkEncode::setOnEncoded(std::function<void(uint8_t*, uint32_t, uint32_t, uint32_t)> cb) {
    m_on_encoded = cb;
}

GstFlowReturn on_sample(GstElement * elt, std::function<void(uint8_t*, uint32_t, uint32_t, uint32_t)>* data) {
    GstSample *sample;
    GstBuffer *app_buffer, *buffer;
    GstElement *source;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        buffer = gst_sample_get_buffer(sample);

        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            Measure::instance()->onEncodeSampleReady();

            if(data != NULL) {
                (*data)((uint8_t*)mapInfo.data, mapInfo.size, buffer->pts, buffer->dts);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
