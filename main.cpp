#ifdef USE_QT
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDateTime>
#endif

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <gst/video/video.h>
#include <gst/gstplugin.h>
#include <iostream>
#include <thread>

#ifdef USE_QT
#include "image_provider/live_image.h"
#include "image_provider/image_provider.h"
#include "image_provider/image_videosink.h"
#else
#include "image_provider/image_videosink.h"
#endif
#include "source/source_device.h"
#include "sink/sink_image.h"
#include "sink/sink_file.h"
#include "sink/sink_encode.h"
#include "source/source_decode.h"
#include "utils/codec_type.h"
#include "source/source_audio.h"
#include "sink/sink_audio.h"
#include "sink_callback.h"
#include "utils/measure.h"

ImageProviderAbstract* image1 = NULL;
ImageProviderAbstract* image2 = NULL;

//#define USE_VIDEO_TO_IMAGE_PREVIEW
//#define USE_VIDEO_TO_ENCODE_FILE
//#define USE_VIDEO_TO_ENCODE_CODEC
#define USE_AUDIO_SRC_SINK
//#define USE_DECODE_FROM_FILE
//#define USE_CRASH_TEST
//#define USE_CRASH_TEST_2

#if __APPLE__
    extern "C" {
    GST_PLUGIN_STATIC_DECLARE(coreelements);
    GST_PLUGIN_STATIC_DECLARE(libav);
    GST_PLUGIN_STATIC_DECLARE(openh264);
    GST_PLUGIN_STATIC_DECLARE(app);
    GST_PLUGIN_STATIC_DECLARE(appsink);
    GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
    GST_PLUGIN_STATIC_DECLARE(x264);
    GST_PLUGIN_STATIC_DECLARE(isomp4);
    GST_PLUGIN_STATIC_DECLARE(applemedia);
    GST_PLUGIN_STATIC_DECLARE(videoconvertscale);
    GST_PLUGIN_STATIC_DECLARE(videorate);
    GST_PLUGIN_STATIC_DECLARE(pango);
    GST_PLUGIN_STATIC_DECLARE(videotestsrc);
    GST_PLUGIN_STATIC_DECLARE(osxvideo);
    GST_PLUGIN_STATIC_DECLARE(autodetect);
    GST_PLUGIN_STATIC_DECLARE(audiotestsrc);
    GST_PLUGIN_STATIC_DECLARE(audioconvert);
    GST_PLUGIN_STATIC_DECLARE(audioresample);
    GST_PLUGIN_STATIC_DECLARE(osxaudio);
    }
#endif

int runLoop (int argc, char *argv[]) {
#if defined(USE_VIDEO_TO_IMAGE_PREVIEW) || defined(USE_VIDEO_TO_ENCODE_FILE) || defined(USE_VIDEO_TO_ENCODE_CODEC) || defined(USE_AUDIO_SRC_SINK)
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    #if __APPLE__
        GST_PLUGIN_STATIC_REGISTER(coreelements);
        GST_PLUGIN_STATIC_REGISTER(libav);
        GST_PLUGIN_STATIC_REGISTER(app);
        GST_PLUGIN_STATIC_REGISTER(openh264);
        GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
        GST_PLUGIN_STATIC_REGISTER(x264);
        GST_PLUGIN_STATIC_REGISTER(isomp4);
        GST_PLUGIN_STATIC_REGISTER(applemedia);
        GST_PLUGIN_STATIC_REGISTER(videoconvertscale);
        GST_PLUGIN_STATIC_REGISTER(videorate);
        GST_PLUGIN_STATIC_REGISTER(pango);
        GST_PLUGIN_STATIC_REGISTER(videotestsrc);
        GST_PLUGIN_STATIC_REGISTER(osxvideo);
        GST_PLUGIN_STATIC_REGISTER(audiotestsrc);
        GST_PLUGIN_STATIC_REGISTER(autodetect);
        GST_PLUGIN_STATIC_REGISTER(audioconvert);
        GST_PLUGIN_STATIC_REGISTER(audioresample);
        GST_PLUGIN_STATIC_REGISTER(osxaudio);
    #endif
    auto loop = g_main_loop_new(NULL, FALSE);
    auto srcFromDevice = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::TimeOverlay);
    auto sinkToImgPrimary = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    auto sinkToImgSecond = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    #ifdef USE_QT
    auto sinkToFile = std::make_shared<SinkFile>((QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test_app.mp4").toLocal8Bit().data());
    #else
    auto videoSink = std::make_shared<ImageVideoSink>();
    #endif
#endif

#ifdef USE_VIDEO_TO_IMAGE_PREVIEW
    srcFromDevice->addSink(sinkToImgPrimary);
    sinkToImgPrimary->setImage(image1);
    sinkToImgPrimary->start();
    srcFromDevice->start();
    g_print ("Let's run!\n");
    g_main_loop_run (loop);
#endif

#ifdef USE_VIDEO_TO_ENCODE_FILE
    srcFromDevice->addSink(sinkToFile);
    srcFromDevice->start();
    sinkToFile->start();
    g_print ("Let's run!\n");
    g_main_loop_run (loop);
#endif

#ifdef USE_VIDEO_TO_ENCODE_CODEC
    srcFromDevice->addSink(sinkToImgSecond);
    sinkToImgSecond->setImage(image1);
    sinkToImgPrimary->setImage(image2);

    srcFromDevice->onConfig([=](uint32_t w, uint32_t h) {
        std::cout << "srcDeviceConfig " << "width: " << w << ", height: " << h << std::endl;
        auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 2560/2,1600 /2, 20, 900000 / 1000));
        srcFromDevice->addSink(sinkToEncode);
        auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 2560/2,1600 /2, 20, 900000 / 1000));
        sinkToEncode->setOnEncoded(SinkEncode::OnEncoded([=](uint8_t *data, uint32_t len, uint64_t pts, uint64_t dts) {
            srcDecode->putDataToDecode(data, len);
        }));
        srcDecode->addSink(sinkToImgPrimary);
        sinkToEncode->start();
        srcDecode->start();
    });
