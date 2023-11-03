#include "source_base.h"
#include <iostream>
#include <algorithm>

SourceBase::SourceBase() {

}

SourceBase::~SourceBase() {
    if(m_pipe != nullptr) {
        gst_element_send_event(m_pipe, gst_event_new_eos());
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        cleanBusEvents();
        auto bus = gst_element_get_bus (m_pipe);
        //gst_bus_remove_watch(bus);
        gst_object_unref(m_pipe);
        gst_object_unref (bus);
        m_pipe = nullptr;
    }
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
//    std::lock_guard lk(m_lock);
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
    }
}

void SourceBase::cleanBusEvents() {
    //// https://stackoverflow.com/questions/76753673/gstreamer-memory-leak-when-switching-between-states#comment135328339_76757393
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipe));
    gst_bus_set_flushing(bus, TRUE);
    for (;;) {
        GstMessage *msg = gst_bus_timed_pop_filtered(bus, 1, GST_MESSAGE_STATE_CHANGED);
        if(msg == NULL) {
            break;
        }
        if (msg->src == GST_OBJECT(m_pipe)) {
            gst_message_unref(msg);
            break;
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
}
