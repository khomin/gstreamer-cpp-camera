#include "play_wav.h"
#include "config.h"
#include <iostream>
#include <thread>

PlayWav::PlayWav(std::string path, bool loop) {
    GError *error = NULL;
    m_loop = loop;
    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
        CMD,
        path.c_str(),
        #if defined(__ANDROID__)
            "openslessink"
        #elif defined(__linux__)
            "autoaudiosink"
        #elif defined(__APPLE__)
        #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
            "osxaudiosink name=sink_out sync=FALSE"
        #elif TARGET_OS_MAC
            "osxaudiosink sync=FALSE"
        #else
        #error "Unknown Apple platform"
        #endif
        #elif defined(_WIN32) || defined(_WIN64)
            "autoaudiosink"
        #else
        #error "Unknown platform"
        #endif
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
    std::cout << TAG << ": created" << std::endl;
}

PlayWav::~PlayWav() {
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_running.load()) {
        m_running.store(false);
        if (m_pipe) {
//            auto audioSink = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
//            if (audioSink) {
//                auto *baseSink = GST_AUDIO_BASE_SINK_CAST(audioSink);
//                GstAudioRingBuffer *theBuffer = baseSink->ringbuffer;
//                GstAudioRingBufferClass *theClass = GST_AUDIO_RING_BUFFER_GET_CLASS(theBuffer);
//                theClass->pause = NULL;
//            }
            gst_element_set_state(m_pipe, GST_STATE_NULL);
            gst_object_unref(GST_OBJECT(m_pipe));
            m_pipe = nullptr;
        }
    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void PlayWav::start() {
    if(m_pipe != NULL) {
        m_running.store(true);
        auto bus = gst_pipeline_get_bus (GST_PIPELINE(m_pipe));
        gst_bus_add_watch (bus, PlayWav::on_bus_cb, this);
        gst_element_set_state(m_pipe, GST_STATE_PLAYING);
        gst_object_unref (bus);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void PlayWav::pause() {
    gst_element_set_state(m_pipe, GST_STATE_PAUSED);
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
        auto player = (PlayWav*)data;
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