#ifndef USE_QT
    image1 = new ImageVideoSink();
    image2 = new ImageVideoSink();
#endif
    sinkToImgSecond->setImage(image1);
    sinkToImgPrimary->setImage(image2);
    sinkToImgSecond->start();
    sinkToImgPrimary->start();
    srcFromDevice->start();
    image1->start();
    image2->start();

    g_print("Let's run!\n");
    g_main_loop_run(loop);
#endif

#ifdef USE_AUDIO_SRC_SINK
    auto srcAudio = std::make_shared<SourceAudio>();
    auto sinkAudio = std::make_shared<SinkAudio>();
    auto sinkCallback = std::make_shared<SinkCallback>();
    srcAudio->addSink(sinkCallback);

    sinkCallback->setDataCb([=](uint8_t * data, uint32_t len) {
        sinkAudio->putData(data, len);
    });
    srcAudio->start();
    sinkAudio->start();
    sinkCallback->start();

    g_print("Let's run!\n");
    g_main_loop_run(loop);
#endif

#ifdef USE_CRASH_TEST
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

#if __APPLE__
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(libav);
    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(openh264);
    GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
    GST_PLUGIN_STATIC_REGISTER(x264);
    GST_PLUGIN_STATIC_REGISTER(isomp4);
    GST_PLUGIN_STATIC_REGISTER(applemedia);
    GST_PLUGIN_STATIC_REGISTER(videoconvertscale);
    GST_PLUGIN_STATIC_REGISTER(videorate);
    GST_PLUGIN_STATIC_REGISTER(pango);
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
    GST_PLUGIN_STATIC_REGISTER(osxvideo);
#endif

    for(int i=0; i<1000; i++) {
        std::cout << "test " << i+1 << " start" << std::endl;

        auto srcFromDevice = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::TimeOverlay);
        auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 1280,720, 20, 900000));
        auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 1280,720, 20, 900000));
        auto sinkToImgPrimary = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
        auto sinkToImgSecond = std::make_shared<SinkImage>(SinkImage::ImageType::Full);

        srcFromDevice->addSink(sinkToEncode);
        srcFromDevice->addSink(sinkToImgPrimary);
        srcDecode->addSink(sinkToImgSecond);
        sinkToImgPrimary->setImage(image1);
        sinkToImgSecond->setImage(image2);

        sinkToEncode->setOnEncoded(SinkEncode::OnEncoded([&](uint8_t *data, uint32_t len, uint64_t pts, uint64_t dts) {
            srcDecode->putDataToDecode(data, len);
        }));
        sinkToImgSecond->start();
        sinkToImgPrimary->start();
        sinkToEncode->start();
        srcDecode->start();
        srcFromDevice->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        srcFromDevice = nullptr;
        sinkToEncode = nullptr;
        srcDecode = nullptr;
        sinkToImgPrimary = nullptr;
        sinkToImgSecond = nullptr;

