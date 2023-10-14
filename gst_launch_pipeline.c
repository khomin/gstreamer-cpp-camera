
//// https://stackoverflow.com/questions/59427179/gst-parse-launch-differs-output-from-command-line-gst-launch
//// https://cpp.hotexamples.com/examples/-/-/gst_parse_launch/cpp-gst_parse_launch-function-examples.html


//// https://stackoverflow.com/questions/38959678/is-there-a-simple-gstreamer-example-for-c
////  gcc -Wall gst_launch_pipeline.c  $(pkg-config --cflags --libs gstreamer-1.0) $(pkg-config --cflags --libs  glib-2.0) -o gst_launch_pipeline




////  command line pipeline examples
//// https://gist.github.com/hum4n0id/2760d987a5a4b68c24256edd9db6b42b#audio--video-rtp-streaming
////   https://gist.github.com/hum4n0id/cda96fb07a34300cdb2c0e314c14df0a

////

//// gst-launch-1.0 -v videotestsrc ! videoconvert ! fpsdisplaysink
//// gst-launch-1.0 -v videotestsrc ! video/x-raw,width=640,height=360  ! videoconvert ! fpsdisplaysink
////
//// gst-launch-1.0 -v videotestsrc ! x264enc ! h264parse ! decodebin ! videoconvert ! fpsdisplaysink sync=false
//// gst-launch-1.0 -v videotestsrc ! x264enc ! decodebin ! videoconvert ! fpsdisplaysink sync=false
//// gst-launch-1.0 -v videotestsrc ! video/x-raw,width=320,height=240  ! x264enc ! decodebin ! videoconvert ! fpsdisplaysink




////     https://stackoverflow.com/questions/62813912/how-to-record-a-stream-into-a-file-while-using-appsink-using-gstreame
//// GstElement *sink = gst_bin_get_by_name(GST_BIN(m_pipeline), "sink");
//// gst_app_sink_set_emit_signals((GstAppSink*)sink, true); 
//// gst_app_sink_set_drop((GstAppSink*)sink, true);
////    Need to add "queue" in the appsink tee. don't know why. 
//// udpsrc port=5000 ! application/x-rtp,media=video,clock-rate=90000,encoding-name=H264,payload=96 ! tee name=t t. ! queue ! rtph264depay ! decodebin ! videoconvert ! video/x-raw,format=BGR ! videoconvert ! appsink name=sink  t. ! queue ! rtph264depay ! h264parse ! mp4mux ! filesink location=Flight.mp4 name=record
/////// And add an eos_event manually





//// gst_parse_launch examples with/wo appsink/appsource
//// https://cpp.hotexamples.com/examples/-/-/gst_parse_launch/cpp-gst_parse_launch-function-examples.html


//// appsink examples
//// https://github.com/GStreamer/gst-plugins-base/blob/master/tests/examples/app/appsink-src2.c
//// https://github.com/GStreamer/gst-plugins-base/blob/master/tests/examples/app/appsink-src.c
//// https://stackoverflow.com/questions/62813912/how-to-record-a-stream-into-a-file-while-using-appsink-using-gstreamer
////  appsrc push_data https://gstreamer.freedesktop.org/documentation/tutorials/playback/short-cutting-the-pipeline.html?gi-language=c
//// appsink -> appsrc example https://eva-support.adlinktech.com/docs/sample-code-topic-nametemplate-2
//// EPIC!:  https://www.it-jim.com/blog/gstreamer-cpp-tutorial/


////gst-inspect-1.0 appsink
////   shows dependency on /usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgstapp.so
//// dpkg -S  /usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgstapp.so
////gstreamer1.0-plugins-base:amd64: /usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgstapp.so
//// /usr/lib/x86_64-linux-gnu/pkgconfig/gstreamer-plugins-base-1.0.pc



//// GstBuffer API https://gstreamer.freedesktop.org/documentation/gstreamer/gstbuffer.html?gi-language=c

#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>
#include <stdio.h>










