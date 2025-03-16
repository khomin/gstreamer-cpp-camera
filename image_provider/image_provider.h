#ifndef IMAGE_PROVIDER_H
#define IMAGE_PROVIDER_H

#include <QObject>
#include <QImage>
#include <mutex>
#include "image_provider_abstract.h"

class ImageProvider : public QObject, public ImageProviderAbstract
{
    Q_OBJECT
    Q_PROPERTY(QImage image MEMBER m_image READ image WRITE setImage NOTIFY imageChanged)
public:
    explicit ImageProvider(uint32_t width, uint32_t height, QObject *parent = nullptr);
    virtual ~ImageProvider();

    void setFrame(uint8_t* data, uint32_t len) override;
    void setImage(QImage const &image);
    QImage image() const;

    void start() override;
signals:
    void imageChanged();

private:
    QImage m_image;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

#endif // IMAGE_PROVIDER_H
