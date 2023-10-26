#ifndef IMG_RAW_PROVIDER_H
#define IMG_RAW_PROVIDER_H

#include "image_provider/image_provider_abstract.h"

class ImageRawProvider : public ImageProviderAbstract {
public:
    ImageRawProvider();
    ~ImageRawProvider() override;
    void setImage(int width, int height, uint8_t* data, uint32_t len) override;
};

#endif // IMG_RAW_PROVIDER_H
