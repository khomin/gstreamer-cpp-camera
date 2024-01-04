#ifndef SOURCE_AUDIO_H
#define SOURCE_AUDIO_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>

class SourceAudio : public SourceBase {
public:
    SourceAudio();
    virtual ~SourceAudio();

    void start() override;
    void pause() override;
private:
    static GstFlowReturn on_sample(GstElement * elt, SourceAudio* data);

    static constexpr const char* CMD = "%s ! audioconvert ! audioresample ! audio/x-raw,rate=16000,format=S16LE,channels=1,layout=interleaved ! appsink name=sink_out";
    static constexpr const char* CMD_DESKTOPS = "autoaudiosrc";
    static constexpr const char* CMD_ANDROID = "openslessrc";

    static constexpr auto TAG = "SourceAudio: ";
};

#endif // SOURCE_AUDIO_H
