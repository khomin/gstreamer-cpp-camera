#include "image_videosink.h"
#include <gst/app/app.h>
#include <iostream>

ImageVideoSink::ImageVideoSink() {
    m_pipe = gst_parse_launch(CMD, NULL);
    if (m_pipe == NULL) {
        std::cerr << TAG << "pipe failed" << std::endl;
    }
    std::cout << TAG << ": created" << std::endl;
}

ImageVideoSink::~ImageVideoSink() {
    std::lock_guard<std::mutex> lk(_lock);
    if (m_pipe) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_pipe));
        m_pipe = nullptr;
        g_print("Pipeline stopped and destroyed\n");
    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void ImageVideoSink::start() {
    std::lock_guard<std::mutex> lk(_lock);
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set(
            source,
            "is-live", TRUE,
            "stream-type", GstAppStreamType::GST_APP_STREAM_TYPE_STREAM,
            "format", GST_FORMAT_TIME,
            "do-timestamp", TRUE,
            NULL
    );
    gst_element_set_state(m_pipe, GST_STATE_PLAYING);
    gst_object_unref (source);
}

void ImageVideoSink::setImage(int width, int height, uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(_lock);
    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if(ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);
}
