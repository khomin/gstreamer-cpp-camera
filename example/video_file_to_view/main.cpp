#include "launch.h"
#include "sink/sink_image.h"
#include "sink/sink_callback.h"
#include "source/source_video_file.h"
#include <thread>

//gboolean on_bus_cb (GstBus * bus, GstMessage * message, gpointer data) {
//    g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
//    switch (GST_MESSAGE_TYPE (message)) {
//    case GST_MESSAGE_ERROR:{
//        GError *err;
//        gchar *debug;
//        gst_message_parse_error (message, &err, &debug);
//        g_print ("Error: %s\n", err->message);
//        g_error_free (err);
//        g_free (debug);
//        gst_message_unref(message);
//        break;
//    }
//    case GST_MESSAGE_EOS: {
//    }
//    break;
//    default:
//        /* unhandled message */
//        break;
//    }
//    gst_message_unref(message);
//    return TRUE;
//}

static gboolean
my_bus_callback (GstBus * bus, GstMessage * message, gpointer data)
{
    g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

    switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
        GError *err;
        gchar *debug;

        gst_message_parse_error (message, &err, &debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
        g_free (debug);

//        g_main_loop_quit (loop);
        break;
    }
    case GST_MESSAGE_EOS:
        /* end-of-stream */
//        g_main_loop_quit (loop);
        break;
    default:
        /* unhandled message */
        break;
    }

    /* we want to be notified again the next time there is a message
   * on the bus, so returning TRUE (FALSE means we want to stop watching
   * for messages on the bus and our callback should not be called again)
   */
    return TRUE;
}


