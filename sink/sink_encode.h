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

private:
    GstElement* m_pipe  = NULL;
    std::function<void(uint8_t*, uint32_t)> m_on_encoded = NULL;

protected:
    // byte-stream=true
    // tune=zerolatency
    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! videoscale ! video/x-raw,format=I420,width=426,height=240,framerate=30/1 ! videoconvert ! queue ! vp8enc ! appsink name=sink_out";

//    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! video/x-raw,format=RGB ! videoconvert ! appsink name=sink_out";video/x-h264,stream-format=byte-stream,alignment=au,parsed=true

//    static constexpr auto cmd = "appsrc name=source_to_out ! video/x-h264,width=1920,height=1080,framerate=30/1 ! h264parse ! video/x-h264,stream-format=byte-stream ! videoconvert ! video/x-raw,width=2880,height=1800,framerate=30/1 ! videoconvert ! queue ! x264enc tune=zerolatency byte-stream=true ! appsink name=sink_out";
//    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! x264enc ! mp4mux ! appsink name=sink_out";
//    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink_out";
};

#endif
