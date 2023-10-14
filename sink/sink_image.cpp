#include "sink_image.h"
#include <iostream>

#include <gst/app/gstappsink.h>
#include <gst/app/app.h>

GstFlowReturn on_sample_mirror(GstElement * elt, ImageProvider* data);

SinkImage::SinkImage(ImageType type) : m_type(type) {
    if(m_type == ImageType::Full) {
         m_sink_to_image = gst_parse_launch(cmd_full.c_str(), NULL);
    } else if(m_type == ImageType::Preview) {
        m_sink_to_image = gst_parse_launch(cmd_preview.c_str(), NULL);
    }
    if (m_sink_to_image == NULL) {
        std::cout << "not all elements created" << std::endl;
    }
}

SinkImage::SinkImage() : SinkImage(ImageType::Preview) {}

void SinkImage::start() {
    // show mirror in canvas
    auto sink_to_image = gst_bin_get_by_name (GST_BIN (m_sink_to_image), "sink_to_image");
    g_object_set (G_OBJECT(sink_to_image), "emit-signals", TRUE, NULL);
    g_signal_connect (sink_to_image, "new-sample", G_CALLBACK (on_sample_mirror), m_imageProvider);
    gst_element_set_state (m_sink_to_image, GST_STATE_PLAYING);
}

void SinkImage::stop() {
    // TODO
}

void SinkImage::putSample(GstSample* sample) {
    auto source_to_image = gst_bin_get_by_name (GST_BIN (m_sink_to_image), "source_to_image");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source_to_image), sample);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_image);
}

void SinkImage::setImageProvider(ImageProvider* imageProvider) {
    m_imageProvider = imageProvider;
}

GstFlowReturn on_sample_mirror(GstElement * elt, ImageProvider* image) {
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

            if(image != NULL) {
                image->setImage(imW, imH, (uint8_t*)mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
            gst_sample_unref(sample);
        } else {
            printf ("BUFFER IS NULL \n\n\n");
        }
    }
    return ret;
}
