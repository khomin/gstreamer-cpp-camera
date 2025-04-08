#include "image_opengl_provider.h"
#include <cstring>
#include <iostream>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

ImageOpenGlProvider::ImageOpenGlProvider(uint32_t width, uint32_t height,
                                         uint64_t fl_texture_id,
                                         uint64_t texture_addr,
                                         uint64_t frame_buf_addr) {
    _fl_texture_id = fl_texture_id;
    _width = width;
    _height = height;
    _texture_addr = texture_addr;
    _frame_buf_addr = frame_buf_addr;

//    // generate a texture ID
//    glGenTextures(1, &_gl_id);
//    // bind the texture
//    glBindTexture(GL_TEXTURE_2D, _gl_id);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
//    // configure the texture parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    // unbind the texture (optional)
//    glBindTexture(GL_TEXTURE_2D, 0);
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
//        uintptr_t addr = _frame_buf_addr;
//        uint8_t* framebuffer = reinterpret_cast<uint8_t*>(addr);
//        auto buf = reinterpret_cast<uint8_t*>(addr);
//        uint64_t _frame_buf_addr = 0;
        uint8_t* buf = (uint8_t*) _frame_buf_addr;
        memcpy(buf, data, len);
        std::cout << "BTEST_texture_CPP: " << _frame_buf_addr << std::endl;
    }

//    glBindTexture(GL_TEXTURE_2D, _gl_id);
////    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glBindTexture(GL_TEXTURE_2D, 0);

    if(onFrameChanged != NULL) {
        onFrameChanged();
    }
}

uint8_t * ImageOpenGlProvider::getBuffer() {
    std::lock_guard<std::mutex> lk(_lock);
//    return m_buf;
    return NULL;
}

uint64_t ImageOpenGlProvider::getBufferLen() {
    std::lock_guard<std::mutex> lk(_lock);
//    return m_buf_len;
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
