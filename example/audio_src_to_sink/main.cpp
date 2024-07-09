#include "launch.h"
#include "source/source_audio.h"
#include "sink/sink_audio.h"
#include "sink/sink_callback.h"
#include <QStandardPaths>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {

        for(int i=0; i<50; i++) {
            auto srcAudio = std::make_shared<SourceAudio>();
            auto sinkAudio = std::make_shared<SinkAudio>();
            auto sinkCallback = std::make_shared<SinkCallback>();
            srcAudio->addSink(sinkCallback);

            sinkCallback->setDataCb([=](uint8_t *data, uint32_t len) {
                sinkAudio->putData(data, len);
            });
            srcAudio->start();
            sinkAudio->start();
            sinkCallback->start();


            std::this_thread::sleep_for(std::chrono::milliseconds(10000));

            srcAudio = nullptr;
            sinkAudio = nullptr;
            sinkCallback = nullptr;
        }
        std::cout << "done" << std::endl;
    });
}
