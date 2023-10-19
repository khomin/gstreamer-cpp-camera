#ifndef SINK_ENCODE_H
#define SINK_ENCODE_H

#include "sink_base.h"
#include "utils/encoder_config.h"
#include <functional>

class SinkEncode : public SinkBase {
public:
    SinkEncode(EncoderConfig config);
    SinkEncode() = delete;
    ~SinkEncode();

    void start() override;
    void putSample(GstSample* sample) override;
    void setOnEncoded(std::function<void(uint8_t*, uint32_t, uint32_t, uint32_t)> cb);

private:
    GstElement* m_pipe  = NULL;
    EncoderConfig m_config;
    std::function<void(uint8_t*, uint32_t,uint32_t, uint32_t)> m_on_encoded = NULL;

protected:
    static constexpr auto tag = "SinkEncode";
    static constexpr const auto cmd = "appsrc name=source_to_out ! videoconvert ! videoscale ! videorate ! video/x-raw,format=%s,width=%d,height=%d,framerate=%d/1 ! %s bitrate=%d ! appsink name=sink_out";
};

#endif
