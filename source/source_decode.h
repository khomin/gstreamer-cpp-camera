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

//    static constexpr const char* cmd = "appsrc name=source_to_decode ! h264parse ! avdec_h264 ! appsink name=sink_out";
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! avdec_h264 ! videoconvert ! osxvideosink";//video/x-raw,format=RGB ! appsink name=sink_out";
    static constexpr const char* cmd = "appsrc name=source_to_decode ! h264parse ! avdec_h264 ! videoconvert ! osxvideosink";
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! videoconvert ! video/x-raw,format=RGB ! videoconvert ! appsink name=sink_out";
//    static constexpr const char* cmd = "appsrc caps=video/x-raw,format=RGB name=source_to_decode ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink_out";
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! appsink name=sink_out";
};

#endif // SOURCE_DECODE_H
