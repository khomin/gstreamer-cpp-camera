#include "launch.h"
#include "source/source_audio.h"
#include "sink/sink_audio.h"
#include "sink/sink_callback.h"
#include <thread>
#include <QStandardPaths>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {
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

        g_main_loop_run (launch->loop);
    });
}
