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
    virtual bool isRunning();
    uint64_t getId();
    bool getError();

protected:
    GstElement * m_pipe  = nullptr;
    uint64_t m_id = 0;
    bool m_error = false;
    bool m_is_running = false;
};

#endif // SINKBASE_H
