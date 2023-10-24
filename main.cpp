#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>

#include <iostream>
#include <thread>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include <QStandardPaths>
#include <QDateTime>

#include "image_provider/live_image.h"
#include "image_provider/image_provider.h"
#include "source/source_device.h"
#include "sink/sink_image.h"
#include "sink/sink_file.h"
#include "sink/sink_encode.h"
#include "source/source_decode.h"
#include "utils/codec_type.h"

ImageProvider* image1 = NULL;
ImageProvider* image2 = NULL;

//#define USE_VIDEO_TO_IMAGE_PREVIEW
//#define USE_VIDEO_TO_ENCODE_FILE
#define USE_VIDEO_TO_ENCODE_CODEC
//#define USE_DECODE_FROM_FILE
//#define USE_CRASH_TEST

typedef struct CustomData {
    gboolean is_live;
    GstElement *pipeline;
    GMainLoop *loop;
    gint buffering_level;
} CustomData;

GstFlowReturn on_sample_(GstElement * elt, SourceDevice* data) {
    auto sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));
    auto buffer = gst_sample_get_buffer(sample);
    gst_sample_unref(sample);
    gst_buffer_unref(buffer);
    return GstFlowReturn::GST_FLOW_OK;
}

static void cb_message (GstBus *bus, GstMessage *msg, CustomData *data) {
    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug;

            gst_message_parse_error (msg, &err, &debug);
            g_print ("Error: %s\n", err->message);
            g_error_free (err);
            g_free (debug);

            gst_element_set_state (data->pipeline, GST_STATE_READY);
            g_main_loop_quit (data->loop);
            break;
        }
        case GST_MESSAGE_EOS:
            /* end-of-stream */
            gst_element_set_state (data->pipeline, GST_STATE_READY);
            g_main_loop_quit (data->loop);
            break;
        case GST_MESSAGE_BUFFERING:
            /* If the stream is live, we do not care about buffering. */
            if (data->is_live) break;

            gst_message_parse_buffering (msg, &data->buffering_level);

            /* Wait until buffering is complete before start/resume playing */
            if (data->buffering_level < 100)
                gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
            else
                gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
            break;
        case GST_MESSAGE_CLOCK_LOST:
            /* Get a new clock */
            gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
            gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
            break;
        default:
            /* Unhandled message */
            break;
    }
}

int runLoop (int argc, char *argv[]) {
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

#if defined(USE_VIDEO_TO_IMAGE_PREVIEW) || defined(USE_VIDEO_TO_ENCODE_FILE) || defined(USE_VIDEO_TO_ENCODE_CODEC)
    auto loop = g_main_loop_new(NULL, FALSE);
//    auto srcFromWebc = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Webc, SourceDevice::OptionType::TimeOverlay);
//    auto srcFromScreen = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::TimeOverlay);
//
//    auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 1280,720, 20, 900000));
//    auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 1280,7204, 20, 900000));
//
//    auto sinkToImgLeft = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
//    auto sinkToImgRight = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
//
//    auto sinkToFile = std::make_shared<SinkFile>((QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test_app.mp4").toLocal8Bit().data());

//    if(srcFromWebc->getError() || srcFromScreen->getError()
//            || sinkToEncode->getError() || srcDecode->getError()
//            || sinkToImgLeft->getError() || sinkToImgRight->getError()
//            || sinkToFile->getError()) {
//        std::cerr << "component failed" << std::endl;
//        return -1;
//    }
#endif

#ifdef USE_VIDEO_TO_IMAGE_PREVIEW
    srcFromScreen->addSink(sinkToImgLeft);
    sinkToImgLeft->setImage(image1);
    sinkToImgLeft->start();
    srcFromScreen->start();
    g_print ("Let's run!\n");
    g_main_loop_run (loop);
#endif

#ifdef USE_VIDEO_TO_ENCODE_FILE
    srcFromScreen->addSink(sinkToFile);
    srcFromScreen->start();
    sinkToFile->start();
    g_print ("Let's run!\n");
    g_main_loop_run (loop);
#endif

#ifdef USE_VIDEO_TO_ENCODE_CODEC
    auto srcFromScreen = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::TimeOverlay);
    auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 1280,720, 20, 900000));
    auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 1280,7204, 20, 900000));
    auto sinkToImgLeft = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    auto sinkToImgRight = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    srcFromScreen->addSink(sinkToEncode);
    srcFromScreen->addSink(sinkToImgLeft);
    srcDecode->addSink(sinkToImgRight);
    sinkToEncode->setOnEncoded([&](uint8_t* data, uint32_t len, uint32_t pts, uint32_t dts) {
        srcDecode->putDataToDecode(data, len);
    });
    sinkToImgLeft->setImage(image1);
    sinkToImgRight->setImage(image2);
    sinkToImgRight->start();
    sinkToImgLeft->start();
    sinkToEncode->start();

    // make it more realtime like, activate encode when everything is ready
    auto tr = std::thread([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        srcFromScreen->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        srcDecode->start();

        g_print ("Let's run!\n");
        g_main_loop_run (loop);
    });
    tr.join();
