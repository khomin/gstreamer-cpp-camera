#ifndef SINK_FILE_H
#define SINK_FILE_H

#include "sink_base.h"
#include <string>
#include <fstream>

class SinkFile : public SinkBase {
public:
    explicit SinkFile(int width, int height, std::string format, std::string path);
    SinkFile() = delete;
    virtual ~SinkFile();

    void start() override;
    void pause() override;

    void putSample(GstSample* sample) override;
private:
    static GstFlowReturn on_sample(GstElement * elt, std::ofstream* file);
    std::ofstream m_file;
protected:
    static constexpr auto TAG = "SinkFile: ";
};

#endif
