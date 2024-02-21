#ifndef SINK_ENCODE_H
#define SINK_ENCODE_H

#include "sink_base.h"
#include "utils/encoder_config.h"
#include <functional>
#include <memory>

class SinkEncode : public SinkBase {
public:
    typedef std::function<void(uint8_t*,uint32_t,uint64_t,uint64_t)> OnEncoded;

    explicit SinkEncode(EncoderConfig config);
    SinkEncode() = delete;
    virtual ~SinkEncode();

    void start() override;
    void putSample(GstSample* sample) override;
    void putData(uint8_t* data, uint32_t len) override;
    void setOnEncoded(OnEncoded cb);
private:
    EncoderConfig m_config;
    OnEncoded m_on_encoded = nullptr;
    static GstFlowReturn on_sample(GstElement * elt, SinkEncode* cb);
protected:
    static constexpr auto TAG = "SinkEncode: ";
    static constexpr const auto CMD = "appsrc name=source_to_out ! videoconvert ! videoscale ! videorate ! video/x-raw,format=%s,width=%d,height=%d,framerate=%d/1 ! %s ! appsink name=sink_out max-buffers=1 drop=true";
};

#endif
