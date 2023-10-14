#include "sink_encode.h"
#include <iostream>

#include <gst/app/gstappsink.h>
#include <gst/app/app.h>

GstFlowReturn on_sample(GstElement * elt, SinkEncode* data);

SinkEncode::SinkEncode() {
    m_sink = gst_parse_launch(cmd, NULL);
    if (m_sink == NULL) {
        std::cout << "not all elements created" << std::endl;
    }
}

SinkEncode::~SinkEncode() {}

void SinkEncode::start() {
    auto source = gst_bin_get_by_name (GST_BIN (m_sink), "source_to_out");
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    gst_object_unref (source);

    auto sink_out = gst_bin_get_by_name (GST_BIN (m_sink), "sink_out");
    g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), this);
    gst_element_set_state (m_sink, GST_STATE_PLAYING);
}

void SinkEncode::stop() {
    // TODO
}

void SinkEncode::putSample(GstSample* sample) {
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_sink), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source_to_out), sample);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);
}

GstFlowReturn on_sample(GstElement * elt, SinkEncode* data) {
    GstSample *sample;
    GstBuffer *app_buffer, *buffer;
    GstElement *source;
    GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

    /* get the sample from appsink */
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
//        GstCaps *caps = gst_sample_get_caps(sample);
//        GstStructure *s = gst_caps_get_structure(caps, 0);
        buffer = gst_sample_get_buffer(sample);

        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            if(data != NULL) {
//                data->setImage(imW, imH, (uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
            gst_sample_unref(sample);
        } else {
            printf ("BUFFER IS NULL \n\n\n");
        }
    }
    return ret;
}
