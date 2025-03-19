#include "source_video_file.h"
#include <gst/pbutils/pbutils.h>
#include "config.h"
#include <iostream>
#include <thread>

SourceVideoFile::SourceVideoFile(std::string path,
                                 int width, int height,
                                 int framerate,
                                 float volume,
                                 bool loop) {
    GError *error = NULL;
    m_loop = loop;
    m_pipe = NULL;
    m_running = false;

    GError *err = NULL;
    GstDiscoverer *discoverer = gst_discoverer_new(5 * GST_SECOND, &err);
    if (!discoverer) {
        g_printerr("Failed to create discoverer: %s\n", err->message);
        g_clear_error(&err);
        return;
    }
    GstDiscovererInfo *info = gst_discoverer_discover_uri(discoverer, ("file:///" + path).c_str(), &err);
    if (!info) {
        g_printerr("Failed to get media info: %s\n", err->message);
        g_clear_error(&err);
        g_object_unref(discoverer);
        return;
    }
    bool has_audio = false;
    bool has_video = false;
    // audio streams
    auto streams = gst_discoverer_info_get_audio_streams(info);
    for (const GList *l = streams; l != NULL; l = l->next) {
        has_audio = true;
    }
    // video streams
    streams = gst_discoverer_info_get_video_streams(info);
    for (const GList *l = streams; l != NULL; l = l->next) {
        has_video = true;
    }

    if(!has_audio && !has_video) {
        std::cerr << TAG << "no streams" << std::endl;
        return;
    }

    auto cmdBuf = std::vector<uint8_t>(Config::CMD_BUFFER_LEN);
    sprintf((char*)cmdBuf.data(),
        has_audio ? CMD_WITH_AUDIO : CMD_NO_AUDIO,
        path.c_str(),
        width, height,
        framerate
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
    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    if(!sink_out) {
        std::cout << TAG << ": sink is null" << std::endl;
    }
    g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect (sink_out, "new-sample", G_CALLBACK (SourceVideoFile::on_sample), this);

    auto bus = gst_pipeline_get_bus (GST_PIPELINE(m_pipe));
    gst_bus_add_watch (bus, SourceVideoFile::on_bus_cb, this);

    if(volume <= 0) {
        setVolume(volume);
    }
    // Cleanup
    gst_discoverer_info_unref(info);
    g_object_unref(discoverer);
    gst_object_unref (bus);
    gst_object_unref (sink_out);
    std::cout << TAG << ": created" << std::endl;
}

SourceVideoFile::~SourceVideoFile() {
    std::lock_guard<std::mutex> lk(m_lock);
    if (m_pipe) {
        gst_element_send_event(m_pipe, gst_event_new_eos());
        gst_element_set_state(m_pipe, GST_STATE_NULL);
        auto bus = gst_pipeline_get_bus (GST_PIPELINE(m_pipe));
        gst_bus_remove_watch(bus);
        gst_object_unref (bus);
        gst_object_unref(GST_OBJECT(m_pipe));
        auto count = GST_OBJECT_REFCOUNT_VALUE(m_pipe);
        std::cout << TAG << ": GST_OBJECT_REFCOUNT: " << count << std::endl;
        m_pipe = nullptr;
    }
    std::cout << TAG << ": destroyed" << std::endl;
}

void SourceVideoFile::start(uint64_t position) {
    if(!m_running) {
        m_running.store(true);
        //
        gst_element_set_state(m_pipe, GST_STATE_PLAYING);
        if(position > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            gst_element_seek(
                m_pipe,
                1.0, // Rate (1.0 for normal speed)
                GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                GST_SEEK_TYPE_SET, position * GST_SECOND, // Seek position in nanoseconds
                GST_SEEK_TYPE_NONE, // Stop type (not needed here)
                GST_CLOCK_TIME_NONE // Stop position (not needed here)
            );
        }
    }
}

void SourceVideoFile::pause() {
    if(m_running) {
        gst_element_set_state(m_pipe, GST_STATE_PAUSED);
        m_running.store(false);
    }
}

void SourceVideoFile::setVolume(float value) {
    GstElement *volume_element = gst_bin_get_by_name(GST_BIN(m_pipe), "volume_control");
    if (volume_element) {
        g_object_set(volume_element, "volume", value, NULL);
        gst_object_unref(volume_element);
    }
}

GstFlowReturn SourceVideoFile::on_sample(GstElement * elt, SourceVideoFile* data) {
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
#ifdef PRINT_CAPS
                        GstCaps *caps = gst_sample_get_caps(sample);
                        const GstStructure *capStr = gst_caps_get_structure(caps, 0);
                        std::string capsStr2 = gst_structure_to_string(capStr);
                        std::cout << TAG << ": caps: " << capsStr2.c_str() << std::endl;
                        gst_caps_unref(caps);
#endif
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

gboolean SourceVideoFile::on_bus_cb (GstBus * bus, GstMessage * message, gpointer data) {
    g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
    switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
        GError *err;
        gchar *debug;
        gst_message_parse_error (message, &err, &debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
        g_free (debug);
        gst_message_unref(message);
        break;
   }
    case GST_MESSAGE_EOS: {
        auto player = (SourceVideoFile*)data;
        if(player->m_running.load() && player->m_loop) {
            gst_element_seek_simple(player->m_pipe, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, 0);
        }
        gst_message_unref(message);
    }
        break;
    default:
        /* unhandled message */
        break;
    }
    return TRUE;
}

bool SourceVideoFile::seekTo(uint64_t sec) {
    gint64 position = sec * GST_SECOND;  // to nanoseconds
    gboolean success = gst_element_seek(
        m_pipe,
        1.0, // Rate (1.0 for normal speed)
        GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
        GST_SEEK_TYPE_SET, position, // Seek position in nanoseconds
        GST_SEEK_TYPE_NONE, // Stop type (not needed here)
        GST_CLOCK_TIME_NONE // Stop position (not needed here)
    );
    if(success) {
        GstState currentState, pendingState;
        gst_element_get_state(m_pipe, &currentState, &pendingState, GST_CLOCK_TIME_NONE);
        if (currentState == GST_STATE_PAUSED) {
            // If paused, set to PLAYING to play one frame
            gst_element_set_state(m_pipe, GST_STATE_PLAYING);
            g_usleep(500000); // Allow time for one frame to be processed and rendered
            gst_element_set_state(m_pipe, GST_STATE_PAUSED);
        } else {
            gst_element_set_state(m_pipe, GST_STATE_PLAYING);
        }
    }
    return success;
}

uint64_t SourceVideoFile::getPlaybackPosition() {
    gint64 position = -1;
    if (gst_element_query_position(m_pipe, GST_FORMAT_TIME, &position)) {
        uint64_t sec = int(position / GST_SECOND);
        return sec;
    } else {
        std::cerr << "Failed to query position" << std::endl;
    }
    return 0;
}
