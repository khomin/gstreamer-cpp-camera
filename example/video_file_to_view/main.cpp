#include "launch.h"
#include "sink/sink_image.h"
#include "sink/sink_callback.h"
#include "source/source_file.h"
#include <thread>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {
//        int width = 1920;
//        int height = 1200;

        auto samples = std::vector<std::string>{
            "/home/khomin/Desktop/test-images/big_buck_bunny.mp4",
            "/home/khomin/Desktop/test-images/sample-15s.mp4",
            "/home/khomin/Desktop/test-images/demo.mp4"
        };
        int samplesIndex = 0;

//        const char* CMD = "filesrc location=%s ! decodebin ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! fakesink sync=false";
//        const char* CMD = "filesrc location=%s ! decodebin ! queue ! videoconvert ! videoscale ! videorate  ! video/x-raw,format=RGBA,width=1280,height=720,framerate=30/1 ! videoconvert !  xvimagesink";
//        const char* CMD = "filesrc location=%s ! decodebin ! queue leaky=downstream max-size-buffers=100 ! videoconvert ! queue leaky=downstream max-size-buffers=100 ! xvimagesink sync=false";
//        const char* CMD = "filesrc location=%s ! decodebin ! videoconvert ! fakesink sync=true";
//        const char* CMD = "filesrc location=%s ! decodebin ! videoconvert ! fakesink";

        for(int i=0; i<100; i++) {
            if(samplesIndex >= samples.size()) {
                samplesIndex = 0;
            }
            GError *error = NULL;
//            auto cmdBuf = std::vector<uint8_t>(1024);
//            auto path = samples[samplesIndex++];
//            sprintf((char*)cmdBuf.data(), CMD, path.c_str());

            // create
//            auto m_pipe = gst_parse_launch("filesrc location=/home/khomin/Desktop/test-images/demo.mp4 ! decodebin max-size-time=0 max-size-buffers=0 ! videoconvert ! fakesink", &error);
            auto m_pipe = gst_parse_launch("filesrc location=/home/khomin/Desktop/test-images/demo.mp4 ! qtdemux ! h264parse ! avdec_h264 ! fakesink", &error);
//            auto m_pipe = gst_parse_launch((char*)cmdBuf.data(), &error);
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
            // start
            gst_element_set_state(m_pipe, GST_STATE_PLAYING);

            auto loop = g_main_loop_new(NULL, TRUE);

            std::thread tr([loop, m_pipe] {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                // Send EOS and wait for state change
                gst_element_send_event(m_pipe, gst_event_new_eos());
                g_main_loop_quit(loop);

            });
            tr.detach();

            g_main_loop_run(loop);
            g_main_loop_unref(loop);

//            // let it run
//            std::this_thread::sleep_for(std::chrono::milliseconds(500));

//            gst_debug_bin_to_dot_file(GST_BIN(m_pipe), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline.dot");

//            // Handle GstBus messages (prevents leaks)
//            GstBus *bus = gst_element_get_bus(m_pipe);
//            GstMessage *msg;
//            while ((msg = gst_bus_timed_pop_filtered(bus, 1000 * GST_MSECOND, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS))) != NULL) {
//                gst_message_unref(msg);
//            }
//            bus = gst_element_get_bus (m_pipe);
//            msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

            /* Free resources */
//            if (msg != NULL)
//                gst_message_unref (msg);
//            gst_object_unref(bus);

//            // Send EOS and wait for state change
//            gst_element_send_event(m_pipe, gst_event_new_eos());

            // Stop pipeline
            gst_element_set_state(m_pipe, GST_STATE_NULL);
            gst_object_unref(m_pipe);

//            GstStateChangeReturn ret = gst_element_set_state(m_pipe, GST_STATE_NULL);
//            if (ret != GST_STATE_CHANGE_SUCCESS) {
//                std::cerr << "Failed to stop pipeline properly!" << std::endl;
//            }

//            gst_object_unref(m_pipe);

//            // Send EOS and wait for state change
//            gst_element_send_event(m_pipe, gst_event_new_eos());
//            GstStateChangeReturn ret = gst_element_set_state(m_pipe, GST_STATE_NULL);
//            if (ret != GST_STATE_CHANGE_SUCCESS) {
//                std::cerr << "Failed to stop pipeline properly!" << std::endl;
//            }

//            // stop
//            gst_element_set_state(m_pipe, GST_STATE_NULL);
//            gst_object_unref(GST_OBJECT(m_pipe));
        }
        std::cout << "done" << std::endl;
//        gst_deinit();
        std::this_thread::sleep_for(std::chrono::seconds(30));
        std::cout << "done2" << std::endl;
    });
}


//            auto srcFile = std::make_shared<SourceFile>(
//                samples[samplesIndex++],
//                SourceFile::Type::video,
//                false
//            );
//            auto sinkCallback = std::make_shared<SinkCallback>();

//            sinkCallback->setDataCb([=](uint8_t *data, uint32_t len) {
//                launch->imageLeft->setImage(1280, 720, data, len);
//            });

//            srcFile->addSink(sinkCallback);
//            sinkCallback->start();
//            srcFile->start();

//            srcFile = nullptr;
//            sinkCallback = nullptr;
