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
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

#else

#include "image_provider/image_videosink.h"

#endif

#include "source/source_device.h"
#include "sink/sink_image.h"
#include "sink/sink_file.h"
#include "sink/sink_encode.h"
#include "source/source_decode.h"
#include "source/source_app.h"
#include "utils/codec_type.h"
#include "source/source_audio.h"
#include "sink/sink_audio.h"
#include "sink_callback.h"
#include "utils/measure.h"

#ifdef __ANDROID__
#include <jni.h>
#endif

ImageProviderAbstract *image1 = NULL;
ImageProviderAbstract *image2 = NULL;

//
// use one of these options one time
//
//#define USE_VIDEO_TO_IMAGE_PREVIEW
//#define USE_VIDEO_TO_ENCODE_FILE
#define USE_VIDEO_TO_ENCODE_CODEC
//#define USE_AUDIO_SRC_SINK
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
#elif __ANDROID__
extern "C" {
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(app);
GST_PLUGIN_STATIC_DECLARE(openh264);
GST_PLUGIN_STATIC_DECLARE(audioconvert);
GST_PLUGIN_STATIC_DECLARE(audioresample);
GST_PLUGIN_STATIC_DECLARE(androidmedia);
GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
//GST_PLUGIN_STATIC_DECLARE(x264);
//GST_PLUGIN_STATIC_DECLARE(isomp4);
//GST_PLUGIN_STATIC_DECLARE(videoconvert);
//GST_PLUGIN_STATIC_DECLARE(videoscale);
//GST_PLUGIN_STATIC_DECLARE(videorate);
//GST_PLUGIN_STATIC_DECLARE(libav);
//GST_PLUGIN_STATIC_DECLARE(ahcsrc);
GST_PLUGIN_STATIC_DECLARE(opensles);
}
#endif

std::vector<std::pair<std::shared_ptr<uint8_t>, uint32_t>> delayList;
int count = 0;

void putWithDelay(std::shared_ptr<SinkImage> sink, uint8_t *data, uint32_t len) {
    if (count++ < 50) {
        auto p = std::shared_ptr<uint8_t>(new uint8_t[len]);
        memcpy(p.get(), data, len);
        auto frame = std::pair(p, len);
        delayList.push_back(frame);
    } else {
        while (!delayList.empty()) {
            auto frame = delayList.back();
            sink->putData(frame.first.get(), frame.second);
            delayList.pop_back();
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
        sink->putData(data, len);
    }
}

int runLoop(int argc, char *argv[]) {
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

//    auto w = 1920;
//    auto h = 1200;
    auto w = 640;
    auto h = 360;

#if defined(USE_VIDEO_TO_IMAGE_PREVIEW) || defined(USE_VIDEO_TO_ENCODE_FILE) || defined(USE_VIDEO_TO_ENCODE_CODEC) || defined(USE_AUDIO_SRC_SINK)
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
#elif __ANDROID__
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(openh264);
    GST_PLUGIN_STATIC_REGISTER(audioconvert);
    GST_PLUGIN_STATIC_REGISTER(audioresample);
    GST_PLUGIN_STATIC_REGISTER(androidmedia);
    //GST_PLUGIN_STATIC_REGISTER(libav);
    GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
    //GST_PLUGIN_STATIC_REGISTER(x264);
    //GST_PLUGIN_STATIC_REGISTER(isomp4);
    //GST_PLUGIN_STATIC_REGISTER(videoconvert);
    //GST_PLUGIN_STATIC_REGISTER(videoscale);
    //GST_PLUGIN_STATIC_REGISTER(videorate);
    //GST_PLUGIN_STATIC_REGISTER(openslessink);
//    GST_PLUGIN_STATIC_REGISTER(opensles);
    //GST_PLUGIN_STATIC_REGISTER(ahcsrc);
#endif

    auto loop = g_main_loop_new(NULL, FALSE);
    auto srcFromDevice = std::make_shared<SourceDevice>(w, h, 25, SourceDevice::SourceDeviceType::Camera1,
                                                        SourceDevice::OptionType::None
    );
#ifdef USE_QT
    auto sinkToFile = std::make_shared<SinkFile>(w, h, "RGB",
                                                 (QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) +
                                                  "/test_app.mp4").toLocal8Bit().data());
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
    auto sinkToImgPrimary = std::make_shared<SinkImage>("RGB", w, h, w, h, 25);
    auto sinkToImgSecond = std::make_shared<SinkImage>("RGB", w,h, w/2,h/2,25);
    sinkToImgSecond->setImage(image1);
    sinkToImgPrimary->setImage(image2);

    auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, w, h, 25, 9000, 50));
    auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, w, h, 25, 9000));

    sinkToEncode->setOnEncoded(SinkEncode::OnEncoded([=](uint8_t *data, uint32_t len, uint64_t pts, uint64_t dts) {
        srcDecode->putData(data, len);
    }));
