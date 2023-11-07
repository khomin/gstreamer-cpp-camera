#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <mutex>

class ImageProviderAbstract {
public:
    ImageProviderAbstract() = default;
    virtual ~ImageProviderAbstract() = default;
    virtual void setImage(int width, int height, uint8_t* data, uint32_t len) = 0;
    virtual void start() = 0;
protected:
    uint8_t* m_buf = nullptr;
    uint32_t  m_buf_len = 0;
    std::mutex _lock;
};

#endif // IMAGEPROVIDER_H
