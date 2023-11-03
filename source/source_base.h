#ifndef SOURCEBASE_H
#define SOURCEBASE_H

#include <vector>
#include <memory>
#include <mutex>
#include "sink_base.h"

class SourceBase
{
public:
    SourceBase();
    virtual ~SourceBase();
    void addSink(std::shared_ptr<SinkBase> sink);
    void removeSink(std::shared_ptr<SinkBase> sink);
    std::vector<std::shared_ptr<SinkBase>> getSinks();
    virtual void start() {}
    virtual void pause() {}
    bool getError();

protected:
    void startPipe();
    void pausePipe();
    void stopPipe();

    GstElement* m_pipe = NULL;
    std::mutex m_lock;
    bool m_error = false;
private:
    void cleanBusEvents();
    std::vector<std::shared_ptr<SinkBase>> m_sinks;
    bool m_is_running = false;
};

#endif // SOURCEBASE_H
