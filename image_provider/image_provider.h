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
    explicit ImageProvider(QObject *parent = nullptr);
    virtual ~ImageProvider();

    void setImage(int width, int height, uint8_t* data, uint32_t len) override;
    void setImage(QImage const &image);
    QImage image() const;

    void start() override;
signals:
    void imageChanged();

private:
    QImage m_image;
};

#endif // IMAGE_PROVIDER_H
