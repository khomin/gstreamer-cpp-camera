#include "sink_image.h"
#include "utils/stringf.h"
#include "utils/measure.h"

#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>

GstFlowReturn on_sample(GstElement * elt, ImageProvider* data);

SinkImage::SinkImage(ImageType type) : m_type(type) {
    std::string cmdf;
    if(m_type == ImageType::Full) {
        cmdf = StringFormatter::format(cmd, "");
    } else if(m_type == ImageType::Preview) {
        cmdf = StringFormatter::format(cmd,
            StringFormatter::format("! videoscale ! video/x-raw,width=%d,height=%d", 380 * 2, 240 * 2).c_str());
    }
    m_pipe = gst_parse_launch(cmdf.c_str(), NULL);
    if (m_pipe == NULL) {
        std::cerr << "not all elements created" << std::endl;
    }
}

SinkImage::SinkImage() : SinkImage(ImageType::Preview) {}

SinkImage::~SinkImage() {}

void SinkImage::start() {
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    gst_object_unref (source);

    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), m_image);
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
}

void SinkImage::stop() {
    // TODO
}

void SinkImage::putSample(GstSample* sample) {
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source_to_out), sample);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);
}

void SinkImage::setImage(ImageProvider* image) {
    m_image = image;
}

GstFlowReturn on_sample(GstElement * elt, ImageProvider* image) {
    GstSample *sample;
    GstBuffer *app_buffer, *buffer;
    GstElement *source;
    GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

    /* get the sample from appsink */
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *s = gst_caps_get_structure(caps, 0);
        int imW, imH;
        gst_structure_get_int(s,"width", &imW);
        gst_structure_get_int(s, "height", &imH);
        buffer = gst_sample_get_buffer(sample);

        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            Measure::instance()->onImageSampleReady();

            if(image != NULL) {
                image->setImage(imW, imH, (uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
            gst_sample_unref(sample);
        } else {
            std::cerr << "buffer is null: " << ret  << std::endl;
        }
    }
    return ret;
}
