#include "launch.h"
#include "source/source_device.h"
#include "sink/sink_file.h"
#include <thread>
#include <QStandardPaths>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {
        int width = 1920;
        int height = 1200;

        auto sinkToFile = std::make_shared<SinkFile>(
            width, height, "RGB",
            (QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test.mp4").toLocal8Bit().data()
        );
        auto srcDevice = std::make_shared<SourceDevice>(
            // source resolution
            width, height,
            // framerate
            25,
            // source type
            SourceDevice::SourceDeviceType::Camera1,
            // no options
            SourceDevice::OptionType::None
        );

        srcDevice->addSink(sinkToFile);
        sinkToFile->start();
        srcDevice->start();

        g_main_loop_run (launch->loop);
    });
}
