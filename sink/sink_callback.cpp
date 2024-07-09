#include "sink_callback.h"
#include <iostream>

SinkCallback::SinkCallback() {
    std::cout << TAG << ": created" << std::endl;
}

SinkCallback::~SinkCallback() {
    std::lock_guard<std::mutex> lock(m_lock);
    if (m_pipe) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_pipe));
        m_pipe = nullptr;
    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkCallback::start() {
    gst_element_set_state(m_pipe, GST_STATE_PLAYING);
    std::cout << TAG << ": started" << std::endl;
}

void SinkCallback::pause() {
    std::lock_guard<std::mutex> lock(m_lock);
    gst_element_set_state(m_pipe, GST_STATE_PAUSED);
}


void SinkCallback::setDataCb(std::function<void(uint8_t *, uint32_t)> cb) {
    m_data_cb = cb;
}

void SinkCallback::putSample(GstSample* sample) {
    std::lock_guard<std::mutex> lock(m_lock);
    if(sample != NULL) {
#ifdef USE_TEST_PUT
        if(m_data_cb != NULL) {
            m_data_cb(sample);
        }
#else
        auto buffer = gst_sample_get_buffer(sample);
        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);
            if(m_data_cb != NULL) {
                GstCaps *caps = gst_sample_get_caps(sample);
                GstStructure *capStr = gst_caps_get_structure(caps, 0);
                std::string capsStr2 = gst_structure_to_string(capStr);
//                std::cout << TAG << ": sink-callback caps: " << capsStr2.c_str() << std::endl;
                m_data_cb((uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
#endif
    }
}
