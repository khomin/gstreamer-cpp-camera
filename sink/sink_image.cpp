#include "sink_image.h"
#include "utils/measure.h"
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <vector>
#include <iostream>

SinkImage::SinkImage(std::string format, int width_in, int height_in, int width_out, int height_out, int framerate) {
    auto appsrc = gst_element_factory_make("appsrc", "source_to_out");
    auto capsFilterIn = gst_element_factory_make("capsfilter", NULL);
    auto queue = gst_element_factory_make("queue", NULL);
    auto videoconvert = gst_element_factory_make("videoconvert", NULL);
    auto videoscale = gst_element_factory_make("videoscale", NULL);
    auto capsFilterOut = gst_element_factory_make("capsfilter", NULL);
    auto appsink = gst_element_factory_make("appsink", "sink_out");

    auto caps_in = gst_caps_new_simple("video/x-raw",
                                       "width", G_TYPE_INT, width_in,
                                       "height", G_TYPE_INT, height_in,
                                       "framerate", GST_TYPE_FRACTION, framerate, 1,
                                       "format", G_TYPE_STRING, format.c_str(), NULL);
    auto caps_out = gst_caps_new_simple("video/x-raw",
                                        "width", G_TYPE_INT, width_out,
                                        "height", G_TYPE_INT, height_out,
                                        "framerate", GST_TYPE_FRACTION, framerate, 1,
                                        "framerate", GST_TYPE_FRACTION, framerate, 1,
                                        "format", G_TYPE_STRING, "RGBA",
                                        NULL);
    g_object_set(capsFilterIn, "caps", caps_in, NULL);
    g_object_set(capsFilterOut, "caps", caps_out, NULL);
    g_object_set(queue,"leaky",2, "max-size-buffers",1, NULL);

    m_pipe = gst_pipeline_new("pipeline");
    gst_bin_add_many(GST_BIN (m_pipe),
                     appsrc,
                     capsFilterIn,
                     queue,
                     videoconvert,
                     videoscale,
                     capsFilterOut,
                     appsink,
                     NULL);
    gst_element_link_many(appsrc, capsFilterIn, queue, videoconvert, videoscale, capsFilterOut, appsink, NULL);

    /* instruct the bus to emit signals for each received message, and connect to the interesting signals */
    auto bus = gst_element_get_bus(m_pipe);
    auto bus_source = gst_bus_create_watch(bus);
    g_source_set_callback(bus_source, (GSourceFunc) gst_bus_async_signal_func, NULL, NULL);
    g_source_unref(bus_source);
    g_signal_connect (G_OBJECT(bus), "message::error", G_CALLBACK(SinkBase::on_error), this);

    auto source = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");

    g_object_set(source,
                 "format", GST_FORMAT_TIME,
                 "do-timestamp", TRUE,
                 "is-live", TRUE,
                 NULL);
    g_object_set(sink_out,
                 "emit-signals", TRUE,
                 "max-buffers", 1,
                 "drop", true,
                 NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK(SinkImage::on_sample), &m_image);
    /* free resources */
    gst_object_unref(bus);
    gst_object_unref(source);
    gst_object_unref(sink_out);
    gst_caps_unref(caps_in);
    gst_caps_unref(caps_out);
    std::cout << TAG << ": created" << std::endl;
}

SinkImage::~SinkImage() {
    std::lock_guard<std::mutex> lock(m_lock);
    auto bus = gst_element_get_bus(m_pipe);
    m_image = nullptr;
    gst_object_unref(bus);
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkImage::start() {
    std::lock_guard<std::mutex> lock(m_lock);
    startPipe();
}

void SinkImage::putSample(GstSample *sample) {
    std::lock_guard<std::mutex> lk(m_lock);
    auto source_to_out = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample(GST_APP_SRC (source_to_out), sample);
    if (ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret << std::endl;
    }
    gst_object_unref(source_to_out);
}

void SinkImage::putData(uint8_t *data, uint32_t len) {
    std::lock_guard<std::mutex> lk(m_lock);
    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);
    auto source_to_out = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if (ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret << std::endl;
    }
    gst_object_unref(source_to_out);
}

void SinkImage::setImage(std::shared_ptr<ImageProviderAbstract> image) {
    std::lock_guard<std::mutex> lock(m_lock);
    m_image = image;
}

GstFlowReturn SinkImage::on_sample(GstElement *elt, std::shared_ptr<ImageProviderAbstract> image) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample(GST_APP_SINK (elt));

    if (sample != NULL) {
        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *capStr = gst_caps_get_structure(caps, 0);
        int imW = 0, imH = 0;
        gst_structure_get_int(capStr, "width", &imW);
        gst_structure_get_int(capStr, "height", &imH);
        buffer = gst_sample_get_buffer(sample);
        if (buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            Measure::instance()->onImageSampleReady();

            if (image != nullptr) {
                image->setImage(imW, imH, (uint8_t *) mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
