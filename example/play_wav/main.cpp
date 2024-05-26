#include "launch.h"
#include "source_to_sink/play_wav.h"
#include <thread>
#include <QStandardPaths>

int main(int argc, char *argv[]) {
    auto launch = std::make_shared<Launch>();
    return launch->runLoop(argc, argv, [=] {

        for(int i=0; i<50; i++) {
            auto playWav = std::make_shared<PlayWav>(
                (QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test.wav").toStdString().c_str(),
                true
            );
            playWav->start();

            std::this_thread::sleep_for(std::chrono::seconds(3));
            playWav = nullptr;
        }
        std::cout << "done" << std::endl;
    });
}
