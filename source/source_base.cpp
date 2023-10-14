#include "source_base.h"

SourceBase::SourceBase() {

}

void SourceBase::addSink(std::shared_ptr<SinkBase> sink) {
    m_sinks.push_back(sink);
}

void SourceBase::removeSink(std::shared_ptr<SinkBase> sink) {

}
