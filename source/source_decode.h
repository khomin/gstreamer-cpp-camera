#ifndef SOURCE_DECODE_H
#define SOURCE_DECODE_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>

class SourceDecode : public SourceBase {
public:
    enum class SourceDecodeType { Todo1, Todo2 };

    SourceDecode(SourceDecodeType type);
    SourceDecode();

    void start() override;
    void pause() override;
    void stop() override;

    void putData(uint8_t* data, uint32_t len);
    void putSample(GstSample* sample);

private:
    SourceDecodeType m_type = SourceDecodeType::Todo1;

//    static constexpr const char* cmd = "appsrc name=source_to_decode ! videoconvert ! video/x-raw,format=RGB ! osxvideosink";
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! h264parse ! avdec_h264 ! appsink name=sink_out";


    // video/x-raw,width=800,height=800,framerate=30/1
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! avdec_h264 ! queue max-size-buffers=300 ! videoconvert ! osxvideosink";//video/x-raw,format=RGB ! appsink name=sink_out"; // width=2880,height=1800 ! video/x-raw,format=Y444,framerate=30/1

//    static constexpr const char* cmd = "appsrc name=source_to_decode ! video/x-h264,width=150,height=100,stream-format=byte-stream,framerate=30/1 ! avdec_h264 ! queue max-size-buffers=5 ! videoconvert ! video/x-raw,format=RGB,width=150,height=100,framerate=30/1 ! videoconvert ! osxvideosink";// ! appsink name=sink_out";//video/x-raw,format=RGB ! appsink name=sink_out";

    // videoconvert ! video/x-raw,format=RGB,width=426,height=240,framerate=30/1 !

    static constexpr const char* cmd = "appsrc name=source_to_decode ! video/x-h264,format=I420,width=426,height=240,stream-format=byte-stream,framerate=24/1,alignment=au ! avdec_h264 ! queue max-size-buffers=5 ! videoconvert ! video/x-raw,format=RGB,width=426,height=240,framerate=24/1 ! videoconvert ! osxvideosink";
//filesrc location=/Users/khominvladimir/Desktop/raw.mp4
    //    static constexpr const char* cmd = "appsrc name=source_to_decode ! video/x-h264,width=2880,height=1800,stream-format=avc,alignment=au ! avdec_h264  ! queue max-size-buffers=5 ! videoconvert ! osxvideosink";// ! appsink name=sink_out";//video/x-raw,format=RGB ! appsink name=sink_out";

    //    static constexpr const char* cmd = "appsrc name=source_to_decode ! h264parse ! avdec_h264 ! videoconvert ! osxvideosink";
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! videoconvert ! video/x-raw,format=RGB ! videoconvert ! appsink name=sink_out";
//    static constexpr const char* cmd = "appsrc caps=video/x-raw,format=RGB name=source_to_decode ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink_out";
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! appsink name=sink_out";
};

#endif // SOURCE_DECODE_H
