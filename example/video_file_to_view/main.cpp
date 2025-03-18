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
                "/home/khomin/Desktop/test-images/bot/videos/output_allo038.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo055.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo029.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo031.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo023.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo024.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo020.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo006.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo007.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo016.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo035.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo004.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo050.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo043.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo034.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo032.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo021.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo010.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo012.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output021.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo011.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo000.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo018.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo037.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo025.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo002.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo041.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo044.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo033.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo008.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo053.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo028.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo003.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo049.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo027.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo048.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo046.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo030.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo022.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo019.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo001.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo052.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo014.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo013.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo047.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo015.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo005.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo054.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo039.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo009.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo051.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo017.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo042.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo040.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo026.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo036.mp4",
                "/home/khomin/Desktop/test-images/bot/videos/output_allo045.mp4"
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
