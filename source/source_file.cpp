#include "source_file.h"
#include "config.h"
#include <iostream>
#include <thread>

SourceFile::SourceFile(std::string path, Type type, bool loop) {
    GError *error = NULL;
    m_loop = loop;
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
        CMD,
        path.c_str()
    );
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
    if (!m_pipe) {
        std::cerr << TAG << "pipe failed" << std::endl;
    }
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_free (message);
    }
//    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
//    if(!sink_out) {
//        std::cout << TAG << ": sink is null" << std::endl;
//    }
//    g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", TRUE, NULL);
//    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SourceFile::on_sample), this);
//    gst_object_unref (sink_out);
    std::cout << TAG << ": created" << std::endl;
}

SourceFile::~SourceFile() {
    std::lock_guard<std::mutex> lk(m_lock);
//    if (m_running.load()) {
//        m_running.store(false);
        if (m_pipe) {
            gst_element_set_state(m_pipe, GST_STATE_NULL);
            gst_object_unref(GST_OBJECT(m_pipe));
            m_pipe = nullptr;
        }
//    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceFile::start() {
    if(m_pipe != NULL) {
        m_running.store(true);
//        auto bus = gst_pipeline_get_bus (GST_PIPELINE(m_pipe));
//        gst_bus_add_watch (bus, SourceFile::on_bus_cb, this);
        gst_element_set_state(m_pipe, GST_STATE_PLAYING);
//        gst_object_unref (bus);
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void SourceFile::pause() {
    gst_element_set_state(m_pipe, GST_STATE_PAUSED);
}

GstFlowReturn SourceFile::on_sample(GstElement * elt, SourceFile* data) {
    GstSample *sample;
    GstBuffer *buffer;
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));
    if(sample != nullptr) {
        buffer = gst_sample_get_buffer(sample);
        if (buffer != nullptr) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

            if (data != nullptr) {
                auto sinks = data->getSinks();
                for (auto it: sinks) {
                    if (it != nullptr) {
                        // if you need caps info
//                        GstCaps *caps = gst_sample_get_caps(sample);
//                        const GstStructure *capStr = gst_caps_get_structure(caps, 0);
//                        std::string capsStr2 = gst_structure_to_string(capStr);
//                        std::cout << TAG << ": caps: " << capsStr2.c_str() << std::endl;
                        it->putSample(sample);
                    }
                }
            }
            gst_buffer_unmap(buffer, &mapInfo);
        }
        gst_sample_unref(sample);
    }
    return GstFlowReturn::GST_FLOW_OK;
}

gboolean SourceFile::on_bus_cb (GstBus * bus, GstMessage * message, gpointer data) {
    g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
    switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
        GError *err;
        gchar *debug;
        gst_message_parse_error (message, &err, &debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
        g_free (debug);
        break;
   }
    case GST_MESSAGE_EOS: {
        auto player = (SourceFile*)data;
        if(player->m_running.load() && player->m_loop) {
            gst_element_seek_simple(player->m_pipe, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, 0);
        }
    }
        break;
    default:
        /* unhandled message */
        break;
    }
    return TRUE;
}