//---------------------------------------------------------------------------------
//// pretty print:  https://stackoverflow.com/questions/72565413/gstreamer-appsink-to-rtsp-server-with-appsrc-as-source-large-latency
//// https://github.com/Kurento/gstreamer/blob/master/tools/gst-inspect.c
static gboolean print_field (GQuark field, const GValue * value, gpointer pfx) {
  gchar *str = gst_value_serialize (value);

  g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
  g_free (str);
  return TRUE;
}


static void print_caps (const GstCaps * caps, const gchar * pfx) {
  guint i;

  g_return_if_fail (caps != NULL);

  if (gst_caps_is_any (caps)) {
    g_print ("%sANY\n", pfx);
    return;
  }
  if (gst_caps_is_empty (caps)) {
    g_print ("%sEMPTY\n", pfx);
    return;
  }

  for (i = 0; i < gst_caps_get_size (caps); i++) {
    GstStructure *structure = gst_caps_get_structure (caps, i);

    g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
    gst_structure_foreach (structure, print_field, (gpointer) pfx);

  }
}


static void printf_caps (const GstCaps * caps, int buf_len) {
    const gchar cc[buf_len];
    memset(cc,0,sizeof(cc));
    print_caps (caps, cc);
    g_print("caps=%s",cc);
}

//---------------------------------------------------------------------------------

static void
print_pad_info (GstElement * element)
{
  const GList *pads;
  GstPad *pad;

  g_print ("\n");
  g_print ("Pads:\n");

  if (!element->numpads) {
    g_print ("  none\n");
    return;
  }

  pads = element->pads;
  while (pads) {
    gchar *name;
    GstCaps *caps;

    pad = GST_PAD (pads->data);
    pads = g_list_next (pads);

    name = gst_pad_get_name (pad);
    if (gst_pad_get_direction (pad) == GST_PAD_SRC)
      g_print ("  SRC: '%s'\n", name);
    else if (gst_pad_get_direction (pad) == GST_PAD_SINK)
      g_print ("  SINK: '%s'\n", name);
    else
      g_print ("  UNKNOWN!!!: '%s'\n", name);

    g_free (name);

    if (pad->padtemplate)
      g_print ("    Pad Template: '%s'\n", pad->padtemplate->name_template);

    caps = gst_pad_get_current_caps (pad);
    if (caps) {
      g_print ("    Capabilities:\n");
      print_caps (caps, "      ");
      gst_caps_unref (caps);
    }
  }
}


static void
print_hierarchy (GType type, gint level, gint * maxlevel,gchar *_name)
{
  GType parent;
  gint i;

  parent = g_type_parent (type);

  *maxlevel = *maxlevel + 1;
  level++;

  if (parent)
    print_hierarchy (parent, level, maxlevel,_name);

  if (_name)
    g_print ("%s", _name);

  for (i = 1; i < *maxlevel - level; i++)
    g_print ("      ");
  if (*maxlevel - level)
    g_print (" +----");

  g_print ("%s\n", g_type_name (type));

  if (level == 1)
    g_print ("\n");
}


static void
print_interfaces (GType type, gchar *_name)
{
  guint n_ifaces;
  GType *iface, *ifaces = g_type_interfaces (type, &n_ifaces);
  
  if (ifaces) {
    if (n_ifaces) {
 
      if (_name)
        g_print ("%s", _name);
      g_print ("Implemented Interfaces:\n");
      iface = ifaces;
      while (*iface) {
        if (_name)
          g_print ("%s", _name);
        g_print ("  %s\n", g_type_name (*iface));
        iface++;
      }

      if (_name)
        g_print ("%s", _name);
      g_print ("\n");
    }
    g_free (ifaces);
  }
  
}

//---------------------------------------------------------------------------------

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      printf("End of stream\n");
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      printf("Error: %s\n", error->message);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}



static void
cb_need_data2 (GstElement *appsrc,
	      guint unused_size,
	      gpointer user_data)
{
  printf("APPSRC NEEDS DATA!!!!!!!!!!!!!!!!!!!!!!!!!\n");
/*
  static gboolean white = FALSE;
  static GstClockTime timestamp = 0;
  GstBuffer *buffer;
  guint size;
  GstFlowReturn ret;
  size = 320 *240 *1;
  buffer = gst_buffer_new_allocate (NULL, size, NULL);
  /// this makes the image black/white 
  gst_buffer_memset (buffer, 0, white ? 0xff : 0x0, size);
  white = !white;
  GST_BUFFER_PTS (buffer) = timestamp;
  GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);
  timestamp += GST_BUFFER_DURATION (buffer);
  g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
  //gst_app_src_push_buffer(GST_APP_SRC(appsrc),buffer);//this alternative method which  also works
  if (ret != GST_FLOW_OK) {
    //// something wrong, stop pushing
    //    g_main_loop_quit (loop);
  }
*/ 

}


