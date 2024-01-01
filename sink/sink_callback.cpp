#include "sink_callback.h"
#include <iostream>

SinkCallback::SinkCallback() {
    std::cout << TAG << ": created" << std::endl;
}

SinkCallback::~SinkCallback() {
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkCallback::start() {
    startPipe();
    std::cout << TAG << ": started" << std::endl;
}

#ifdef USE_TEST_PUT
void SinkCallback::setDataCb(std::function<void(GstSample *)> cb) {
    m_data_cb = cb;
}
#else
void SinkCallback::setDataCb(std::function<void(uint8_t *, uint32_t)> cb) {
    m_data_cb = cb;
}
#endif

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
                m_data_cb((uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
#endif
    }
}