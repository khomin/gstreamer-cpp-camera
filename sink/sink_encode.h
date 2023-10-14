#ifndef SINK_ENCODE_H
#define SINK_ENCODE_H

#include "sink_base.h"

class SinkEncode : public SinkBase {
public:
    SinkEncode();
    ~SinkEncode();

    void start() override;
    void stop() override;
    void putSample(GstSample* sample) override;

private:
    GstElement* m_sink  = NULL;

protected:
    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! appsink name=sink_out";
};

#endif
