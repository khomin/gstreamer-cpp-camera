#ifndef SINK_ENCODE_H
#define SINK_ENCODE_H

#include "sink_base.h"
#include "utils/encoder_config.h"
#include <functional>

class SinkEncode : public SinkBase {
public:
    explicit SinkEncode(EncoderConfig config);
    SinkEncode() = delete;
    virtual ~SinkEncode();

    void start() override;
    void putSample(GstSample* sample) override;
    void setOnEncoded(std::function<void(uint8_t*, uint32_t, uint64_t, uint64_t)> cb);

private:
    EncoderConfig m_config;
    std::function<void(uint8_t*, uint32_t,uint32_t, uint32_t)> m_on_encoded = NULL;
protected:
    static constexpr auto tag = "SinkEncode";
    static constexpr const auto cmd = "appsrc name=source_to_out ! videoconvert ! videoscale ! videorate ! video/x-raw,format=%s,width=%d,height=%d,framerate=%d/1 ! %s bitrate=%d ! queue ! appsink name=sink_out max-buffers=1 drop=true";
};

#endif
