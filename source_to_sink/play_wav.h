#ifndef SOURCE_WAV_H
#define SOURCE_WAV_H

#include "source/source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>

class PlayWav : public SourceBase {
public:
    explicit PlayWav(std::string path, bool loop = false);
    PlayWav() = delete;
    virtual ~PlayWav();

    void start() override;
    void pause() override;
private:
    static gboolean on_bus_cb(GstBus * bus, GstMessage * message, gpointer data);

    bool m_loop = false;

    static constexpr const char* CMD = "filesrc location=%s ! wavparse ! audioconvert ! %s";
    static constexpr const char* CMD_SINK_DESKTOP = "autoaudiosink";
    static constexpr const char* CMD_SINK_ANDROID = "openslessink";
    static constexpr auto TAG = "SourceAudio: ";
};

#endif // SOURCE_AUDIO_H
