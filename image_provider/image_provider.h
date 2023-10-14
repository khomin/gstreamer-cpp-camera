#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QImage>
#include <mutex>

class ImageProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage image MEMBER m_image READ image WRITE setImage NOTIFY imageChanged)

    QImage m_image;

public:
    explicit ImageProvider(QObject *parent = nullptr);
    void setImage(QImage const &image);
    QImage image() const;

    void setImage(int width, int height, uint8_t* data, uint32_t len) {
        auto image = QImage((const uchar *)data, width, height, QImage::Format_RGB888);
//        image.fill(Qt::yellow);
        {
            std::lock_guard<std::mutex> lk(_lock);
//            auto file = QFile("/Users/khominvladimir/Desktop/test.png");
//            file.open(QIODevice::ReadWrite);
//            auto buf2 = file.readAll();
//            image.loadFromData(buf2); //"/Users/khominvladimir/Desktop/test.png");
//            m_image = image;
//            _bArray = buf2;
//            _bArray = QByteArray((char*) data, len);
            setImage(image);
        }
//        emit onFrameChanged();
    }

signals:
    void imageChanged();

private:
    std::mutex _lock;
};

#endif // IMAGEPROVIDER_H
