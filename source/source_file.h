#ifndef SOURCE_WAV_H
#define SOURCE_WAV_H

#include "source/source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>
#include <atomic>

class SourceFile : public SourceBase {
public:
    explicit SourceFile(std::string path, bool loop = false);
    SourceFile() = delete;
    virtual ~SourceFile();

    void start() override;
    void pause() override;
private:
    static gboolean on_bus_cb(GstBus * bus, GstMessage * message, gpointer data);

    bool m_loop = false;
    std::atomic<bool> m_running;

    static constexpr const char* CMD = "filesrc location=%s ! wavparse ! audioconvert ! %s";
    static constexpr auto TAG = "SourceFile: ";
};

#endif // SOURCE_WAV_H
