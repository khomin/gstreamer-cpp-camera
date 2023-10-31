#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>
#include <thread>

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

ImageProviderAbstract* image1 = NULL;
ImageProviderAbstract* image2 = NULL;
uint64_t maxPacketSize = 0;

//#define USE_VIDEO_TO_IMAGE_PREVIEW
//#define USE_VIDEO_TO_ENCODE_FILE
#define USE_VIDEO_TO_ENCODE_CODEC
//#define USE_DECODE_FROM_FILE
//#define USE_CRASH_TEST

int runLoop (int argc, char *argv[]) {
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

#if defined(USE_VIDEO_TO_IMAGE_PREVIEW) || defined(USE_VIDEO_TO_ENCODE_FILE) || defined(USE_VIDEO_TO_ENCODE_CODEC)
    auto loop = g_main_loop_new(NULL, FALSE);
    auto srcFromWebc = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Webc, SourceDevice::OptionType::TimeOverlay);
    auto srcFromScreen = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::TimeOverlay);
    auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecVp8, 1280,720, 30, 1000000 / 1000));
    auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecVp8, 1280,720, 20, 1000000 / 1000));
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
    srcFromScreen->addSink(sinkToEncode);
    srcFromScreen->addSink(sinkToImgLeft);
    srcDecode->addSink(sinkToImgRight);
    sinkToImgLeft->setImage(image1);
    sinkToImgRight->setImage(image2);

    sinkToEncode->setOnEncoded(std::make_shared<SinkEncode::OnEncoded>([&](uint8_t *data, uint32_t len, uint64_t pts, uint64_t dts) {
        if(maxPacketSize < len) {
            maxPacketSize = len;
            std::cout << "BTEST_MAX: " << maxPacketSize << std::endl; // max 163840
        }
        srcDecode->putDataToDecode(data, len);
    }));
    sinkToImgRight->start();
    sinkToImgLeft->start();
    sinkToEncode->start();
    srcDecode->start();
    srcFromScreen->start();
    g_print("Let's run!\n");
    g_main_loop_run(loop);
#endif

#ifdef USE_CRASH_TEST
    for(int i=0; i<1000; i++) {
        std::cout << "test " << i+1 << " start" << std::endl;

        auto srcFromScreen = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen, SourceDevice::OptionType::TimeOverlay);
        auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 1280,720, 20, 900000));
        auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, 1280,720, 20, 900000));
        auto sinkToImgLeft = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
        auto sinkToImgRight = std::make_shared<SinkImage>(SinkImage::ImageType::Full);

        srcFromScreen->addSink(sinkToEncode);
        srcFromScreen->addSink(sinkToImgLeft);
        srcDecode->addSink(sinkToImgRight);
        sinkToImgLeft->setImage(image1);
        sinkToImgRight->setImage(image2);

        sinkToEncode->setOnEncoded(std::make_shared<SinkEncode::OnEncoded>([&](uint8_t *data, uint32_t len, uint64_t pts, uint64_t dts) {
            srcDecode->putDataToDecode(data, len);
        }));
        sinkToImgRight->start();
        sinkToImgLeft->start();
        sinkToEncode->start();
        srcDecode->start();
        srcFromScreen->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        srcFromScreen = nullptr;
        sinkToEncode = nullptr;
        srcDecode = nullptr;
        sinkToImgLeft = nullptr;
        sinkToImgRight = nullptr;
        std::cout << "test " << i+1 << " end" << std::endl;
    }
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
