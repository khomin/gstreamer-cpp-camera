#include "sink_image.h"
#include "utils/stringf.h"
#include "utils/measure.h"

#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>

SinkImage::SinkImage(ImageType type) : m_type(type) {
    std::string cmdf;
    if(m_type == ImageType::Full) {
        cmdf = StringFormatter::format(cmd, "! video/x-raw,format=RGBA");
    } else if(m_type == ImageType::Preview) {
        cmdf = StringFormatter::format(cmd,
            StringFormatter::format("! videoscale ! video/x-raw,format=RGBA,width=%d,height=%d", 380, 240).c_str());
    }
    m_pipe = gst_parse_launch(cmdf.c_str(), NULL);
    if (m_pipe == NULL) {
        std::cerr << tag << "pipe failed" << std::endl;
        m_error = true;
    }
    std::cout << tag << ": created" << std::endl;
}

SinkImage::SinkImage() : SinkImage(ImageType::Preview) {}

SinkImage::~SinkImage() {
    if(m_pipe != NULL) {
        stopPipe();
        gst_object_unref(m_pipe);
    }
    std::cout << tag << ": destroyed" << std::endl;
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
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SinkImage::on_sample), m_image);
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

GstFlowReturn SinkImage::on_sample(GstElement * elt, ImageProviderAbstract* image) {
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

            if(image != NULL) {
                image->setImage(imW, imH, (uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
