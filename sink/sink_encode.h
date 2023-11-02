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
    void setOnEncoded(std::shared_ptr<OnEncoded> cb);
private:
    EncoderConfig m_config;
    std::shared_ptr<OnEncoded> m_on_encoded = nullptr;
    static GstFlowReturn on_sample(GstElement * elt, std::shared_ptr<SinkEncode::OnEncoded> cb);
protected:
    static constexpr auto tag = "SinkEncode: ";
    static constexpr const auto cmd = "appsrc name=source_to_out ! videoconvert ! videoscale ! videorate ! video/x-raw,format={},width={},height={},framerate={}/1 ! {} ! queue ! appsink name=sink_out max-buffers=1 drop=true";
};

#endif
