#include "sink_callback.h"
#include <iostream>

SinkCallback::SinkCallback() {
    std::cout << TAG << ": created" << std::endl;
}

SinkCallback::~SinkCallback() {
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkCallback::start() {
    std::cout << TAG << ": started" << std::endl;
}

void SinkCallback::pause() {
    std::cout << TAG << ": paused" << std::endl;
}

void SinkCallback::onData(std::function<void(uint8_t *, uint32_t)> cb) {
    m_on_data = cb;
}

void SinkCallback::putSample(GstSample* sample) {
    std::lock_guard<std::mutex> lock(m_lock);
    if(sample != NULL) {
        auto buffer = gst_sample_get_buffer(sample);
        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);
            if(m_on_data != NULL) {
#ifdef PRINT_CAPS
                GstCaps *caps = gst_sample_get_caps(sample);
                GstStructure *capStr = gst_caps_get_structure(caps, 0);
                std::string capsStr2 = gst_structure_to_string(capStr);
                std::cout << TAG << ": sink-callback caps: " << capsStr2.c_str() << std::endl;
                gst_caps_unref(caps);
#endif
                m_on_data((uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
    }
}
