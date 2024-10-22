#ifndef SOURCE_AUDIO_H
#define SOURCE_AUDIO_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>

class SourceAudio : public SourceBase {
public:
    SourceAudio();
    virtual ~SourceAudio();

    void start() override;
    void pause() override;
private:
    static GstFlowReturn on_sample(GstElement * elt, SourceAudio* data);
    static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

    static constexpr const char* CMD = "%s ! queue leaky=downstream max-size-buffers=100 ! audioconvert ! audioresample ! audio/x-raw,rate=16000,format=S16LE,channels=1,layout=interleaved ! appsink name=sink_out";

    static constexpr auto TAG = "SourceAudio: ";
};

#endif // SOURCE_AUDIO_H