static void
push_buffer_gray8 (GstElement *appsrc)
{
  printf("APPSRC NEEDS DATA!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  static gboolean white = FALSE;
  static GstClockTime timestamp = 0;
  GstBuffer *buffer;
  guint size;
  GstFlowReturn ret;
  size = 320 *240 *1;
  buffer = gst_buffer_new_allocate (NULL, size, NULL);
  /// this makes the image black/white 
  gst_buffer_memset (buffer, 0, white ? 0xff : 0x0, size);
  white = !white;
  GST_BUFFER_PTS (buffer) = timestamp;
  GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);
  timestamp += GST_BUFFER_DURATION (buffer);
  g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
  //gst_app_src_push_buffer(GST_APP_SRC(appsrc),buffer);//this alternative method which  also works

 

}

static void
push_buffer_rgb16 (GstElement *appsrc)
{
  printf("APPSRC NEEDS DATA!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  static gboolean white = FALSE;
  static GstClockTime timestamp = 0;
  GstBuffer *buffer;
  guint size;
  GstFlowReturn ret;
  size = 320 *240 *2;
  buffer = gst_buffer_new_allocate (NULL, size, NULL);
  /// this makes the image black/white
  gst_buffer_memset (buffer, 0, white ? 0xA5 : 0x0f, size);
  white = !white;
  GST_BUFFER_PTS (buffer) = timestamp;
  GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);
  timestamp += GST_BUFFER_DURATION (buffer);
  g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
  //gst_app_src_push_buffer(GST_APP_SRC(appsrc),buffer);//this alternative method which  also works

}


////memcpy(vpu_enc->buf_data[i], GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer));


static void
push_buffer_copy (GstElement *appsrc, GstBuffer *srcBuf)
{




  static gboolean white = FALSE;
  static GstClockTime timestamp = 0;
  GstBuffer *buffer;
  guint size;
  GstFlowReturn ret;


  if(srcBuf){
    GstBuffer *bufferCopy = gst_buffer_copy_deep (srcBuf);//TODO: what's with refcounter?
    gst_app_src_push_buffer(GST_APP_SRC(appsrc),bufferCopy);
  }


  //GstBuffer *bufferCopy = gst_buffer_new_memdup (gconstpointer data,  gsize size);

/*
  if(srcBuf){
       
    size = GST_BUFFER_SIZE(srcBuf); //BROKEN COMPILATION 
    
    buffer = gst_buffer_new_allocate (NULL, size, NULL);
    
    //// this makes the image black/white
    gst_buffer_memset (buffer, 0, white ? 0xA5 : 0x0f, size);
    white = !white;
    //GST_BUFFER_PTS (buffer) = timestamp;
    //GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);
    //timestamp += GST_BUFFER_DURATION (buffer);
        
    memcpy( GST_BUFFER_DATA(buffer), (GST_BUFFER_DATA(srcBuf)), size);
    GST_BUFFER_PTS (buffer) = GST_BUFFER_PTS (srcBuf);
    GST_BUFFER_DURATION (buffer) = GST_BUFFER_DURATION (srcBuf);
    
    g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);
    //gst_app_src_push_buffer(GST_APP_SRC(appsrc),buffer);//this alternative method which  also works
      
  }
*/
}


