#ifndef IMG_RAW_PROVIDER_H
#define IMG_RAW_PROVIDER_H

#include "image_provider/image_provider_abstract.h"

class ImageRawProvider : public ImageProviderAbstract {
public:
    ImageRawProvider(uint32_t id, int width, int height);
    ~ImageRawProvider() override;
    void setFrame(uint8_t* data, uint32_t len) override;
    void start() override;
    uint8_t * getBuffer();
    uint64_t  getBufferLen();
    uint32_t getWidth();
    uint32_t getHeight();
    uint32_t getId();

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t _id = 0;
};

#endif // IMG_RAW_PROVIDER_H
