#ifndef SINK_FILE_H
#define SINK_FILE_H

#include "sink_base.h"
#include <string>
#include <fstream>

class SinkFile : public SinkBase {
public:
    SinkFile(std::string path);
    SinkFile() = delete;
    virtual ~SinkFile();

    void start() override;
    void putSample(GstSample* sample) override;

private:
    GstElement* m_pipe  = NULL;
    std::ofstream m_file;

protected:
    static constexpr auto tag = "SinkFile";
    static constexpr auto cmd = "appsrc name=source_to_out ! videoconvert ! x264enc ! mp4mux ! appsink name=sink_out";
};

#endif
