#include "source_base.h"
#include <iostream>
#include <algorithm>

SourceBase::SourceBase() {}

SourceBase::~SourceBase() {}

void SourceBase::addSink(std::shared_ptr<SinkBase> sink) {
    std::lock_guard lk(m_lock);
    m_sinks.push_back(sink);
}

void SourceBase::removeSink(std::shared_ptr<SinkBase> sink) {
    std::lock_guard lk(m_lock);
    auto it = std::find_if(m_sinks.begin(), m_sinks.end(), [&](auto p) {
        return p == sink;
    });
    if(it != m_sinks.end()) {
        m_sinks.erase(it);
    }
}

void SourceBase::removeSinkAll() {
    std::lock_guard lk(m_lock);
    m_sinks.clear();

}

std::vector<std::shared_ptr<SinkBase>> SourceBase::getSinks() {
    return m_sinks;
}
