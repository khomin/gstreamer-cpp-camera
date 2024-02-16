#ifndef SINKBASE_H
#define SINKBASE_H

#include <stdint.h>
#include <mutex>
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
    static void on_error (GstBus * bus, GstMessage * message, SinkBase * p);
    GstElement * m_pipe  = nullptr;
protected:
    void startPipe();
    void pausePipe();
    void stopPipe();

    uint64_t m_id = 0;
    uint64_t m_signal_id = 0;
    bool m_error = false;
    std::mutex m_lock;
private:
    void cleanBusEvents();
    bool m_is_running = false;
    uint64_t m_id_cnt = 0;
};

#endif // SINKBASE_H
