#ifndef SINK_IMAGE_H
#define SINK_IMAGE_H

#include "sink_base.h"
#include "image_provider/image_provider_abstract.h"

class SinkImage : public SinkBase {
public:
    explicit SinkImage(std::string format, int width_in, int height_in, int width_out, int height_out, int framerate);
    SinkImage() = delete;
    ~SinkImage() override;

    void start() override;
    void putSample(GstSample* sample) override;
    void putData(uint8_t *data, uint32_t len) override;
    void setImage(ImageProviderAbstract* imageProvider);

private:
    static GstFlowReturn on_sample(GstElement * elt, SinkImage* image);
    ImageProviderAbstract* m_image = NULL;
protected:
    static constexpr auto TAG = "SinkImage: ";
};

#endif
