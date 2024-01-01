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

    static constexpr auto TAG = "SourceAudio: ";
};

#endif // SOURCE_AUDIO_H
