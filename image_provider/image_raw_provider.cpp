#include "image_raw_provider.h"
#include <cstring>

ImageRawProvider::ImageRawProvider() {}

ImageRawProvider::~ImageRawProvider() {
    if(m_buf != nullptr) {
        delete[] m_buf;
    }
}

void ImageRawProvider::setImage(int width, int height, uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(_lock);
    if(m_buf_len < len) {
        if(m_buf != nullptr) {
            delete[] m_buf;
        }
        m_buf = new uint8_t[len];
        m_buf_len = len;
    }
    m_width = width;
    m_height = height;
    memcpy(m_buf, data, len);
}

uint8_t * ImageRawProvider::getBuffer() {
    std::lock_guard<std::mutex> lk(_lock);
    return m_buf;
}

uint64_t ImageRawProvider::getBufferLen() {
    std::lock_guard<std::mutex> lk(_lock);
    return m_buf_len;
}

uint32_t ImageRawProvider::getWidth() {
    return m_width;
}

uint32_t ImageRawProvider::getHeight() {
    return m_height;
}

void ImageRawProvider::start() {}
