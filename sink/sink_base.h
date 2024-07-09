#ifndef SINKBASE_H
#define SINKBASE_H

#include <stdint.h>
#include <mutex>
#include <gst/gst.h>

class SinkBase {
public:
    SinkBase();
    virtual ~SinkBase();

    virtual void start() = 0;
    virtual void pause() = 0;

    virtual void putData(uint8_t* data, uint32_t len);
    virtual void putSample(GstSample* sample);

protected:
    GstElement * m_pipe  = nullptr;
    std::mutex m_lock;
};

#endif // SINKBASE_H