//    srcFromDevice->addSink(sinkToImgSecond);
//    srcFromDevice->addSink(sinkToEncode);
    srcDecode->addSink(sinkToImgPrimary);
    auto sinkCallback = std::make_shared<SinkCallback>();
    auto sourceApp = std::make_shared<SourceApp>("RGB",w, h, 25);

//    sinkToImgSecond->putData(data, len);
//    sinkToImgPrimary->putData(data, len);
//    sinkToEncode->putData(data, len);

    sinkCallback->setDataCb([=](uint8_t *data, uint32_t len) {
        std::cout << "BTEST: putData IN-1" << std::endl;
        auto p = std::shared_ptr<uint8_t>(new uint8_t[len]);
        memcpy(p.get(), data, len);
        std::thread([p, sourceApp, len]() {
            std::cout << "BTEST: putData IN-2" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            sourceApp->putData(p.get(), len);
            std::cout << "BTEST: putData END " << std::endl;
        }).detach();
    });
    srcFromDevice->addSink(sinkCallback);
    sourceApp->addSink(sinkToImgSecond);
    sourceApp->addSink(sinkToEncode);
    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        sourceApp->start();
    }).detach();
    sinkCallback->start();
    sinkToImgSecond->start();
    sinkToImgPrimary->start();
    sinkToEncode->start();
    srcDecode->start();
    srcFromDevice->start();

    // 2
//    srcFromDevice->addSink(sinkCallback);
//    sinkCallback->setDataCb([=](uint8_t *data, uint32_t len) {
//        sinkToImgSecond->putData(data, len);
//        sinkToImgPrimary->putData(data, len);
//    });
//    sinkCallback->start();

//    // 3
//    static std::shared_ptr<SinkImage> sinkToImgPrimary;
//    static std::shared_ptr<SinkImage> sinkToImgSecond;
////    sinkToImgPrimary->setImage(image2);
////    sinkToImgSecond->setImage(image1);
//
//    srcFromDevice->onConfigChanged([=](uint32_t width, uint32_t height) {
//        sinkToImgPrimary = std::make_shared<SinkImage>("RGB", width, height, width / 3, height / 3);
////        sinkToImgPrimary->setImage(m_img1.get());
//        sinkToImgPrimary->setImage(image2);
//        srcFromDevice->addSink(sinkToImgPrimary);
//
//        sinkToImgSecond = std::make_shared<SinkImage>("RGB", width, height, width / 3, height / 3);
//        sinkToImgSecond->setImage(image1);
//        srcFromDevice->addSink(sinkToImgSecond);
////        if(!recordToFile.empty()) {
////            sinkToFile = std::make_shared<SinkFile>(width, height, "RGB", recordToFile);
////            srcFromDevice->addSink(sinkToFile);
////            sinkToFile->start();
////        }
//        std::thread([]() {
//            sinkToImgSecond->start();
//            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//            sinkToImgPrimary->start();
//        }).detach();
//
////        sinkToEncode->start();
//    });
//    srcFromDevice->start();