int
main (int argc, char *argv[])
{
  GstElement *pipeline;
  GstElement *pipeline2;
  GstBus *bus;
  GstElement *sink;
  GstElement *capsfilter;
  GstCaps *caps;
  GstMessage *msg;
  guint bus_watch_id;


  GstBuffer *buffer;
  GMainLoop *loop;




  //char *pline = "v4l2src device='/dev/video0' ! 'video/x-raw,format=(string)YUY2,width=(int)640,height=(int)480' ! nvvidconv ! 'video/x-raw(memory:NVMM),format=(string)NV12,width=(int)640,height=(int)480' ! nvvidconv ! 'video/x-raw,format=(string)NV12,width=(int)640,height=(int)480' ! nvvideoconvert ! 'video/x-raw(memory:NVMM),format=(string)NV12,width=(int)640,height=(int)480' ! mux.sink_0 nvstreammux live-source=1 name=mux batch-size=1 width=640 height=480 ! nvinfer config-file-path=/opt/nvidia/deepstream/deepstream-4.0/sources/apps/sample_apps/deepstream-test1/dstest1_pgie_config.txt batch-size=1 ! nvmultistreamtiler rows=1 columns=1 width=640 height=480 ! nvvideoconvert ! nvdsosd ! nvegltransform ! nveglglessink sync=false";



  //TODO: insert rtp    rtph264pay  rtph264depay 
//----
  //char *pline = " videotestsrc  name=the_source ! capsfilter name=cf  ! video/x-raw,width=320,height=240  ! x264enc ! decodebin ! videoconvert ! fpsdisplaysink name=the_sink";
////REF pipeline
//char *pline = "videotestsrc  name=the_source ! capsfilter name=cf  ! video/x-raw,width=320,height=240,format=GRAY8 ! videoconvert  ! x264enc ! rtph264pay  config-interval=1 !  rtph264depay ! decodebin ! videoconvert !  fpsdisplaysink name=the_sink";
  
//// USP streaming
//  char *pline = "videotestsrc  name=the_source ! capsfilter name=cf  !  x264enc ! rtph264pay  config-interval=1  ! udpsink port=3445 host=127.0.0.1 name=the_sink";


//// RAW images
//     char *pline = " videotestsrc  name=the_source ! capsfilter name=cf  !  x264enc ! decodebin ! videoconvert !  video/x-raw,width=320,height=240,format=GRAY8 ! appsink name=the_sink";
//    char *pline = "videotestsrc  name=the_source ! capsfilter name=cf  ! appsink max-buffers=10 name=the_sink";
    char *pline = "videotestsrc  name=the_source ! capsfilter name=cf  ! videoconvert! appsink max-buffers=10 name=the_sink";


//----


////REF pipeline
//   char *pline2 =  " videotestsrc  name=the_source ! capsfilter name=cf  ! video/x-raw,width=320,height=240,format=BGR ! videoconvert  config-interval=1 ! x264enc ! decodebin ! videoconvert ! fpsdisplaysink name=the_sink";

//// UDP streaming
//char *pline2 =  " udpsrc port=3445 name=the_source  ! application/x-rtp ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink name=the_sink";


//// RAW images
   char *pline2 =  " appsrc  name=the_source ! video/x-raw,width=320,height=240,format=RGB16 !  videoconvert ! fpsdisplaysink name=the_sink";



  //---- 


  //// Initialize GStreamer
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);//!!! ???

  //// Build the appsink pipeline
  pipeline = gst_parse_launch(pline,NULL);

  ////  Build the appsrc pipeline
  pipeline2 = gst_parse_launch(pline2,NULL);



  //// get pipeline components by name
  sink = gst_bin_get_by_name(GST_BIN(pipeline), "the_sink");
  printf("\nsink = 0x%x\n\n",sink);
  gst_app_sink_set_emit_signals((GstAppSink*)sink, TRUE);

  //TODO
  // https://eva-support.adlinktech.com/docs/sample-code-topic-nametemplate-2
  //  g_object_set (G_OBJECT(sink), "emit-signals", TRUE, NULL);
  //  g_signal_connect (sink, "new-sample", G_CALLBACK (new_sample), NULL);


  //TODO: use it with appsink
  //gst_app_sink_get_max_buffers(GST_APP_SINK (sink));



  capsfilter = gst_bin_get_by_name (GST_BIN (pipeline), "cf");
  if(capsfilter != NULL){
    caps = gst_caps_from_string("video/x-raw,width=320,height=240,format=GRAY8");
    g_object_set(capsfilter,"caps",caps,NULL);
  }


  if (!pipeline || !capsfilter || !sink) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }





  //// get pipeline components by name
   GstElement *source2 = gst_bin_get_by_name(GST_BIN(pipeline2), "the_source");
