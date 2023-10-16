#ifndef SINK_ENCODE_H
#define SINK_ENCODE_H

#include "sink_base.h"
#include <functional>

class SinkEncode : public SinkBase {
public:
    SinkEncode();
    ~SinkEncode();

    void start() override;
    void stop() override;
    void putSample(GstSample* sample) override;

    void setOnEncoded(std::function<void(uint8_t*, uint32_t)> cb);
    void setOnEncodedSample(std::function<void(GstSample*)> cb);

    void putEncoded(uint8_t* data, uint32_t len);
    void putEncodedSample(GstSample* sample);

private:
    GstElement* m_pipe  = NULL;
    std::function<void(uint8_t*, uint32_t)> m_on_encoded = NULL;
    std::function<void(GstSample*)> m_on_encoded_sample_cb = NULL;

protected:
    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! video/x-raw,format=RGB ! videoconvert ! x264enc ! appsink name=sink_out";
//    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! x264enc ! mp4mux ! appsink name=sink_out";
//    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink_out";
};

#endif
