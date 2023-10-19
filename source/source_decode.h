#ifndef SOURCE_DECODE_H
#define SOURCE_DECODE_H

#include "source_base.h"
#include "utils/decoder_config.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <mutex>

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
    std::mutex m_lock;

    static constexpr auto tag = "SourceDecode";
    static constexpr const char* cmd = "appsrc name=source_to_decode ! video/%s,format=%s,width=%d,height=%d,stream-format=byte-stream,framerate=%d/1,alignment=au,bitrate=%d ! %s ! videoconvert ! video/x-raw,format=RGB ! appsink name=sink_out drop=true";
};

#endif // SOURCE_DECODE_H