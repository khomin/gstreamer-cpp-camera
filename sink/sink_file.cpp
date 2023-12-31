#include "sink_file.h"
#include <iostream>

#include <gst/app/gstappsink.h>
#include <gst/app/app.h>

SinkFile::SinkFile(std::string path) {
    m_file = std::ofstream(path.c_str(), std::ios::out | std::ios :: binary);
    m_pipe = gst_parse_launch(CMD, NULL);
    if (m_pipe == NULL) {
        std::cerr << TAG << "pipe failed" << std::endl;
    }
    std::cout << TAG << ": created" << std::endl;
}

SinkFile::~SinkFile() {
    std::lock_guard<std::mutex> lock(m_lock);
    stopPipe();
    m_file.close();
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkFile::start() {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    gst_object_unref (source);

    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SinkFile::on_sample), &m_file);
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
    startPipe();
}

void SinkFile::putSample(GstSample* sample) {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source_to_out), sample);
    if(ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref(source_to_out);
}

GstFlowReturn SinkFile::on_sample(GstElement * elt, std::ofstream* file) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        buffer = gst_sample_get_buffer(sample);
        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            if(file != NULL) {
                file->write((char*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
