#ifndef IMG_VIDEOSINK
#define IMG_VIDEOSINK

#include "image_provider/image_provider_abstract.h"
#include <gst/gst.h>

class ImageVideoSink : public ImageProviderAbstract {
public:
    ImageVideoSink();
    ~ImageVideoSink() override;
    void setImage(int width, int height, uint8_t* data, uint32_t len) override;
    void start() override;
private:
    GstElement * m_pipe  = nullptr;
    static constexpr auto tag = "ImageVideoSink: ";
#if __APPLE__
    static constexpr auto cmd = "appsrc name=source_to_out ! video/x-raw,format=RGBA,width=1280,height=720,framerate=20/1 ! videoconvert ! queue ! osxvideosink";
#elif _WIN32
    static constexpr auto cmd = "appsrc name=source_to_out ! video/x-raw,format=RGBA,width=1280,height=720,framerate=20/1 ! videoconvert ! queue ! d3d11videosink";
#else
    static constexpr auto cmd = "appsrc name=source_to_out ! video/x-raw,format=RGBA,width=200,height=125,framerate=20/1 ! videoconvert ! queue ! xvimagesink";
#endif
};

#endif // IMG_VIDEOSINK
