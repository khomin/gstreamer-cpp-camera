#include "sink_image.h"
#include "utils/measure.h"
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include "config.h"
#include <vector>
#include <iostream>

SinkImage::SinkImage(int width, int height)  {
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    auto cmdSubBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdSubBuf.data(), "! videoscale ! video/x-raw,format=RGBA,width=%d,height=%d", width, height);
    sprintf((char*)cmdBuf.data(), CMD, cmdSubBuf.data());
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), NULL);
    if (m_pipe == NULL) {
        std::cerr << TAG << "pipe failed" << std::endl;
        m_error = true;
    }
    std::cout << TAG << ": created" << std::endl;
}

SinkImage::~SinkImage() {
    std::lock_guard<std::mutex> lock(m_lock);
    auto bus = gst_element_get_bus (m_pipe);
    g_signal_handlers_disconnect_by_func(bus, reinterpret_cast<gpointer>(SinkImage::on_sample), this);
    m_image = nullptr;
    gst_object_unref (bus);
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkImage::start() {
    std::lock_guard<std::mutex> lock(m_lock);
    // src
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    if(source == NULL) m_error = true;
    // sink
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
    m_signal_id = g_signal_connect (sink_out, "new-sample", G_CALLBACK (SinkImage::on_sample), this);
    if(source == NULL) m_error = true;
    gst_object_unref (sink_out);
    gst_object_unref (source);
    startPipe();
}

void SinkImage::putSample(GstSample* sample) {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source_to_out), sample);
    if(ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);
}

void SinkImage::setImage(ImageProviderAbstract* image) {
    std::lock_guard<std::mutex> lock(m_lock);
    m_image = image;
}

GstFlowReturn SinkImage::on_sample(GstElement * elt, SinkImage* data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *capStr = gst_caps_get_structure(caps, 0);
        int imW = 0, imH = 0;
        gst_structure_get_int(capStr,"width", &imW);
        gst_structure_get_int(capStr, "height", &imH);
        buffer = gst_sample_get_buffer(sample);
        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            Measure::instance()->onImageSampleReady();

            if(data != NULL && data->m_image != NULL) {
                data->m_image->setImage(imW, imH, (uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
