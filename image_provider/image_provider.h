#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QImage>
#include <mutex>

class ImageProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage image MEMBER m_image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit ImageProvider(QObject *parent = nullptr);

    void setImage(int width, int height, uint8_t* data, uint32_t len) {
        std::lock_guard<std::mutex> lk(_lock);
        auto buf = new uint8_t[len];
        memcpy(buf, data, len);
        auto image = QImage((const uchar *)buf, width, height, QImage::Format_RGB888);
        setImage(image);
        if(m_buf != nullptr) {
            delete[] m_buf;
        }
        m_buf = buf;
    }

    void setImage(QImage const &image);
    QImage image() const;

signals:
    void imageChanged();

private:
    QImage m_image;
    uint8_t* m_buf = NULL;
    std::mutex _lock;
};

#endif // IMAGEPROVIDER_H
