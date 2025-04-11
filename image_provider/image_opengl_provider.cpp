#include "image_opengl_provider.h"
#include <cstring>
#include <iostream>

ImageOpenGlProvider::ImageOpenGlProvider(uint32_t width, uint32_t height,
                                         uint64_t fl_texture_id,
                                         uint64_t frame_buf_addr) {
    _fl_texture_id = fl_texture_id;
    _width = width;
    _height = height;
    _frame_buf_addr = frame_buf_addr;
}

ImageOpenGlProvider::~ImageOpenGlProvider() {}

void ImageOpenGlProvider::setFrame(uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(_lock);
    if(len == 0) {
        std::cerr << "setFrame: invalid len: " << len << std::endl;
        return;
    }
    uint32_t expected_len = _width * _height * 4;
    if (len < expected_len) {
        std::cerr << "Frame data too small: " << len << " < " << expected_len << std::endl;
        return;
    }
    if(_frame_buf_addr != 0) {
        uint8_t* buf = (uint8_t*) _frame_buf_addr;
        memcpy(buf, data, len);
    }
    if(onFrameChanged != NULL) {
        onFrameChanged();
    }
}

uint8_t * ImageOpenGlProvider::getBuffer() {
    std::lock_guard<std::mutex> lk(_lock);
    return NULL;
}

uint64_t ImageOpenGlProvider::getBufferLen() {
    std::lock_guard<std::mutex> lk(_lock);
    return 0;
}

uint32_t ImageOpenGlProvider::getWidth() {
    return _width;
}

uint32_t ImageOpenGlProvider::getHeight() {
    return _height;
}

uint64_t ImageOpenGlProvider::getId() {
    return _fl_texture_id;
}

void ImageOpenGlProvider::start() {}
