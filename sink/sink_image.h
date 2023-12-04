#ifndef SINK_IMAGE_H
#define SINK_IMAGE_H

#include "sink_base.h"
#include "image_provider/image_provider_abstract.h"

class SinkImage : public SinkBase {
public:
    explicit SinkImage(int width, int height);
    SinkImage();
    ~SinkImage() override;

    void start() override;
    void putSample(GstSample* sample) override;
    void setImage(ImageProviderAbstract* imageProvider);

private:
    static GstFlowReturn on_sample(GstElement * elt, SinkImage* image);
    ImageProviderAbstract* m_image = NULL;
protected:
    static constexpr auto tag = "SinkImage: ";
    static constexpr auto cmd = "appsrc name=source_to_out is-live=true ! videoconvert {} ! queue ! appsink name=sink_out max-buffers=1 drop=true";
};

#endif
