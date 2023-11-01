#include "image_provider.h"

ImageProvider::ImageProvider(QObject *parent)
    : QObject(parent)
{}

ImageProvider::~ImageProvider() {
    if(m_buf != nullptr) {
        delete[] m_buf;
    }
}

void ImageProvider::setImage(int width, int height, uint8_t* data, uint32_t len) {
    std::lock_guard<std::mutex> lk(_lock);
    if(m_buf_len < len) {
        if(m_buf != nullptr) {
            delete[] m_buf;
        }
        m_buf = new uint8_t[len];
        m_buf_len = len;
    }
    memcpy(m_buf, data, len);
    auto image = QImage((const uchar *)m_buf, width, height, QImage::Format_RGB888);
    setImage(image);
}

void ImageProvider::setImage(QImage const &image)
{
    m_image = image;
    emit imageChanged();
}

QImage ImageProvider::image() const
{
    return m_image;
}
