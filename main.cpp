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
#include "image/liveImage.h"
#include "image/imageProvider.h"

ImageProvider* provider = NULL;
QFile* outFile;

typedef struct {
    GMainLoop *loop;
    GstElement *source;
    GstElement *sink;
} ProgramData;

/* called when the appsink notifies us that there is a new buffer ready for
 * processing */
static GstFlowReturn
on_new_sample_from_sink (GstElement * elt, ProgramData * data)
{
//    GstSample *sample;
//    GstElement *source;
//    GstFlowReturn ret;

//    /* get the sample from appsink */
//    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

//    /* get source an push new sample */
//    source = gst_bin_get_by_name (GST_BIN (data->sink), "testsource");
//    ret = gst_app_src_push_sample (GST_APP_SRC (source), sample);
//    gst_object_unref (source);

//    /* we don't need the appsink sample anymore */
//    gst_sample_unref (sample);

////    return ret;
  GstSample *sample;
  GstBuffer *app_buffer, *buffer;
  GstElement *source;
  GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

  /* get the sample from appsink */
  sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

//  source = gst_bin_get_by_name (GST_BIN (data->sink), "testsource");
//  ret = gst_app_src_push_sample (GST_APP_SRC (source), sample);
//  gst_object_unref (source);
//  return ret;

  if(sample != NULL) {
    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *s = gst_caps_get_structure(caps, 0);
    int imW, imH;
    gst_structure_get_int(s,"width", &imW);
    gst_structure_get_int(s, "height", &imH);
    buffer = gst_sample_get_buffer(sample);

    if(buffer != NULL) {
          GstMapInfo mapInfo;
          gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

          if(provider != NULL) {
            // std::cout << "btest setImage: size=" << mapInfo.size << std::endl;
            provider->setImage(imW, imH, (uint8_t*)mapInfo.data, mapInfo.size);
          }

        source = gst_bin_get_by_name (GST_BIN (data->sink), "testsource");
        ret = gst_app_src_push_sample (GST_APP_SRC (source), sample);
        gst_object_unref (source);

          //do not need reference to the buffer here any more
          gst_buffer_unmap(buffer, &mapInfo);
          gst_sample_unref(sample);
    } else {
          printf ("BUFFER IS NULL \n\n\n");
    }
  }
  return ret; // return GstFlowReturn::GST_FLOW_OK;
}

/* called when we get a GstMessage from the source pipeline when we get EOS, we
 * notify the appsrc of it. */
static gboolean
on_source_message (GstBus * bus, GstMessage * message, ProgramData * data)
{
  GstElement *source;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_EOS:
      g_print ("The source got dry\n");
      source = gst_bin_get_by_name (GST_BIN (data->sink), "testsource");
      gst_app_src_end_of_stream (GST_APP_SRC (source));
      gst_object_unref (source);
      break;
    case GST_MESSAGE_ERROR:
      g_print ("Received error\n");
      g_main_loop_quit (data->loop);
      break;
    default:
      break;
  }
  return TRUE;
}

/* called when we get a GstMessage from the sink pipeline when we get EOS, we
 * exit the mainloop and this testapp. */
static gboolean
on_sink_message (GstBus * bus, GstMessage * message, ProgramData * data)
{
  /* nil */
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_EOS:
      g_print ("Finished playback\n");
      g_main_loop_quit (data->loop);
      break;
    case GST_MESSAGE_ERROR:
      g_print ("Received error\n");
      g_main_loop_quit (data->loop);
      break;
    default:
      break;
  }
  return TRUE;
}

static GstFlowReturn on_sample_to_file(GstElement * elt, ProgramData * data) {
  GstSample *sample;
  GstBuffer *app_buffer, *buffer;
  GstElement *source;
  GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

  /* get the sample from appsink */
  sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

  if(sample != NULL) {
    buffer = gst_sample_get_buffer(sample);
    if(buffer != NULL) {
          GstMapInfo mapInfo;
          gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

          if(outFile == NULL) {
              auto path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
              outFile = new QFile(path + "/test_app.mp4");
              outFile->open(QFile::WriteOnly);
          }
          auto bArray = QByteArray((char*) mapInfo.data, mapInfo.size);
          outFile->write(bArray);

          std::cout << "btest size = " << bArray.length() << std::endl;

          //do not need reference to the buffer here any more
          gst_buffer_unmap(buffer, &mapInfo);
          gst_sample_unref(sample);
    } else {
          printf ("BUFFER IS NULL \n\n\n");
    }
  }
  return GstFlowReturn::GST_FLOW_OK;
}

