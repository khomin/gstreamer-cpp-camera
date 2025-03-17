#include "launch.h"
#include "sink/sink_image.h"
#include "sink/sink_callback.h"
#include "source/source_video_file.h"
#include <thread>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {
//        int width = 1920;
//        int height = 1200;
//        launch->imageLeft = std::make_shared<ImageProvider>(1920, 1080);
//        launch->imageRight = std::make_shared<ImageProvider>(1920, 1080);

        auto samples = std::vector<std::string>{
//            "/home/khomin/Desktop/test-images/big_buck_bunny.mp4",
//            "/home/khomin/Desktop/test-images/sample-15s.mp4",
//            "/home/khomin/Desktop/test-images/demo.mp4"
            "/home/khomin/Desktop/test-images/demo.mp4",
            "/home/khomin/Desktop/test-images/demo.mp4",
            "/home/khomin/Desktop/test-images/demo.mp4"
        };
        int samplesIndex = 0;

        for(int i=0; i<1000; i++) {
            if(samplesIndex >= samples.size()) {
                samplesIndex = 0;
            }
            auto srcFile = std::make_shared<SourceVideoFile>(
                samples[samplesIndex++],
                1920, 1080,
                30,
                0.0,
                false
            );
            auto sinkCallback = std::make_shared<SinkCallback>();

            sinkCallback->onData([=](uint8_t *data, uint32_t len) {
                launch->imageLeft->setFrame(data, len);
            });

            srcFile->addSink(sinkCallback);
            sinkCallback->start();
            srcFile->start();
            // let it run
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            // reset
            srcFile.reset();
            sinkCallback.reset();
            std::cout << "round " << i << std::endl;
        }
        std::cout << "done" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(30));
        std::cout << "done2" << std::endl;
    });
}
