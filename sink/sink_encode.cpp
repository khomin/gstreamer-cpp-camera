#include "sink_encode.h"
#include <iostream>
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>

#include <fstream>

GstFlowReturn on_sample(GstElement * elt, SinkEncode* data);

//std::ofstream m_file;

SinkEncode::SinkEncode() {
    m_pipe = gst_parse_launch(cmd, NULL);
    if (m_pipe == NULL) {
        std::cout << "not all elements created" << std::endl;
    }
//    m_file = std::ofstream("/Users/khominvladimir/Desktop/raw.mp4", std::ios::out | std::ios :: binary);
}

SinkEncode::~SinkEncode() {
    m_on_encoded = NULL;
}

void SinkEncode::start() {
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    gst_object_unref (source);

    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), this);
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
}

void SinkEncode::stop() {
    // TODO
}

void SinkEncode::putSample(GstSample* sample) {
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source_to_out), sample);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);
}

void SinkEncode::setOnEncoded(std::function<void(uint8_t*, uint32_t)> cb) {
    m_on_encoded = cb;
}

void SinkEncode::setOnEncodedSample(std::function<void(GstSample*)> cb) {
    m_on_encoded_sample_cb = cb;
}

void SinkEncode::putEncoded(uint8_t* data, uint32_t len) {
    if(m_on_encoded != NULL) {
        m_on_encoded(data, len);
    }
}

void SinkEncode::putEncodedSample(GstSample* sample) {
    if(m_on_encoded_sample_cb != NULL) {
        m_on_encoded_sample_cb(sample);
    }
}

GstFlowReturn on_sample(GstElement * elt, SinkEncode* data) {
    GstSample *sample;
    GstBuffer *app_buffer, *buffer;
    GstElement *source;
    GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

    /* get the sample from appsink */
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        buffer = gst_sample_get_buffer(sample);

        if(buffer != NULL && data != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

//            if(m_file != NULL) {
//                m_file.write((char*)mapInfo.data, mapInfo.size);
//            }

            data->putEncoded((uint8_t*)mapInfo.data, mapInfo.size);
            data->putEncodedSample(sample);

            gst_buffer_unmap(buffer, &mapInfo);
            gst_sample_unref(sample);
        } else {
            printf ("BUFFER IS NULL \n\n\n");
        }
    }
    return ret;
}
