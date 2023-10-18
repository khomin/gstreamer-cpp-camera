#include "sink_encode.h"
#include "utils/stringf.h"
#include "utils/measure.h"
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <iostream>

GstFlowReturn on_sample(GstElement * elt, std::function<void(uint8_t*, uint32_t, uint32_t, uint32_t)>* data);

SinkEncode::SinkEncode(EncoderConfig config) {
    m_config = config;
    std::string cmdf = StringFormatter::format(cmd,
                config.pixelFormat.c_str(),
                config.width, config.height,
                config.framerate,
                (config.codec + " " + config.codecOptions).c_str(),
                config.bitrate
                );
    m_pipe = gst_parse_launch(cmdf.c_str(), NULL);
    if (m_pipe == NULL) {
        std::cerr << "not all elements created" << std::endl;
    }
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    g_object_set(
        source,
        "is-live", TRUE,
        "stream-type", 0,
        "format", GST_FORMAT_TIME,
        "leaky-type", GST_APP_LEAKY_TYPE_DOWNSTREAM,
//        "do-timestamp", TRUE,
//        "min-latency", 0,
//        "max-latency", 0,
//        "max-bytes", 1000,
        NULL
      );
    gst_object_unref (source);
}

SinkEncode::~SinkEncode() {
    m_on_encoded = NULL;
}

void SinkEncode::start() {
    if(m_on_encoded != NULL) {
        auto sink_out = gst_bin_get_by_name (GST_BIN (m_pipe), "sink_out");
        g_object_set (G_OBJECT(sink_out), "emit-signals", TRUE, NULL);
        g_signal_connect (sink_out, "new-sample", G_CALLBACK (on_sample), &m_on_encoded);
        gst_object_unref (sink_out);
    }
    gst_element_set_state (m_pipe, GST_STATE_PLAYING);
}

void SinkEncode::stop() {
    // TODO
}

void SinkEncode::putSample(GstSample* sample) {
    auto source = gst_bin_get_by_name (GST_BIN (m_pipe), "source_to_out");
    auto ret = gst_app_src_push_sample (GST_APP_SRC (source), sample);
    if(ret != GST_FLOW_OK) {
        std::cout << "push_sample error: " << ret  << std::endl;
    }
    gst_object_unref (source);
    Measure::instance()->onEncodePutSample();
}

void SinkEncode::setOnEncoded(std::function<void(uint8_t*, uint32_t, uint32_t, uint32_t)> cb) {
    m_on_encoded = cb;
}

static GstClockTime test_pts = 0;

GstFlowReturn on_sample(GstElement * elt, std::function<void(uint8_t*, uint32_t, uint32_t, uint32_t)>* data) {
    GstSample *sample;
    GstBuffer *app_buffer, *buffer;
    GstElement *source;
    GstFlowReturn ret = GstFlowReturn::GST_FLOW_OK;

//    std::cout << "btest: HOHOOH: " << std::endl;

    /* get the sample from appsink */
    sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));

    if(sample != NULL) {
        buffer = gst_sample_get_buffer(sample);

        if(buffer != NULL) {
            GstMapInfo mapInfo;
            gst_buffer_map(buffer, &mapInfo, GST_MAP_READ);

//            GST_BUFFER_PTS(buffer) = QDateTime::currentMSecsSinceEpoch() / 1000;
//            GST_BUFFER_DTS(buffer) = QDateTime::currentMSecsSinceEpoch() / 1000;

//            GstClockTime ts = gst_element_get_current_running_time(elt);
//            GST_BUFFER_PTS(buffer) = ts-10000;
//            GST_BUFFER_DTS(buffer) = ts-10000;

            Measure::instance()->onEncodeSampleReady();

            if(data != NULL) {
                (*data)((uint8_t*)mapInfo.data, mapInfo.size, buffer->pts, buffer->dts);
            }
            if(test_pts < buffer->pts) {
                test_pts = buffer->pts;
//                std::cout << "btest: LOWER : " << buffer->pts  << std::endl;
            } else {
                std::cout << "btest: HIGHER: " << buffer->pts  << std::endl;
            }
            gst_buffer_unmap(buffer, &mapInfo);
            gst_sample_unref(sample);
        } else {
            std::cerr << "buffer is null" << std::endl;
        }
    }
    return ret;
}
