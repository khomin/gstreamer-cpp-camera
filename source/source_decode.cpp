#include "source_decode.h"
#include <iostream>

GstFlowReturn on_sample (GstElement * elt, SourceDecode* data);

SourceDecode::SourceDecode() : SourceDecode(SourceDecodeType::Todo1) {}

static void
cb_need_data2 (GstElement *appsrc,
          guint unused_size,
          gpointer user_data)
{
  printf("APPSRC NEEDS DATA!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

SourceDecode::SourceDecode(SourceDecodeType type) : m_type(type) {
//    if(m_type == SourceDecodeType::Todo1) {
//    } else if(m_type == SourceDecodeType::Todo2) {}
    m_pipe = gst_parse_launch(cmd, NULL);
    if (m_pipe == NULL) {
        std::cout << "not all elements created" << std::endl;
    }

    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_decode");
    if (source == NULL) {
        std::cout << "value is NULL" << std::endl;
    }
    g_object_set (source, "format", GST_FORMAT_TIME, NULL);
    g_object_set (G_OBJECT (source),
            "stream-type", 0,
            "format", GST_FORMAT_TIME, NULL);
      g_signal_connect (source, "need-data", G_CALLBACK (cb_need_data2), NULL);

    gst_object_unref (source);


//    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
//    if (sink_out == NULL) {
//        std::cout << "value is NULL" << std::endl;
//    }
//    g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
//    g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), this);
//    gst_object_unref (sink_out);
}

void SourceDecode::start() {
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
}

void SourceDecode::pause() {}

void SourceDecode::stop() {}

void SourceDecode::putData(uint8_t* data, uint32_t len) {
    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);

    std::chrono::nanoseconds ns = std::chrono::high_resolution_clock::now().time_since_epoch();
    buffer->pts = ns.count();
//1947998612670
//1970464120096
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_decode");
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);
}

void SourceDecode::putSample(GstSample* sample) {
    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_decode");

    auto sample2 = gst_sample_copy(sample);

    auto ret = gst_app_src_push_sample (GST_APP_SRC (source_to_out), sample2);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref(source_to_out);

//    GstMapInfo mapInfo;
//    auto bufferIn = gst_sample_get_buffer(sample);
//    gst_buffer_map(bufferIn, &mapInfo, GST_MAP_READ);

////    GstBuffer *bufferOut = gst_buffer_new_and_alloc(mapInfo.size);
//    GstBuffer *bufferOut = gst_buffer_new_allocate(NULL, mapInfo.size, NULL);
//    gst_buffer_fill(bufferOut, 0, (uint8_t*)mapInfo.data, mapInfo.size);
//    std::chrono::nanoseconds ns = std::chrono::high_resolution_clock::now().time_since_epoch();
//    bufferOut->pts = ns.count();//bufferIn->pts;//ns.count();
////    bufferOut->dts = bufferIn->dts;//ns.count();
////    bufferOut->duration = bufferIn->duration;//ns.count();


//    GstBuffer* bufferOut = gst_buffer_copy(bufferIn);

//    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_decode");
//    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), bufferOut);
//    if(ret != GST_FLOW_OK) {
//        std::cout << "push_sample error: " << ret  << std::endl;
//    }
//    gst_object_unref (source_to_out);
}

/* called when the appsink notifies us that there is a new buffer ready for processing */
GstFlowReturn on_sample(GstElement * elt, SourceDecode* data) {
    GstSample *sample;
    GstBuffer *app_buffer, *buffer;
    GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

    /* get the sample from appsink */
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample == NULL) {
        return ret;
    }
    buffer = gst_sample_get_buffer(sample);

    if(buffer != NULL) {
        GstMapInfo mapInfo;
        gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

        for(auto & it : data->m_sinks) {
            it->putSample(sample);
        }
        gst_buffer_unmap(buffer, &mapInfo);
        gst_sample_unref(sample);
    } else {
        printf ("BUFFER IS NULL \n\n\n");
    }
    return ret; // return GstFlowReturn::GST_FLOW_OK;
}
