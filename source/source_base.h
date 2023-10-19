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
    bool getError();

    std::vector<std::shared_ptr<SinkBase>> sinks;

protected:
    GstElement* m_pipe = NULL;
    bool m_error = false;
};

#endif // SOURCEBASE_H
