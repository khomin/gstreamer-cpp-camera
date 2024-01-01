#ifndef SOURCE_DECODE_H
#define SOURCE_DECODE_H

#include "source_base.h"
#include "utils/decoder_config.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>

class SourceDecode : public SourceBase {
public:
    explicit SourceDecode(DecoderConfig config);
    SourceDecode() = delete;
    virtual ~SourceDecode();

    void start() override;
    void pause() override;

    void putDataToDecode(uint8_t* data, uint32_t len);

private:
    static GstFlowReturn on_sample(GstElement * elt, SourceDecode* data);

    static constexpr auto TAG = "SourceDecode: ";
    static constexpr const char* CMD = "appsrc name=source_to_decode ! video/%s,format=%s,width=%d,height=%d,stream-format=byte-stream,framerate=%d/1,alignment=au,bitrate=%d ! %s ! videoconvert ! video/x-raw,format=RGBA ! queue ! appsink name=sink_out max-buffers=1 drop=true";
};

#endif // SOURCE_DECODE_H
