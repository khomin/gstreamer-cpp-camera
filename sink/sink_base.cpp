#include "sink_base.h"
#include <iostream>

SinkBase::SinkBase() {
    m_id = m_id_cnt++;
}

SinkBase::~SinkBase() {
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
    }
    m_is_running = true;
}

void SinkBase::pausePipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_PAUSED);
        cleanBusEvents();
    }
    m_is_running = false;
}

void SinkBase::stopPipe() {
    if(m_pipe != nullptr) {
        gst_element_set_state(m_pipe, GST_STATE_NULL);
    }
    m_is_running = false;
}

void SinkBase::cleanBusEvents() {
    //// https://stackoverflow.com/questions/76753673/gstreamer-memory-leak-when-switching-between-states#comment135328339_76757393
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipe));
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

void SinkBase::on_error (GstBus * bus, GstMessage * message, SinkBase * p) {
    gchar *message_string;
    GError *err;
    gchar *debug_info;

    gst_message_parse_error (message, &err, &debug_info);
    message_string = g_strdup_printf ("Error received from element %s: %s",
                                      GST_OBJECT_NAME (message->src), err->message);
    // emit message here
    g_clear_error (&err);
    g_free (debug_info);
    g_free (message_string);
    gst_element_set_state (p->m_pipe, GST_STATE_NULL);
}
