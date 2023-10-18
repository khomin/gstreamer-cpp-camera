#include "source_decode.h"
#include "utils/stringf.h"
#include "utils/measure.h"
#include <iostream>

GstFlowReturn on_sample (GstElement * elt, SourceDecode* data);

SourceDecode::SourceDecode(DecoderConfig config) {
    auto cmdf = StringFormatter::format(cmd,
                                       config.codecInVideo.c_str(),
                                       config.pixelFormat.c_str(),
                                       config.width, config.height,
                                       config.framerate,
                                       config.bitrate,
                                       config.decoder.c_str());
//    auto cmdf = std::string("appsrc name=source_to_decode ! h264parse ! avdec_h264 ! videoconvert ! queue ! appsink name=sink_out");//osxvideosink");
    m_pipe = gst_parse_launch(cmdf.c_str(), NULL);
    if (m_pipe == NULL) {
        std::cerr << "not all elements created" << std::endl;
    }
}

SourceDecode::~SourceDecode() {
    std::cout << "SourceDecode deleted" << std::endl;
}

void SourceDecode::start() {
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_decode");
    if (source == NULL) {
        std::cout << "value is NULL" << std::endl;
    }
    g_object_set(
        source,
        "is-live", TRUE,
        "stream-type", GstAppStreamType::GST_APP_STREAM_TYPE_STREAM,
        "format", GST_FORMAT_TIME,
        "do-timestamp", TRUE,
        "min-latency", 0,
//        "max-latency", 0,
//        "max-bytes", 10000,
        NULL
    );
    gst_object_unref (source);

    auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
    if (sink_out == NULL) {
        std::cout << "value is NULL" << std::endl;
    } else {
        g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
        g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), this);
        gst_object_unref (sink_out);
    }
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
}

void SourceDecode::pause() {}

void SourceDecode::stop() {}

void SourceDecode::putDataToDecode(uint8_t* data, uint32_t len) {
    GstBuffer *buffer = gst_buffer_new_and_alloc(len);
    gst_buffer_fill(buffer, 0, data, len);

//    GstClockTime ts = gst_element_get_current_running_time(elt);
//    GST_BUFFER_PTS(buffer) = QDateTime::currentMSecsSinceEpoch() / 1000;
//    GST_BUFFER_DTS(buffer) = QDateTime::currentMSecsSinceEpoch() / 1000;

    auto source_to_out = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_decode");
    auto ret = gst_app_src_push_buffer(GST_APP_SRC (source_to_out), buffer);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source_to_out);

    Measure::instance()->onDecodePutSample();
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

//        GstClockTime ts = gst_element_get_current_running_time(elt);
//        GST_BUFFER_PTS(buffer) = ts + 2000000;
//        GST_BUFFER_DTS(buffer) = ts + 2000000;

        Measure::instance()->onDecodeSampleReady();

        for(auto & it : data->m_sinks) {
            it->putSample(sample);
        }
        gst_buffer_unmap(buffer, &mapInfo);
        gst_sample_unref(sample);
    } else {
        printf ("BUFFER IS NULL \n\n\n");
    }
    return ret;
}
