#include "sink_audio.h"
#include "utils/measure.h"
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>

SinkAudio::SinkAudio() {
    m_pipe = gst_parse_launch("appsrc name=source_to_out ! audio/x-raw,rate=16000,format=S16LE,channels=1,layout=interleaved ! audioconvert ! audioresample ! autoaudiosink", NULL);
    if (m_pipe == NULL) {
        std::cerr << tag << "pipe failed" << std::endl;
        m_error = true;
    }
    std::cout << tag << ": created" << std::endl;
}

SinkAudio::~SinkAudio() {
    std::cout << tag << ": destroyed" << std::endl;
}

void SinkAudio::start() {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    if(source == NULL) m_error = true;
    gst_object_unref (source);
    startPipe();
    std::cout << tag << ": started" << std::endl;
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
