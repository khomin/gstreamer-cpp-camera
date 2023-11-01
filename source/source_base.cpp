#include "source_base.h"
#include <iostream>
#include <algorithm>

SourceBase::SourceBase() {

}

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

std::vector<std::shared_ptr<SinkBase>> SourceBase::getSinks() {
    std::lock_guard lk(m_lock);
    return m_sinks;
}

bool SourceBase::getError() {
    return m_error;
}

void SourceBase::startPipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_PLAYING);
        cleanBusEvents();
        m_is_running = true;
    }
}

void SourceBase::pausePipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_PAUSED);
        cleanBusEvents();
    }
}

void SourceBase::stopPipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        cleanBusEvents();
        m_is_running = false;
    }
}

void SourceBase::cleanBusEvents() {
    //// https://stackoverflow.com/questions/76753673/gstreamer-memory-leak-when-switching-between-states#comment135328339_76757393
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipe));
    for (;;) {
        std::cout << "SourceBase: polling bus events and cleaning them" << std::endl;
        GstMessage *msg = gst_bus_timed_pop_filtered(bus, 1, GST_MESSAGE_STATE_CHANGED);
        if (msg == NULL || msg->src == GST_OBJECT(m_pipe)) {
            gst_message_unref(msg);
            break;
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    std::cout << "SinkBase: cleaned event" << std::endl;
}
