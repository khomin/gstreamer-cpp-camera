#include "sink_file.h"
#include <iostream>

#include <gst/app/gstappsink.h>
#include <gst/app/app.h>

SinkFile::SinkFile(int width, int height, std::string format, std::string path) {
    m_file = std::ofstream(path.c_str(), std::ios::out | std::ios::binary);
    GstBus *bus;
    GSource *bus_source;
    auto appsrc = gst_element_factory_make("appsrc", "source_to_out");
    auto caps_filter_in = gst_element_factory_make("capsfilter", NULL);
//    auto videoconvert = gst_element_factory_make("videoconvert", NULL);
//    auto caps_filter_middle = gst_element_factory_make("capsfilter", NULL);
    auto videoconvert2 = gst_element_factory_make("videoconvert", NULL);
    auto x264enc = gst_element_factory_make("x264enc", "x264enc");
//    auto mp4mux = gst_element_factory_make("mp4mux", "mp4mux");
//    auto queue = gst_element_factory_make("queue", nullptr);
//    auto caps_filter_out = gst_element_factory_make("capsfilter", NULL);
    auto appsink = gst_element_factory_make("appsink", "sink_out");
//    auto appsink = gst_element_factory_make("fakesink", "sink_out");

    g_object_set(caps_filter_in, "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "width", G_TYPE_INT, width,
                                     "height", G_TYPE_INT, height,
                                     "framerate", GST_TYPE_FRACTION, 20, 1,
                                     "format", G_TYPE_STRING, format.c_str(),
                                     NULL), NULL);
//    g_object_set(caps_filter_middle, "caps",
//                 gst_caps_new_simple("video/x-I420",
//                                     "width", G_TYPE_INT, width,
//                                     "height", G_TYPE_INT, height,
//                                     "framerate", GST_TYPE_FRACTION, 20, 1,
//                                     "format", G_TYPE_STRING, "I420",
//                                     NULL), NULL);
//    g_object_set(caps_filter_out, "caps",
//                 gst_caps_new_simple("video/x-h264",
//                                     "width", G_TYPE_INT, width,
//                                     "height", G_TYPE_INT, height,
//                                     "framerate", GST_TYPE_FRACTION, 20, 1,
//                                     "format", G_TYPE_STRING, "BGR",
//                                     NULL), NULL);

    m_pipe = gst_pipeline_new("pipeline");
    gst_bin_add_many(GST_BIN (m_pipe),
                     appsrc,
//                     caps_filter_in,
//                     videoconvert,
//                     caps_filter_middle,
                     videoconvert2,
//                     queue,
                     x264enc,
//                     queue,
//                     mp4mux,
//                     caps_filter_out,
                     appsink,
                     NULL);

    if (gst_element_link_many(appsrc,
//                          caps_filter_in,
//                          videoconvert,
//                              caps_filter_middle,
                              videoconvert2,
//                              queue,
                              x264enc,
//                              queue,
//                              mp4mux,
//                          caps_filter_out,
                              appsink,
                              NULL) != TRUE) {
        g_printerr("Elements could not be linked.\n");
    }

    /* instruct the bus to emit signals for each received message, and connect to the interesting signals */
    bus = gst_element_get_bus(m_pipe);
    bus_source = gst_bus_create_watch(bus);
    g_source_set_callback(bus_source, (GSourceFunc) gst_bus_async_signal_func, NULL, NULL);
    g_source_unref(bus_source);
    g_signal_connect (G_OBJECT(bus), "message::error", G_CALLBACK(SinkBase::on_error), this);

    auto source = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    g_object_set(source,
                 "format", GST_FORMAT_TIME,
#ifdef GST_APP_LEAKY_TYPE_UPSTREAM
            "leaky-type", GST_APP_LEAKY_TYPE_UPSTREAM, // since 1.20
#endif
                 NULL);

    g_object_set(x264enc,
                 "speed-preset", 1,
                 NULL);

    auto sink_out = gst_bin_get_by_name(GST_BIN (m_pipe), "sink_out");
    g_object_set(sink_out,
                 "emit-signals", TRUE,
                 "sync", TRUE,
                 "max-buffers", 1,
                 "drop", true,
                 NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK(SinkFile::on_sample), &m_file);

    /* free resources */
    gst_object_unref(bus);
    gst_object_unref(source);
    gst_object_unref(sink_out);
    std::cout << TAG << ": created" << std::endl;
}

SinkFile::~SinkFile() {
    std::lock_guard<std::mutex> lock(m_lock);
    stopPipe();
    m_file.close();
    std::cout << TAG << ": destroyed" << std::endl;
}

void SinkFile::start() {
    std::lock_guard<std::mutex> lock(m_lock);
    startPipe();
}

void SinkFile::putSample(GstSample *sample) {
    std::lock_guard<std::mutex> lock(m_lock);
    auto source_to_out = gst_bin_get_by_name(GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample(GST_APP_SRC (source_to_out), sample);
    if (ret != GST_FLOW_OK && ret != GST_FLOW_EOS) {
        std::cout << "push_sample error: " << ret << std::endl;
    }
    gst_object_unref(source_to_out);
}

GstFlowReturn SinkFile::on_sample(GstElement *elt, std::ofstream *file) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample(GST_APP_SINK (elt));

    if (sample != NULL) {
        buffer = gst_sample_get_buffer(sample);
        if (buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);
            std::cout << "BTEST-file-sample: << " << std::endl;
            if (file != NULL) {
                file->write((char *) mapInfo.data, mapInfo.size);
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}
