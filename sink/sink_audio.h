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

    static constexpr const char* CMD = "appsrc name=source_to_out ! audio/x-raw,rate=16000,format=S16LE,channels=1,layout=interleaved ! audioconvert ! volume volume=1.3 ! audioresample ! %s";
    static constexpr const char* CMD_DESKTOPS = "autoaudiosink";
    static constexpr const char* CMD_ANDROID = "openslessink";

    static constexpr auto TAG = "SinkAudio: ";
};

#endif
