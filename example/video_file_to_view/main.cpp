#include "launch.h"
#include "sink/sink_image.h"
#include "sink/sink_callback.h"
#include "source/source_file.h"
#include <thread>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {
//        int width = 1920;
//        int height = 1200;

        auto samples = std::vector<std::string>{
//            "/home/khomin/Desktop/test-images/big_buck_bunny.mp4",
//            "/home/khomin/Desktop/test-images/sample-15s.mp4",
//            "/home/khomin/Desktop/test-images/demo.mp4"
            "/home/khomin/Desktop/test-images/demo.mp4",
            "/home/khomin/Desktop/test-images/demo.mp4",
            "/home/khomin/Desktop/test-images/demo.mp4"
        };
        int samplesIndex = 0;

        for(int i=0; i<100; i++) {
            if(samplesIndex >= samples.size()) {
                samplesIndex = 0;
            }
            auto srcFile = std::make_shared<SourceFile>(
                samples[samplesIndex++],
                1920, 1080,
                SourceFile::Type::video,
                false
            );
            auto sinkCallback = std::make_shared<SinkCallback>();

            sinkCallback->setDataCb([=](uint8_t *data, uint32_t len) {
                launch->imageLeft->setImage(1920, 1080, data, len);
            });

            srcFile->addSink(sinkCallback);
            sinkCallback->start();
            srcFile->start();
            // let it run
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            // reset
            srcFile.reset();
            sinkCallback.reset();
        }
        std::cout << "done" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(30));
        std::cout << "done2" << std::endl;
    });
}