#endif

#ifdef USE_CRASH_TEST
//    auto loop = g_main_loop_new(NULL, FALSE);

//    auto pipe = gst_parse_launch("avfvideosrc ! videoconvert ! videorate ! video/x-raw,format=RGB,framerate=20/1 ! appsink name=sink_out drop=true",
//            //"filesrc location=/Users/khominvladimir/Desktop/raw.mp4 ! video/x-h264,format=I420,width=320,height=240,stream-format=byte-stream,framerate=30/1,alignment=au,bitrate=3000 ! h264parse ! avdec_h264 ! videoconvert ! appsink",
//                                 nullptr);

//    auto bus = gst_element_get_bus (pipe);
//    CustomData data;
//    data.pipeline = pipe;
//    data.loop = loop;

//    bin = gst_bin_get_by_name (GST_BIN (pipe), "sink");
//    if(bin != nullptr) {
//        gst_object_unref(bin);
//    }

//    g_signal_connect (bus, "message", G_CALLBACK (cb_message), &data);
//    g_signal_connect (pipe, "", G_CALLBACK (cb_message), &data);

//    auto trLoop = std::thread([&] {
//        g_main_loop_run (loop);
//    });

    for(int i=0; i<1000; i++) {
        std::cout << "test " << i+1 << " start" << std::endl;
        auto pipe = gst_parse_launch("avfvideosrc ! videoconvert ! videorate ! video/x-raw,format=RGB,framerate=20/1 ! appsink name=sink_out max-buffers=1 drop=true",
                //"filesrc location=/Users/khominvladimir/Desktop/raw.mp4 ! video/x-h264,format=I420,width=320,height=240,stream-format=byte-stream,framerate=30/1,alignment=au,bitrate=3000 ! h264parse ! avdec_h264 ! videoconvert ! appsink",
                                     nullptr);

        auto sink_out = gst_bin_get_by_name (GST_BIN (pipe), "sink_out");
//        g_object_set (G_OBJECT (sink_out), "emit-signals", TRUE, "sync", FALSE, NULL);
//        g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample_), NULL);
//        gst_object_unref (sink_out);

        gst_element_set_state (pipe, GST_STATE_PLAYING);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        auto state1 = gst_element_set_state (pipe,GST_STATE_PAUSED);
        auto state2 = gst_element_set_state (pipe, GST_STATE_READY);
        auto state3 = gst_element_set_state (pipe,GST_STATE_NULL);
//        auto bus = gst_element_get_bus (pipe);
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
//        gst_pipeline_set_auto_flush_bus(GST_PIPELINE(pipe), false);
//        auto isFlush = gst_pipeline_get_auto_flush_bus(GST_PIPELINE(pipe));
        gst_app_sink_get_wait_on_eos(GST_APP_SINK(sink_out));

        auto sample = gst_app_sink_pull_sample (GST_APP_SINK (sink_out));
        auto buffer = gst_sample_get_buffer(sample);
        gst_sample_unref(sample);
        gst_buffer_unref(buffer);

        gst_object_unref (sink_out);

        std::cout << "btest: state1: " << state1 <<  std::endl;
        std::cout << "btest: state2: " << state2 <<  std::endl;
        std::cout << "btest: state3: " << state3 <<  std::endl;
//        std::cout << "btest: isFlush: " << isFlush <<  std::endl;
        gst_object_unref(pipe);
    }
//    g_main_loop_quit(loop);
//    trLoop.join();
#endif

#ifdef USE_DECODE_FROM_FILE
    auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 2560 /4 ,1600 /4, 20, 400));
    auto sinkToImgLeft = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    auto sinkToImgRight = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    sinkToImgLeft->setImage(image1);
    sinkToImgRight->setImage(image2);
    srcDecode->addSink(sinkToImgRight);

    auto tr = std::thread([&] {
        QFile inputFile("/Users/khominvladimir/Desktop/raw.json");
        if (inputFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inputFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                auto raw = QByteArray::fromBase64(line.toUtf8());
                srcDecode->putDataToDecode((uint8_t*)raw.data(), raw.length());
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            inputFile.close();
        }
    });
    tr.detach();

    srcDecode->start();
    sinkToImgLeft->start();
    sinkToImgRight->start();

    g_print ("Let's run!\n");
    g_main_loop_run (loop);

    srcDecode = NULL;
    sinkToImgLeft = NULL;
    sinkToImgRight = NULL;
#endif
    g_print ("Going out\n");
    return 0;
}

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    image1 = new ImageProvider();
    image2 = new ImageProvider();

    qmlRegisterType<LiveImage>("ImageAdapter", 1, 0, "LiveImage");
    engine.rootContext()->setContextProperty("provider1", image1);
    engine.rootContext()->setContextProperty("provider2", image2);

    auto tr = std::thread([&] {
        runLoop(0, NULL);
        QCoreApplication::exit(-1);
    });
    tr.detach();

    engine.load(url);

    auto ret = app.exec();
    return ret;
}
