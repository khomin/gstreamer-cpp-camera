#ifndef IMG_RAW_PROVIDER_H
#define IMG_RAW_PROVIDER_H

#include "image_provider/image_provider_abstract.h"

class ImageRawProvider : public ImageProviderAbstract {
public:
    ImageRawProvider();
    ~ImageRawProvider() override;
    void setImage(int width, int height, uint8_t* data, uint32_t len) override;
    void start() override;
    uint8_t * getBuffer();
    uint64_t  getBufferLen();
    uint32_t getWidth();
    uint32_t getHeight();

private:
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

#endif // IMG_RAW_PROVIDER_H
