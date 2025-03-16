#include "image_provider.h"

ImageProvider::ImageProvider(uint32_t width, uint32_t height, QObject *parent) : QObject(parent) {
    m_width = width;
    m_height = height;
}

ImageProvider::~ImageProvider() {
    if(m_buf != nullptr) {
        delete[] m_buf;
    }
}

void ImageProvider::setFrame(uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(_lock);
    if(m_buf_len < len) {
        if(m_buf != nullptr) {
            delete[] m_buf;
        }
        m_buf = new uint8_t[len];
        m_buf_len = len;
    }
    memcpy(m_buf, data, len);
    auto image = QImage((const uchar *)m_buf, m_width, m_height, QImage::Format_RGBA8888);
    setImage(image);
}

void ImageProvider::setImage(QImage const &image)
{
    m_image = image;
    emit imageChanged();
}

void ImageProvider::start() {}

QImage ImageProvider::image() const
{
    return m_image;
}
