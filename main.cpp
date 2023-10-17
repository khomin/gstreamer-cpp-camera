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

#include <QThread>
#include <QFile>
#include <QStandardPaths>
#include "image_provider/live_image.h"
#include "image_provider/image_provider.h"

ImageProvider* image1 = NULL;
ImageProvider* image2 = NULL;

#include "source/source_device.h"
#include "sink/sink_image.h"
#include "sink/sink_file.h"
#include "sink/sink_encode.h"
#include "source/source_decode.h"

//#define USE_VIDEO_TO_IMAGE_PREVIEW
//#define USE_VIDEO_TO_ENCODE_FILE
#define USE_VIDEO_TO_ENCODE_CODEC

int tutorial_main (int argc, char *argv[]) {
    gst_init(NULL, NULL);
    gst_debug_set_active(TRUE);
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

    auto srcFromWebc = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Webc);
    auto srcFromScreen = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Screen);
    auto srcDecode = std::make_shared<SourceDecode>();

    auto sinkToImgPreview = std::make_shared<SinkImage>(SinkImage::ImageType::Preview);
    auto sinkToImgFull = std::make_shared<SinkImage>(SinkImage::ImageType::Full);
    auto sinkToFile = std::make_shared<SinkFile>((QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test_app.mp4").toLocal8Bit().data());
    auto sinkToEncode = std::make_shared<SinkEncode>();

#ifdef USE_VIDEO_TO_IMAGE_PREVIEW
    srcFromScreen->addSink(sinkToImgFull);
    sinkToImgFull->setImage(image1);
    sinkToImgFull->start();
    srcFromScreen->start();
#endif

#ifdef USE_VIDEO_TO_ENCODE_FILE
    srcFromScreen->addSink(sinkToFile);
    srcFromScreen->start();
    sinkToFile->start();
#endif

#ifdef USE_VIDEO_TO_ENCODE_CODEC
    sinkToImgFull->setImage(image1);
    sinkToImgPreview->setImage(image2);

    srcFromScreen->addSink(sinkToEncode);
    srcFromScreen->addSink(sinkToImgFull);

    srcDecode->addSink(sinkToImgPreview);

    sinkToEncode->setOnEncoded([&](uint8_t* data, uint32_t len) {
        srcDecode->putData(data, len);
    });
    sinkToImgFull->start();
    sinkToImgPreview->start();
    srcFromScreen->start();
    sinkToEncode->start();
    srcDecode->start();
#endif

    g_print ("Let's run!\n");
    auto loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);
    g_print ("Going out\n");
    return 0;
}

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    bool _isQuit = false;

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
    });
    tr.detach();

    engine.load(url);

    auto ret = app.exec();
    return ret;
}

static gboolean my_bus_func (GstBus * bus, GstMessage * message, gpointer user_data) {
  GstDevice *device;
  gchar *name;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_DEVICE_ADDED:
      gst_message_parse_device_added (message, &device);
      name = gst_device_get_display_name (device);
      g_print("Device added: %s\n", name);
      g_free (name);
      gst_object_unref (device);
      break;
    case GST_MESSAGE_DEVICE_REMOVED:
      gst_message_parse_device_removed (message, &device);
      name = gst_device_get_display_name (device);
      g_print("Device removed: %s\n", name);
      g_free (name);
      gst_object_unref (device);
      break;
    default:
      break;
  }

  return G_SOURCE_CONTINUE;
}

GstDeviceMonitor* setup_raw_video_source_device_monitor (void) {
    GstDeviceMonitor *monitor;
    GstBus *bus;
    GstCaps *caps;

    monitor = gst_device_monitor_new ();

    bus = gst_device_monitor_get_bus (monitor);
    gst_bus_add_watch (bus, my_bus_func, NULL);
    gst_object_unref (bus);

    caps = gst_caps_new_empty_simple ("video/x-raw");
    gst_device_monitor_add_filter (monitor, "Video/Source", caps);
    gst_caps_unref (caps);

    gst_device_monitor_start (monitor);

    return monitor;
}
