#ifndef SINK_IMAGE_H
#define SINK_IMAGE_H

#include "sink_base.h"
#include "image_provider/image_provider.h"

class SinkImage : public SinkBase {
public:
    enum class ImageType { Full, Preview};

    SinkImage(ImageType type);
    SinkImage();

    void start() override;
    void stop() override;
    void putSample(GstSample* sample) override;
    void setImage(ImageProvider* imageProvider);

private:
    ImageProvider* m_image = NULL;
    GstElement * m_sink_to_image  = NULL;
    ImageType m_type = ImageType::Full;

protected:
    static constexpr auto cmd_full = "appsrc name=source_to_image ! videoconvert ! appsink name=sink_to_image";
    static constexpr auto cmd_preview = "appsrc name=source_to_image ! videoconvert ! videoscale ! video/x-raw,width=426,height=240 ! appsink name=sink_to_image";
};

#endif
