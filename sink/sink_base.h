#ifndef SINKBASE_H
#define SINKBASE_H

#include <stdint.h>
#include <gst/gst.h>

class SinkBase {
public:
    SinkBase();
    virtual ~SinkBase();
    virtual void start() {}
    virtual void stop() {}
    virtual void putData(uint8_t* data, uint32_t len);
    virtual void putSample(GstSample* sample);

    uint64_t id;
};

#endif // SINKBASE_H
