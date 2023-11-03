#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>
#include <thread>

static gboolean bus_call(GstBus *bus,
                         GstMessage *msg,
                         gpointer data) {
    GMainLoop *loop = (GMainLoop *) data;
    auto msgType = GST_MESSAGE_TYPE (msg);
    switch (msgType) {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            g_main_loop_quit(loop);
            break;

        case GST_MESSAGE_ERROR: {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);

            g_printerr("Error: %s\n", error->message);
            g_error_free(error);

            g_main_loop_quit(loop);
            break;
        }
        default:
            if (msgType != GST_MESSAGE_STATE_CHANGED && msgType != GST_MESSAGE_STREAM_START &&
                msgType != GST_MESSAGE_STREAM_STATUS && msgType != GST_MESSAGE_LATENCY) {
                std::cout << "EVENT" << msgType << std::endl;
            }
            break;
    }

    return TRUE;
}

/* This function is called when an error message is posted on the bus */
static void error_cb(GstBus *bus, GstMessage *msg, void *data) {
    GError *err;
    gchar *debug_info;

    /* Print error details on the screen */
    gst_message_parse_error(msg, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error(&err);
    g_free(debug_info);
}

int runLoop(int argc, char *argv[]) {
    gst_init(nullptr, nullptr);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    auto loop = g_main_loop_new(nullptr, FALSE);

    auto tr = std::thread([&] {
        g_main_loop_run(loop);
    });

    for (int i = 0; i < 1000; i++) {
        std::cout << "test " << i + 1 << " start" << std::endl;

//        auto pipe = gst_parse_launch("videotestsrc ! timeoverlay draw-shadow=false draw-outline=false deltay=50 font-desc=\"Sans, 30\" color=0xFFFFFFFF ! videoconvert ! videorate ! videoscale ! video/x-raw,format=RGB,framerate=30/1,width=2560,height=1600 ! appsink name=sink_out", NULL);
        auto pipe = gst_parse_launch(
                "videotestsrc ! videoconvert ! videorate ! videoscale ! video/x-raw,format=RGB,framerate=30/1,width=2560,height=1600 ! appsink name=sink_out",
                NULL);
        if (pipe == NULL) {
            std::cerr << "pipe failed" << std::endl;
        }
        /* we add a message handler */
        //        auto bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipe));
        //        // gst_bus_add_signal_watch (bus);
        //        auto bus_watch_id = gst_bus_add_watch (bus, bus_call, Measure::instance()->Loop);
        //        g_signal_connect (G_OBJECT (bus), "message::error", G_CALLBACK (error_cb), nullptr);
        //        g_signal_connect (G_OBJECT (bus), "message::eos", G_CALLBACK (error_cb), nullptr);
        //        gst_object_unref (bus);
        //
        gst_element_set_state(pipe, GST_STATE_PLAYING);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        gst_element_set_state(pipe, GST_STATE_NULL);
        //    g_signal_handlers_disconnect_by_func(bus, reinterpret_cast<gpointer>(SourceDevice::on_sample), this);
        //    gst_bus_disable_sync_message_emission(bus);
        //    gst_bus_remove_signal_watch(bus);

        std::cout << "test " << i + 1 << " end" << std::endl;
        if (i == 300) {
            std::cout << "test " << i + 1 << " end" << std::endl;
        }
    }
    g_main_loop_unref(loop);
    gst_deinit();
    tr.join();
    g_print("Going out\n");
    return 0;
}

int main(int argc, char *argv[]) {
    auto tr = std::thread([&] {
        runLoop(0, nullptr);
    });
    tr.join();
    return 0;
}