int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {
        int width = 1920;
        int height = 1200;
//        launch->imageLeft = std::make_shared<ImageProvider>(1920, 1080);
//        launch->imageRight = std::make_shared<ImageProvider>(1920, 1080);

        auto samples = std::vector<std::string>{
            "/home/khomin/Desktop/test-images/demo.mp4"
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo038.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo055.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo029.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo031.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo023.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo024.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo020.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo006.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo007.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo016.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo035.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo004.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo050.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo043.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo034.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo032.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo021.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo010.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo012.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output021.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo011.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo000.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo018.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo037.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo025.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo002.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo041.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo044.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo033.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo008.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo053.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo028.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo003.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo049.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo027.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo048.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo046.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo030.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo022.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo019.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo001.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo052.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo014.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo013.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo047.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo015.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo005.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo054.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo039.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo009.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo051.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo017.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo042.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo040.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo026.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo036.mp4",
//                "/home/khomin/Desktop/test-images/bot/videos/output_allo045.mp4"
        };
        int samplesIndex = 0;
//        bool is_run = false;

//        auto loop = g_main_loop_new(NULL, TRUE);
//        std::thread tr([loop] {
//            g_main_loop_run(loop);
//            g_main_loop_unref(loop);
//        });
//        tr.detach();

        for(int i=0; i<30; i++) {
            if(samplesIndex >= samples.size()) {
                samplesIndex = 0;
            }
            GError *error = NULL;
            auto file_path = samples[samplesIndex++].c_str();

#define RAW 1

#ifdef RAW
//            static constexpr const char* CMD_WITH_AUDIO = "filesrc location=%s \
//                ! decodebin name=demux  \
//                demux. ! queue leaky=downstream max-size-buffers=1 ! videoconvert ! videoscale ! videorate  \
//                ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out sync=false drop=true \
//                demux. ! queue leaky=downstream max-size-buffers=1 ! audioconvert ! autoaudiosink sync=false";

//static constexpr const char* CMD_WITH_AUDIO = "filesrc location=%s \
//! decodebin name=demux demux. ! queue leaky=downstream max-size-buffers=1 max-size-time=10 ! videoconvert ! fakesink name=sink_out sync=false";

//static constexpr const char* CMD_WITH_AUDIO = "filesrc num-buffers=-1 location=%s \
//! decodebin name=demux  \
//demux. ! videoconvert ! videoscale ! videorate ! appsink name=sink_out sync=false drop=true \
//demux. ! audioconvert ! autoaudiosink sync=false";

//static constexpr const char* CMD_WITH_AUDIO = "filesrc location=%s ! decodebin ! audioconvert ! autoaudiosink";
static constexpr const char* CMD_WITH_AUDIO = "filesrc location=%s ! qtdemux name=demux \
                demux.audio_0 ! queue leaky=upstream max-size-buffers=50 ! avdec_aac ! audioconvert ! queue leaky=downstream max-size-buffers=50 ! autoaudiosink \
                demux.video_0 ! queue leaky=upstream max-size-buffers=50 ! avdec_h264 ! queue leaky=downstream max-size-buffers=500 ! videoconvert ! videoscale ! videorate ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! queue leaky=downstream max-size-buffers=50 ! appsink name=sink_out sync=false drop=true";

//static constexpr const char* CMD_WITH_AUDIO =
//"filesrc location=%s ! qtdemux name=demux \
//demux.audio_0 ! queue ! avdec_aac ! audioconvert ! audioresample ! autoaudiosink \
//demux.video_0 ! queue ! avdec_h264 ! videoconvert ! videoscale ! videorate ! video/x-raw,format=RGBA,width=%d,height=%d,framerate=%d/1 ! appsink name=sink_out sync=false drop=true";

            auto cmdBuf = std::vector<uint8_t>(1024);
            sprintf((char*)cmdBuf.data(),
                CMD_WITH_AUDIO,
                file_path
                    ,
                width, height,
                30
            );
            auto m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
            if (!m_pipe) {
                std::cerr << "pipe failed" << std::endl;
                return;
            }
            if (error) {
                gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
                g_clear_error (&error);
                g_free (message);
                gst_object_unref(m_pipe);
                return;
            }
//            GstBus *bus;
//            GstMessage *msg;
//            bus = gst_pipeline_get_bus(GST_PIPELINE (m_pipe));
//            auto id = gst_bus_add_watch (bus, my_bus_callback, NULL);

            // start
            gst_element_set_state(m_pipe, GST_STATE_PLAYING);

//            std::thread tr([loop, m_pipe] {
//                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                // Send EOS and wait for state change
//                gst_element_send_event(m_pipe, gst_event_new_eos());
//                g_main_loop_quit(loop);

//            });
//            tr.detach();

//            g_main_loop_run(loop);
//            g_main_loop_unref(loop);

//            // Send EOS and wait for state change
//            gst_element_send_event(m_pipe, gst_event_new_eos());

            // Handle GstBus messages (prevents leaks)
//            GstBus *bus = gst_element_get_bus(m_pipe);
//            GstMessage *msg;
////            while ((msg = gst_bus_timed_pop_filtered(bus, 20 * GST_MSECOND, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS))) != NULL) {
//            while ((msg = gst_bus_timed_pop_filtered(bus, 20 * GST_MSECOND, (GstMessageType)(GST_MESSAGE_ANY))) != NULL) {
//                gst_message_unref(msg);
//            }

//            auto loop = g_main_loop_new(NULL, TRUE);
//            std::thread tr1([loop] {
//                g_main_loop_run(loop);
//                g_main_loop_unref(loop);
//            });
//            tr1.detach();

//            msg = gst_bus_poll (bus, (GstMessageType) (GST_MESSAGE_EOS | GST_MESSAGE_ERROR), (GstClockTime) 1000000);
//            if(msg != NULL) {
//            switch (GST_MESSAGE_TYPE (msg)) {
//            case GST_MESSAGE_EOS: {
//                g_print ("EOS\n");
//                break;
//            }
//            case GST_MESSAGE_ERROR: {
//                GError *err = NULL; /* error to show to users                 */
//                gchar *dbg = NULL;  /* additional debug string for developers */

//                gst_message_parse_error (msg, &err, &dbg);
//                if (err) {
//                    g_printerr ("ERROR: %s\n", err->message);
//                    g_error_free (err);
//                }
//                if (dbg) {
//                    g_printerr ("[Debug details: %s]\n", dbg);
//                    g_free (dbg);
//                }
//            }
//            default:
//                g_printerr ("Unexpected message of type %d", GST_MESSAGE_TYPE (msg));
//                break;
//            }
//            gst_message_unref (msg);
//            }

//            auto loop = g_main_loop_new(NULL, TRUE);

//            std::thread tr2([m_pipe] {
//                std::this_thread::sleep_for(std::chrono::milliseconds(500));

            GstBus *bus;
            GstMessage *msg;
            bus = gst_pipeline_get_bus(GST_PIPELINE (m_pipe));
            auto bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);

//            while ((msg = gst_bus_timed_pop_filtered(bus, 200 * GST_MSECOND, (GstMessageType)(GST_MESSAGE_ANY))) != NULL) {
//                gst_message_unref(msg);
//            }
        auto loop = g_main_loop_new(NULL, TRUE);
//        std::thread tr([loop] {

//        auto loop = g_main_loop_new(NULL, TRUE);
        std::thread tr([m_pipe, loop] {
//            g_main_loop_run(loop);
//            g_main_loop_unref(loop);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            gst_element_send_event(m_pipe, gst_event_new_eos());
            g_main_loop_quit(loop);
        });
        tr.detach();

            g_main_loop_run(loop);
            g_main_loop_unref(loop);

//            std::this_thread::sleep_for(std::chrono::milliseconds(500));

//            gst_element_send_event(m_pipe, gst_event_new_eos());
                // Create an EOS event
//                GstEvent *eos_event = gst_event_new_eos();

                // Send the EOS event to the pipeline
                //                g_signal_emit_by_name(m_pipe, "send-event", eos_event);
//                g_main_loop_quit(loop);
                //                // Send EOS and wait for state change
//                                gst_element_send_event(m_pipe, gst_event_new_eos());
//                gst_element_send_event(m_pipe, eos_event);
//                gst_event_unref(eos_event);
//            });
//            tr2.detach();

//            std::this_thread::sleep_for(std::chrono::milliseconds(500));

//            g_main_loop_run(loop);
//            g_main_loop_unref(loop);

            auto ret1 = gst_element_set_state(m_pipe, GST_STATE_NULL);

//            GstState state;
//            gst_element_get_state(m_pipe, &state);
//            GstState state1;
//            GstState state2;
            GstStateChangeReturn ret2 = gst_element_get_state(m_pipe, NULL, NULL, GST_CLOCK_TIME_NONE);
            while ((msg = gst_bus_timed_pop_filtered(bus, 20 * GST_MSECOND, (GstMessageType)(GST_MESSAGE_ANY))) != NULL) {
                gst_message_unref(msg);
            }

//            std::cout << "pipe done: ret1=" << ret1 << ", ret2=" << ret2 << ": state1=" << state1 << ": state2=" << state2 << std::endl;
            gst_bus_remove_watch(bus);
            g_source_remove (bus_watch_id);
            gst_object_unref(bus);
            gst_object_unref(m_pipe);
#else
            auto loop = g_main_loop_new(NULL, TRUE);

            auto srcFile = std::make_shared<SourceVideoFile>(
                file_path,
                width, height,
                30,
                0.0,
                false
            );
            auto sinkCallback = std::make_shared<SinkCallback>();

            sinkCallback->onData([=](uint8_t *data, uint32_t len) {
                launch->imageLeft->setFrame(data, len);
            });

            srcFile->addSink(sinkCallback);
            sinkCallback->start();
            srcFile->start();
            // loop
            //            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            //            std::thread tr([m_pipe] {
//            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            //                // Send EOS and wait for state change
//            gst_element_send_event(m_pipe, gst_event_new_eos());
            // reset
//            srcFile.reset();
//            sinkCallback.reset();

//            auto loop = g_main_loop_new(NULL, TRUE);
            //        std::thread tr([loop] {

            //        auto loop = g_main_loop_new(NULL, TRUE);
            std::thread tr([loop] {
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                g_main_loop_quit(loop);
            });
            tr.detach();

            g_main_loop_run(loop);
            g_main_loop_unref(loop);

            srcFile.reset();
            sinkCallback.reset();

//            g_main_loop_quit(loop);
            //            });
            //            tr.detach();

            //            g_main_loop_run(loop);
//            g_main_loop_unref(loop);
            // let it run
//            std::this_thread::sleep_for(std::chrono::milliseconds(200));

#endif

            std::cout << "round " << i << std::endl;
        }
//        g_main_loop_quit(loop);
//        g_main_loop_unref(loop);
//        g_main_loop_quit(loop);

        std::cout << "done" << std::endl;

//        launch->imageLeft = nullptr;
//        launch->imageRight = nullptr;
        std::this_thread::sleep_for(std::chrono::seconds(30));
        std::cout << "done2" << std::endl;
    });
}
