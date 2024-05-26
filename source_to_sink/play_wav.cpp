#include "play_wav.h"
#include "config.h"
#include <iostream>

PlayWav::PlayWav(std::string path, bool loop) {
    GError *error = NULL;
    m_loop = loop;
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
            CMD,
            path.c_str(),
#ifdef __ANDROID__
            CMD_SINK_ANDROID
#else
            CMD_SINK_DESKTOP
#endif
    );
    m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
    if (!m_pipe) {
        std::cerr << TAG << "pipe failed" << std::endl;
        m_error = true;
    }
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_free (message);
    }
    std::cout << TAG << ": created" << std::endl;
}

PlayWav::~PlayWav() {
    std::lock_guard<std::mutex> lk(m_lock);
    auto bus = gst_element_get_bus (m_pipe);
    gst_bus_remove_watch(bus);
    gst_object_unref (bus);
    std::cout << TAG << ": destroyed" << std::endl;
}

void PlayWav::start() {
    if(m_pipe != NULL) {
        auto bus = gst_pipeline_get_bus (GST_PIPELINE(m_pipe));
        gst_bus_add_watch (bus, PlayWav::on_bus_cb, this);
        stopPipe();
        startPipe();
        gst_object_unref (bus);
    }
}

void PlayWav::pause() {
    pausePipe();
}

gboolean PlayWav::on_bus_cb (GstBus * bus, GstMessage * message, gpointer data) {
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
        auto play = (PlayWav*)data;
        if(play->m_loop) {
            play->stopPipe();
            play->startPipe();
        }
    }
        break;
    default:
        /* unhandled message */
        break;
    }
    return TRUE;
}
