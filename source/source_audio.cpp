#include "source_audio.h"
#include "fmt/core.h"
#include <iostream>
#include <thread>

SourceAudio::SourceAudio() {
    m_pipe = gst_parse_launch("audiotestsrc ! audio/x-raw,rate=16000,format=S16LE,channels=1,layout=interleaved ! audioconvert ! audioresample ! appsink name=sink_out", NULL);
    if (m_pipe == NULL) {
        std::cerr << tag << "pipe failed" << std::endl;
        m_error = true;
    }
    /* we use appsink in push mode, it sends us a signal when data is available
    * and we pull out the data in the signal callback. We want the appsink to
    * push as fast as it can, hence the sync=false */
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SourceAudio::on_sample), this);
    gst_object_unref (sink_out);
    std::cout << tag << ": created" << std::endl;
}

SourceAudio::~SourceAudio() {
    std::lock_guard<std::mutex> lk(m_lock);
    auto bus = gst_element_get_bus (m_pipe);
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_signal_handlers_disconnect_by_data(sink_out, this);
    gst_object_unref (sink_out);
    gst_object_unref (bus);
    std::cout << tag << ": destroyed" << std::endl;
}

void SourceAudio::start() {
    if(m_pipe != NULL) {
        gst_element_set_state (m_pipe, GST_STATE_PLAYING);
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
