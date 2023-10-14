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
    void setImageProvider(ImageProvider* imageProvider);

private:
    ImageProvider* m_imageProvider = NULL;
    GstElement * m_sink_to_image  = NULL;
    ImageType m_type = ImageType::Full;

protected:
    const std::string cmd_full = "appsrc name=source_to_image ! videoconvert ! appsink name=sink_to_image";
    const std::string cmd_preview = "appsrc name=source_to_image ! videoconvert ! videoscale ! video/x-raw,width=426,height=240 ! appsink name=sink_to_image";
};

#endif
