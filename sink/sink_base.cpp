#include "sink_base.h"
#include <iostream>
#include <random>

SinkBase::SinkBase() {
    m_id = random();
}

SinkBase::~SinkBase() {}

void SinkBase::putData(uint8_t* data, uint32_t len) {}

void SinkBase::putSample(GstSample* sample) {}

bool SinkBase::isRunning() {
    return m_is_running;
}

uint64_t SinkBase::getId() {
    return m_id;
}

bool SinkBase::getError() {
    return m_error;
}

void SinkBase::startPipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_PLAYING);
        cleanBusEvents();
        m_is_running = true;
    }
}

void SinkBase::pausePipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_PAUSED);
    }
}

void SinkBase::stopPipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        m_is_running = false;
    }
}

void SinkBase::cleanBusEvents() {
    //// https://stackoverflow.com/questions/76753673/gstreamer-memory-leak-when-switching-between-states#comment135328339_76757393
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipe));
    for (;;) {
        //std::cout << "SinkBase: polling bus events and cleaning them" << std::endl;
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