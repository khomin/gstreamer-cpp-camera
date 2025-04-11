#ifndef IMG_OPENGL_PROVIDER_H
#define IMG_OPENGL_PROVIDER_H

#include "image_provider/image_provider_abstract.h"
#include <functional>

class ImageOpenGlProvider : public ImageProviderAbstract {
public:
    ImageOpenGlProvider(uint32_t width, uint32_t height, uint64_t fl_texture_id,  uint64_t frame_buf_addr);
    ~ImageOpenGlProvider() override;
    void setFrame(uint8_t* data, uint32_t len) override;
    void start() override;
    uint8_t * getBuffer();
    uint64_t  getBufferLen();
    uint32_t getWidth();
    uint32_t getHeight();
    uint64_t getId();

    std::function<void()> onFrameChanged = NULL;

private:
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint64_t _frame_buf_addr = 0;
    uint64_t _fl_texture_id = 0;
};

#endif // IMG_OPENGL_PROVIDER_H
