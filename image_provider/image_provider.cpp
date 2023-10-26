#include "image_provider.h"

ImageProvider::ImageProvider(QObject *parent)
    : QObject(parent)
{}

ImageProvider::~ImageProvider() {
    if(m_buf != nullptr) {
        delete[] m_buf;
    }
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
