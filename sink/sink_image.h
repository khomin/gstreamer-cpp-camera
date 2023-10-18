#ifndef SINK_IMAGE_H
#define SINK_IMAGE_H

#include "sink_base.h"
#include "image_provider/image_provider.h"

class SinkImage : public SinkBase {
public:
    enum class ImageType { Full, Preview};

    SinkImage(ImageType type);
    SinkImage();
    virtual ~SinkImage();

    void start() override;
    void stop() override;
    void putSample(GstSample* sample) override;
    void setImage(ImageProvider* imageProvider);

private:
    ImageProvider* m_image = NULL;
    GstElement * m_pipe  = NULL;
    ImageType m_type = ImageType::Full;

protected:
    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert %s ! queue ! appsink name=sink_out max-buffers=1 drop=true";
};

#endif