////        list<string> GstToolkit::all_plugins()
////        {
//            std::list<std::string> pluginlist;
//            GList *l, *g;
//
//            l = gst_registry_get_plugin_list (gst_registry_get ());
//
//            for (g = l; g; g = g->next) {
//                GstPlugin *plugin = GST_PLUGIN (g->data);
//                pluginlist.push_front(std::string( gst_plugin_get_name (plugin) ) );
//            }
//
//            gst_plugin_list_free (l);

//            gst_plugin_fr

//            return pluginlist;
//        }


        std::cout << "test " << i+1 << " end" << std::endl;
    }
#endif

#ifdef USE_CRASH_TEST_2
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    for(int i=0; i<1000; i++) {
        std::cout << "test " << i+1 << " start" << std::endl;

#if __APPLE__
        GST_PLUGIN_STATIC_REGISTER(coreelements);
        GST_PLUGIN_STATIC_REGISTER(libav);
        GST_PLUGIN_STATIC_REGISTER(app);
        GST_PLUGIN_STATIC_REGISTER(openh264);
        GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
        GST_PLUGIN_STATIC_REGISTER(x264);
        GST_PLUGIN_STATIC_REGISTER(isomp4);
        GST_PLUGIN_STATIC_REGISTER(applemedia);
        GST_PLUGIN_STATIC_REGISTER(videoconvertscale);
        GST_PLUGIN_STATIC_REGISTER(videorate);
        GST_PLUGIN_STATIC_REGISTER(pango);
        GST_PLUGIN_STATIC_REGISTER(videotestsrc);
        GST_PLUGIN_STATIC_REGISTER(osxvideo);
#endif

        auto srcFromDevice = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::None);
        auto sinkToImgPrimary = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
        auto sinkToImgSecond = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
        auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecVp8, 1280,720, 20, 400));
        auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecVp8, 1280 ,720, 20, 400));
        sinkToImgPrimary->setImage(image1);
        sinkToImgSecond->setImage(image2);

        sinkToEncode->setOnEncoded(SinkEncode::OnEncoded([&, srcDecode](uint8_t *data, uint32_t len, uint64_t pts, uint64_t dts) {
            srcDecode->putDataToDecode(data, len);
        }));
        srcDecode->addSink(sinkToImgSecond);
        srcFromDevice->addSink(sinkToImgPrimary);
        srcFromDevice->addSink(sinkToEncode);
        sinkToImgPrimary->start();
        sinkToImgSecond->start();
        sinkToEncode->start();
        srcDecode->start();
        srcFromDevice->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        sinkToEncode = nullptr;
        srcFromDevice = nullptr;
        sinkToImgPrimary = nullptr;
        sinkToImgSecond = nullptr;
        srcDecode = nullptr;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        std::cout << "test " << i+1 << " end" << std::endl;
        if(i == 300) {
            std::cout << "test " << i+1 << " end" << std::endl;
        }
    }
    gst_deinit();
//    tr.join();
#endif

#ifdef USE_DECODE_FROM_FILE
    auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 2560 /4 ,1600 /4, 20, 400));
    auto sinkToImgPrimary = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    auto sinkToImgSecond = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    sinkToImgPrimary->setImage(image1);
    sinkToImgSecond->setImage(image2);
    srcDecode->addSink(sinkToImgSecond);

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
    sinkToImgPrimary->start();
    sinkToImgSecond->start();

    g_print ("Let's run!\n");
    g_main_loop_run (loop);

    srcDecode = NULL;
    sinkToImgPrimary = NULL;
    sinkToImgSecond = NULL;
#endif
    g_print ("Going out\n");
    return 0;
}

int main(int argc, char *argv[]) {
#ifdef USE_QT
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
    engine.rootContext()->setContextProperty("provider1", (ImageProvider*) image1);
    engine.rootContext()->setContextProperty("provider2", (ImageProvider*) image2);
#endif
    auto tr = std::thread([&] {
        runLoop(0, NULL);
#ifdef USE_QT
        QCoreApplication::exit(-1);
#endif
    });
#ifdef USE_QT
    tr.detach();
    engine.load(url);
    auto ret = app.exec();
    return ret;
#else
    tr.join();
    return 0;
#endif
}
