#include "image_raw_provider.h"
#include <cstring>
#include <iostream>

ImageRawProvider::ImageRawProvider(uint32_t id, int width, int height) {
    _id = id;
    m_width = width;
    m_height = height;
    int len = width * height * 4;
    m_buf = new uint8_t[len];
    m_buf_len = len;
}

ImageRawProvider::~ImageRawProvider() {
    std::lock_guard<std::mutex> lk(_lock);
    delete[] m_buf;
}

void ImageRawProvider::setFrame(uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(_lock);
    if(len == 0) {
        std::cerr << "setFrame: invalid len: " << len << std::endl;
        return;
    }
    if(m_buf == nullptr) {
        std::cerr << "setFrame: no buffer: " << len << std::endl;
        return;
    }
    if(len > m_buf_len) {
        std::cerr << "setFrame: len > buf: " << len << std::endl;
        return;
    }
    memcpy(m_buf, data, len);}

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

uint32_t ImageRawProvider::getId() {
    return _id;
}

void ImageRawProvider::start() {}
