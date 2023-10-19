#ifndef SINKBASE_H
#define SINKBASE_H

#include <stdint.h>
#include <gst/gst.h>

class SinkBase {
public:
    SinkBase();
    virtual ~SinkBase();
    virtual void start() {}
    virtual void putData(uint8_t* data, uint32_t len);
    virtual void putSample(GstSample* sample);
    uint64_t getId();
    bool getError();

protected:
    uint64_t m_id;
    bool m_error = false;
};

#endif // SINKBASE_H
