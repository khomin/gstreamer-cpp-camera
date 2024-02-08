#include "image_raw_provider.h"
#include <cstring>
#include <iostream>

ImageRawProvider::ImageRawProvider() {}

ImageRawProvider::~ImageRawProvider() {
    std::lock_guard<std::mutex> lk(_lock);
    delete[] m_buf;
    m_buf = nullptr;
}

void ImageRawProvider::setImage(int width, int height, uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(_lock);
    if(len == 0) {
        std::cerr << "setImage: invalid len: " << len << std::endl;
        return;
    }
    if(m_buf == nullptr) {
        m_buf = new uint8_t[len];
        m_buf_len = len;
    }
    if(len > m_buf_len) {
        std::cerr << "setImage: len > buf: " << len << std::endl;
        return;
    }
    memcpy(m_buf, data, len);
    m_width = width;
    m_height = height;
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
