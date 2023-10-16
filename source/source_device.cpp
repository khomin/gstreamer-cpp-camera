#include "source_device.h"
#include <iostream>

GstFlowReturn on_sample (GstElement * elt, SourceDevice* data);

SourceDevice::SourceDevice() : SourceDevice(SourceDeviceType::Screen) {}

SourceDevice::SourceDevice(SourceDeviceType type) : m_type(type) {
    if(m_type == SourceDeviceType::Screen) {
         m_pipe = gst_parse_launch(cmd_screen, NULL);
    } else if(m_type == SourceDeviceType::Webc) {
        m_pipe = gst_parse_launch(cmd_webc, NULL);
    }
    if (m_pipe == NULL) {
        std::cout << "not all elements created" << std::endl;
    }
    /* we use appsink in push mode, it sends us a signal when data is available
    * and we pull out the data in the signal callback. We want the appsink to
    * push as fast as it can, hence the sync=false */
    auto sink_raw_image = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_raw_image");
    g_object_set (G_OBJECT (sink_raw_image), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect (sink_raw_image, "new-sample", G_CALLBACK (on_sample), this);
    gst_object_unref (sink_raw_image);
}

void SourceDevice::start() {
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
}

void SourceDevice::pause() {}

void SourceDevice::stop() {}

/* called when the appsink notifies us that there is a new buffer ready for processing */
GstFlowReturn on_sample(GstElement * elt, SourceDevice* data) {
    GstSample *sample;
    GstBuffer *app_buffer, *buffer;
    GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

    /* get the sample from appsink */
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample == NULL) {
        return ret;
    }
    buffer = gst_sample_get_buffer(sample);

    if(buffer != NULL) {
        GstMapInfo mapInfo;
        gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

        for(auto & it : data->m_sinks) {
            it->putSample(sample);
        }
        gst_buffer_unmap(buffer, &mapInfo);
        gst_sample_unref(sample);
    } else {
        printf ("BUFFER IS NULL \n\n\n");
    }
    return ret; // return GstFlowReturn::GST_FLOW_OK;
}