//  printf("\nsource2 = 0x%x\n\n",source2);
////   gst_app_src_end_of_stream(GST_APP_SRC(source2));
   gst_app_src_set_emit_signals((GstAppSink*)source2, TRUE);


  //// setup appsrc stream type (type: push or pull)
  g_object_set (G_OBJECT (source2),
		"stream-type", 0,
		"format", GST_FORMAT_TIME, NULL);
  g_signal_connect (source2, "need-data", G_CALLBACK (cb_need_data2), NULL);

   ////do we  need to tell the appsrc what format o frames it produces?
//// https://eva-support.adlinktech.com/docs/sample-code-topic-nametemplate-2
//// https://usermanual.wiki/Pdf/gstreamerapplicationmanual.325090947/html  chap.19.2.1.3
g_object_set (G_OBJECT (source2), "caps",
	      gst_caps_new_simple ("video/x-raw",
				   "format", G_TYPE_STRING, "RGB16",//GRAY8,RGB16, I420
				   "width", G_TYPE_INT, 320,
				   "height", G_TYPE_INT, 240,
//				   "framerate", GST_TYPE_FRACTION, 30, 1,
				   NULL), NULL);
g_object_set (G_OBJECT (sink), "caps",
	      gst_caps_new_simple ("video/x-raw",
				   "format", G_TYPE_STRING, "RGB16", //GRAY8, RGB16, I420
				   "width", G_TYPE_INT, 320,
				   "height", G_TYPE_INT, 240,
//				   "framerate", GST_TYPE_FRACTION, 30, 1,
				   NULL), NULL);

//GstCaps * gst_caps_from_string (gchar*);




  //// Start playing
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  gst_element_set_state (pipeline2, GST_STATE_PLAYING);

  //// get bus
  bus = gst_element_get_bus (pipeline);
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);

  
//TODO
//  buffer = gst_app_sink_pull_buffer (GST_APP_SINK (sink));

  //// TODO: ???
  //// Iterate
//  g_print ("Running...\n");
//  g_main_loop_run (loop);







  int bufCnt = 0;
  while(1) {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
    printf("\nsample = 0x%x\n\n",sample);
    if(sample != NULL) {
      GstCaps *caps = gst_sample_get_caps(sample);
      GstStructure *s = gst_caps_get_structure(caps, 0);
      int imW, imH;
      gst_structure_get_int(s,"width", &imW);gst_structure_get_int(s, "height", &imH);
      printf("imW=%d imH=%d\n",imW,imH);
      //// https://gstreamer.freedesktop.org/documentation/gstreamer/gstbufferlist.html?gi-language=c#GstBufferList
      ////gsize gst_buffer_list_calculate_size (GstBufferList * list)
      /// guint gst_buffer_list_length (GstBufferList * list)
      //GstBufferList *gst_sample_get_buffer_list(GstSample * sample)
      //GstBuffer *gst_buffer_list_get (GstBufferList * list, guint idx)
      buffer = gst_sample_get_buffer(sample);

      /////buffer = gst_buffer_new_allocate(NULL, 614400, NULL);
      if(buffer != NULL) {
            ++bufCnt;
            printf ("  BUFFER NOT NULL %d\n\n\n", bufCnt);
            GstMapInfo mapInfo;
            printf("mapInfo.size=%d\n",mapInfo.size);
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);


            //gst_app_src_push_buffer(GST_APP_SRC(source2),buffer);
            //TODO: use it? GstFlowReturn ret; g_signal_emit_by_name (data->app_source, "push-buffer", buffer, &ret);


            //push_buffer_gray8 (source2);
            //push_buffer_rgb16 (source2);
            push_buffer_copy(source2, buffer);


            //do not need reference to the buffer here any more
            gst_buffer_unmap(buffer, &mapInfo);gst_sample_unref(sample);
      } else {
            printf ("BUFFER IS NULL \n\n\n");
      }
    }
  }



  //// Wait until error or EOS
  msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Free resources */
  if (msg != NULL)
    gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}
