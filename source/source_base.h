#ifndef SOURCEBASE_H
#define SOURCEBASE_H

#include <vector>
#include <memory>
#include "sink_base.h"

class SourceBase
{
public:
    SourceBase();

    void addSink(std::shared_ptr<SinkBase> sink);
    void removeSink(std::shared_ptr<SinkBase> sink);

    virtual void start() {}
    virtual void pause() {}
    virtual void stop() {}

    std::vector<std::shared_ptr<SinkBase>> m_sinks;
    GstElement* m_pipe = NULL;
};

#endif // SOURCEBASE_H
