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
//#define USE_CRASH_TEST

int tutorial_main (int argc, char *argv[]) {
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    auto loop = g_main_loop_new(NULL, FALSE);

#if defined(USE_VIDEO_TO_IMAGE_PREVIEW) || defined(USE_VIDEO_TO_ENCODE_FILE) || defined(USE_VIDEO_TO_ENCODE_CODEC)
    auto srcFromWebc = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Webc, SourceDevice::OptionType::TimeOverlay);
    auto srcFromScreen = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::TimeOverlay);

    auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 2560 /4,1600 /4, 20, 400));
    auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 2560 /4 ,1600 /4, 20, 400));

    auto sinkToImgLeft = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    auto sinkToImgRight = std::make_shared<SinkImage>(SinkImage::ImageType::Full);

    auto sinkToFile = std::make_shared<SinkFile>((QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test_app.mp4").toLocal8Bit().data());

    if(srcFromWebc->getError() || srcFromScreen->getError()
            || sinkToEncode->getError() || srcDecode->getError()
            || sinkToImgLeft->getError() || sinkToImgRight->getError()
            || sinkToFile->getError()) {
        std::cerr << "component failed" << std::endl;
        return -1;
    }
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
    sinkToImgLeft->setImage(image1);
    sinkToImgRight->setImage(image2);

    srcFromScreen->addSink(sinkToEncode);
    srcFromScreen->addSink(sinkToImgLeft);

    srcDecode->addSink(sinkToImgRight);

    sinkToEncode->setOnEncoded([&](uint8_t* data, uint32_t len,uint32_t pts, uint32_t dts) {
        srcDecode->putDataToDecode(data, len);
    });
    sinkToImgLeft->start();
    sinkToImgRight->start();
    srcFromScreen->start();
    sinkToEncode->start();
    srcDecode->start();
    g_print ("Let's run!\n");
    g_main_loop_run (loop);
#endif

#ifdef USE_CRASH_TEST
    for(int i=0; i<20; i++) {
        std::cout << "test " << i+1 << " start" << std::endl;

        auto srcFromWebc = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Webc, SourceDevice::OptionType::TimeOverlay);

        auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 2560 /4,1600 /4, 20, 400));
        auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 2560 /4 ,1600 /4, 20, 400));

        auto sinkToImgLeft = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
        auto sinkToImgRight = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
        sinkToImgLeft->setImage(image1);
        sinkToImgRight->setImage(image2);

        srcFromWebc->addSink(sinkToImgLeft);
        srcFromWebc->addSink(sinkToEncode);

        srcDecode->addSink(sinkToImgRight);

        sinkToEncode->setOnEncoded([&](uint8_t* data, uint32_t len,uint32_t pts, uint32_t dts) {
            srcDecode->putDataToDecode(data, len);
        });
        srcFromWebc->start();
        srcDecode->start();
        sinkToImgLeft->start();
        sinkToImgRight->start();
        sinkToEncode->start();

        auto tr = std::thread([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            g_main_loop_quit(loop);
        });
        tr.detach();

        g_print ("Let's run!\n");
        g_main_loop_run (loop);

        srcFromWebc = NULL;
        sinkToEncode = NULL;
        srcDecode = NULL;
        sinkToImgLeft = NULL;
        sinkToImgRight = NULL;

        std::cout << "test " << i+1 << " end" << std::endl;
    }
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
        tutorial_main(0, NULL);
        QCoreApplication::exit(-1);
    });
    tr.detach();

    engine.load(url);

    auto ret = app.exec();
    return ret;
}
