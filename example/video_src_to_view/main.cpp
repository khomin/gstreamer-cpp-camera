#include "launch.h"
#include "sink/sink_image.h"
#include "source/source_device.h"
#include <thread>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {
        int width = 1920;
        int height = 1200;

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
        auto sinkToImg = std::make_shared<SinkImage>("RGB", width, height, width/5, height/5, 25);

        srcDevice->addSink(sinkToImg);
        sinkToImg->setImage(launch->imageLeft);
        sinkToImg->start();
        srcDevice->start();

        g_main_loop_run (launch->loop);
    });
}
