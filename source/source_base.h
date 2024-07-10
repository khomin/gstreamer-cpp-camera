#ifndef SOURCEBASE_H
#define SOURCEBASE_H

#include <vector>
#include <memory>
#include <mutex>
#include "sink/sink_base.h"

class SourceBase
{
public:
    SourceBase();
    virtual ~SourceBase();

    virtual void start() = 0;
    virtual void pause() = 0;

    void addSink(std::shared_ptr<SinkBase> sink);
    void removeSink(std::shared_ptr<SinkBase> sink);
    void removeSinkAll();
    std::vector<std::shared_ptr<SinkBase>> getSinks();

protected:
    GstElement* m_pipe = NULL;
    std::mutex m_lock;
private:
    std::vector<std::shared_ptr<SinkBase>> m_sinks;
};

#endif // SOURCEBASE_H
