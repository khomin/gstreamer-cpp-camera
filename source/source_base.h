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

    void addSink(std::shared_ptr<SinkBase> sink);
    void removeSink(std::shared_ptr<SinkBase> sink);

    virtual void start() {}
    virtual void pause() {}
    bool getError();

    std::vector<std::shared_ptr<SinkBase>> sinks;

protected:
    GstElement* m_pipe = NULL;
    bool m_error = false;
    std::mutex m_lock;
};

#endif // SOURCEBASE_H
