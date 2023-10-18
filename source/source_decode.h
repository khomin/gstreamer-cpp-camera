#ifndef SOURCE_DECODE_H
#define SOURCE_DECODE_H

#include "source_base.h"
#include "utils/decoder_config.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>

class SourceDecode : public SourceBase {
public:
    SourceDecode(DecoderConfig config);
    SourceDecode() = delete;
    ~SourceDecode();

    void start() override;
    void pause() override;
    void stop() override;

    void putDataToDecode(uint8_t* data, uint32_t len);

private:
//    static constexpr const char* cmd = "appsrc name=source_to_decode ! video/%s,format=%s,width=%d,height=%d,stream-format=byte-stream,framerate=%d/1,alignment=au,bitrate=%d ! %s ! videoconvert ! queue ! osxvideosink";
static constexpr const char* cmd = "appsrc name=source_to_decode ! video/%s,format=%s,width=%d,height=%d,stream-format=byte-stream,framerate=%d/1,alignment=au,bitrate=%d ! %s ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink_out drop=true";
//static constexpr const char* cmd = "appsrc name=source_to_decode ! video/x-h264,format=I420,width=426,height=240,stream-format=byte-stream,framerate=24/1,alignment=au ! avdec_h264 ! videoconvert ! video/x-raw,format=RGB ! queue ! appsink name=sink_out";
};

#endif // SOURCE_DECODE_H
