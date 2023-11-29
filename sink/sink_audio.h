#ifndef SINK_AUDIO_H
#define SINK_AUDIO_H

#include "sink_base.h"
#include "image_provider/image_provider_abstract.h"

class SinkAudio : public SinkBase {
public:
    SinkAudio();
    ~SinkAudio() override;
    void start() override;
    void putSample(GstSample* sample) override;
    void putData(uint8_t* data, uint32_t len) override;

protected:
    static constexpr auto tag = "SinkAudio: ";
};

#endif
