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
    virtual ~ImageProvider();

    void setImage(int width, int height, uint8_t* data, uint32_t len) {
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

    void setImage(QImage const &image);
    QImage image() const;

signals:
    void imageChanged();

private:
    QImage m_image;
    uint8_t* m_buf = NULL;
    uint32_t  m_buf_len = 0;
    std::mutex _lock;
};

#endif // IMAGEPROVIDER_H
