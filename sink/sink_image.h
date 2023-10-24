#ifndef SINK_IMAGE_H
#define SINK_IMAGE_H

#include "sink_base.h"
#include "image_provider/image_provider.h"

class SinkImage : public SinkBase {
public:
    enum class ImageType { Full, Preview};

    explicit SinkImage(ImageType type);
    SinkImage();
    ~SinkImage() override;

    void start() override;
    void putSample(GstSample* sample) override;
    void setImage(ImageProvider* imageProvider);

private:
    ImageProvider* m_image = NULL;
    ImageType m_type = ImageType::Full;
    std::mutex m_lock;

protected:
    static constexpr auto tag = "SinkImage";
    static constexpr auto cmd = "appsrc name=source_to_out is-live=true ! videoconvert %s ! queue ! appsink name=sink_out max-buffers=1 drop=true";
};

#endif