//
//    // 4
//    static std::shared_ptr<SinkImage> sinkToImgPrimary;
////    static std::shared_ptr<SinkImage> sinkToImgSecond;
//
//    sinkToImgPrimary = std::make_shared<SinkImage>("RGB", w, h, w/2, h/2);
////    sinkToImgSecond = std::make_shared<SinkImage>("RGB", w, h, w/2, h/2);
//    sinkToImgPrimary->setImage(image2);
////    sinkToImgSecond->setImage(image1);
//
////    srcFromDevice->addSink(sinkToImgPrimary);
////    srcFromDevice->addSink(sinkToImgSecond);
//    auto sinkCallback = std::make_shared<SinkCallback>();
//    srcFromDevice->addSink(sinkCallback);
//
//    sinkCallback->setDataCb([=](uint8_t *data, uint32_t len) {
//        putWithDelay(sinkToImgPrimary, data, len);
//    });
////    sinkToImgSecond->start();
//    sinkToImgPrimary->start();
//    sinkCallback->start();
//
////    std::thread([w, h, srcFromDevice]() {
////        while(true) {
////            auto dst_argb_size = w * h * 4;
////            auto dst_argb = std::shared_ptr<uint8_t>(new uint8_t[dst_argb_size]);
////            memset((uint8_t *) dst_argb.get(), 150, dst_argb_size);
////            srcFromDevice->putVideoFrame((uint8_t *) dst_argb.get(), dst_argb_size, w, h);
////            std::this_thread::sleep_for(std::chrono::milliseconds(5));
////        }
////    }).detach();
//
////    std::thread([w, h, srcFromDevice]() {
////        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//        srcFromDevice->start();
////    }).detach();

#ifndef USE_QT
    image1 = new ImageVideoSink();
    image2 = new ImageVideoSink();
#endif
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

    sinkCallback->setDataCb([=](uint8_t *data, uint32_t len) {
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
    std::shared_ptr<SourceDecode> srcDecode;
    auto loop = g_main_loop_new(NULL, FALSE);

    auto tr = std::thread([&] {
//        QFile inputFile(        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/raw.json");
        QFile inputFile(        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/raw_andrey_b.json");
        if (inputFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inputFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                auto doc = QJsonDocument::fromJson(line.toUtf8());
                auto jobj = doc.object();
                auto bitrate = jobj["bitrate"].toInt();
                auto counter = jobj["counter"].toInt();
                auto chan = jobj["chan"].toInt();
                auto timeStr = jobj["time"].toString();
                auto time = timeStr.toLongLong();
                auto flags = jobj["flags"].toInt();
                auto data = jobj["data"].toString();
                auto codec = jobj["codec"].toInt();
                auto width = jobj["width"].toInt();
                auto height = jobj["height"].toInt();
                auto sampleRate = jobj["sample_rate"].toInt();
                auto keyRate = jobj["key_rate"].toInt();
                auto frame = QByteArray::fromBase64(data.toUtf8(), QByteArray::Base64UrlEncoding);

                if(srcDecode == nullptr) {
                    srcDecode = std::make_shared<SourceDecode>(
                            DecoderConfig::make(CodecType::CodecAvc, width, height, sampleRate, bitrate));
                    srcDecode->start();

                    auto sinkToImgPrimary = std::make_shared<SinkImage>(width / 2, height / 2);
                    sinkToImgPrimary->setImage(image1);

                    srcDecode->addSink(sinkToImgPrimary);
                    sinkToImgPrimary->start();
                }

                if(!frame.isEmpty()) {
                    srcDecode->putDataToDecode((uint8_t*)frame.data(), frame.length(), time);
                    std::cout << "frame put:" << counter << ": length: " << frame.length() << std::endl;
//                    if(frame.length() == 21677 || frame.length() == 12311) {
//                        std::cout << "frame put:" << counter << ": length: " << frame.length() << std::endl;
//                    }
                } else {
                    std::cerr << "frame is empty" << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            inputFile.close();
        }
    });
    tr.detach();

    g_print ("Let's run!\n");
    g_main_loop_run (loop);
#endif
    g_print("Going out\n");
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
    engine.rootContext()->setContextProperty("provider1", (ImageProvider *) image1);
    engine.rootContext()->setContextProperty("provider2", (ImageProvider *) image2);
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

#ifdef __ANDROID__
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_mytestcpp_MainActivity_startExternal(JNIEnv *env, jobject thiz, jint argc) {
    main(0, nullptr);
    return 0;
}
#endif