int tutorial_main (int argc, char *argv[]) {
    std::cout << "hello" << std::endl;
    auto data = new ProgramData();
    //mp4mux
    gst_init(NULL, NULL);
    // auto pipe_rgb_cmd = "avfvideosrc ! autovideoconvert ! appsink caps=video/x-raw,format=RGB name=testsink";
    // gst-launch-1.0 avfvideosrc ! videoconvert ! video/x-raw,format=RGB ! filesink location=/Users/khominvladimir/Desktop/test.bin
    // gst-launch-1.0 avfvideosrc ! videoconvert ! x264enc ! flvmux ! filesink location=/Users/khominvladimir/Desktop/test.mp4 -e
    // auto pipe_sink_cmd = "appsrc name=testsource ! videoconvert ! x264enc ! flvmux ! filesink location=/Users/khominvladimir/Desktop/test.mp4";
    //auto pipe_sink_cmd = "appsrc name=testsource ! videoconvert ! x264enc ! flvmux ! appsink name=out_file_sink";
    //auto pipe_rgb_cmd = "avfvideosrc ! videoconvert ! videoscale ! video/x-raw,format=RGB ! appsink name=testsink";

    auto pipe_encode_cmd = "appsrc name=testsource ! videoconvert ! videoscale ! video/x-raw,width=426,height=240 ! x264enc ! flvmux ! appsink name=out_file_sink";
//    auto pipe_mirror_cmd = "appsrc name=sourceToMirror ! videoconvert ! x264enc ! flvmux ! appsink name=out_file_sink";
#if __APPLE__
    auto pipe_rgb_cmd = "avfvideosrc capture-screen=true capture-screen-cursor=true capture-screen-mouse-clicks=true ! videoconvert ! video/x-raw,format=RGB ! appsink name=testsink";
#else
    auto pipe_rgb_cmd = "v4l2src ! videoconvert ! video/x-raw,format=RGB ! appsink name=testsink";
#endif

    // gst-launch-1.0 avfvideosrc capture-screen=true capture-screen-cursor=true capture-screen-mouse-clicks=true ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! queue ! x264enc ! mp4mux ! filesink location=/Users/khominvladimir/Desktop/test.mp4
    // gst-launch-1.0 avfvideosrc ! videoconvert ! videoscale ! video/x-raw,format=RGB ! queue ! videoconvert ! queue ! x264enc ! mp4mux ! filesink location=/Users/khominvladimir/Desktop/test.mp4
    // gst-launch-1.0 videotestsrc ! x264enc ! qtmux ! filesink location=/Users/khominvladimir/Desktop/test.mp4

    data->loop = g_main_loop_new (NULL, FALSE);
    data->source = gst_parse_launch(pipe_rgb_cmd, NULL);

    if (pipe_rgb_cmd == NULL || data->source == NULL) {
      std::cout << "not all elements created" << std::endl;
      delete data;
      return -1;
    }

    /* to be notified of messages from this pipeline, mostly EOS */
    auto bus = gst_element_get_bus (data->source);
    gst_bus_add_watch (bus, (GstBusFunc) on_source_message, data);
    gst_object_unref (bus);

    /* we use appsink in push mode, it sends us a signal when data is available
    * and we pull out the data in the signal callback. We want the appsink to
    * push as fast as it can, hence the sync=false */
    auto testsink = gst_bin_get_by_name (GST_BIN (data->source), "testsink");
    g_object_set (G_OBJECT (testsink), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect (testsink, "new-sample", G_CALLBACK (on_new_sample_from_sink), data);
    gst_object_unref (testsink);

    data->sink = gst_parse_launch (pipe_encode_cmd, NULL);
    if (data->sink == NULL) {
        g_print ("Bad sink\n");
        gst_object_unref (data->source);
        g_main_loop_unref (data->loop);
        delete data;
        return -1;
    }

    // write to file
    auto out_file_sink = gst_bin_get_by_name (GST_BIN (data->sink), "out_file_sink");
    g_object_set (G_OBJECT(out_file_sink), "emit-signals", TRUE, NULL);
    g_signal_connect (out_file_sink, "new-sample", G_CALLBACK (on_sample_to_file), NULL);

    auto testsource = gst_bin_get_by_name (GST_BIN (data->sink), "testsource");
    /* configure for time-based format */
    g_object_set (testsource, "format", GST_FORMAT_TIME, NULL);
    /* uncomment the next line to block when appsrc has buffered enough */
    //g_object_set (testsource, "block", TRUE, NULL);
    gst_object_unref (testsource);

    bus = gst_element_get_bus (data->sink);
    gst_bus_add_watch (bus, (GstBusFunc) on_sink_message, data);
    gst_object_unref (bus);

    /* launching things */
    gst_element_set_state (data->sink, GST_STATE_PLAYING);
    gst_element_set_state (data->source, GST_STATE_PLAYING);

    /* let's run !, this loop will quit when the sink pipeline goes EOS or when an
    * error occurs in the source or sink pipelines. */
    g_print ("Let's run!\n");
    g_main_loop_run (data->loop);
    g_print ("Going out\n");

    gst_element_set_state (data->source, GST_STATE_NULL);
    gst_element_set_state (data->sink, GST_STATE_NULL);

    gst_object_unref (data->source);
    gst_object_unref (data->sink);
    g_main_loop_unref (data->loop);
    delete data;
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

    provider = new ImageProvider();

    qmlRegisterType<LiveImage>("MyApp.Images", 1, 0, "LiveImage");
    engine.rootContext()->setContextProperty("LiveImageProvider", provider);

    auto tr = std::thread([&] {
        tutorial_main(0, NULL);
    });
    tr.detach();

    engine.load(url);

    auto ret = app.exec();
    return ret;
}
