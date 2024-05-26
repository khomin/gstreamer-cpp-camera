#include "launch.h"
#include "source/source_device.h"
#include "source/source_decode.h"
#include "sink/sink_encode.h"
#include "sink/sink_image.h"
#include <thread>
#include <QStandardPaths>

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
        auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, width, height, 25, 9000, 50));
        auto srcDecode = std::make_shared<SourceDecode>(DecoderConfig::make(CodecType::CodecAvc, width, height, 25, 9000));

        auto sinkToImgLeft = std::make_shared<SinkImage>("RGB", width, height, width/5, height/5, 25);
        auto sinkToImgRight = std::make_shared<SinkImage>("RGB", width, height, width/5, height/5, 25);

        sinkToEncode->setOnEncoded(SinkEncode::OnEncoded([=](uint8_t *data, uint32_t len, uint64_t pts, uint64_t dts) {
            srcDecode->putData(data, len);
        }));

        srcDevice->addSink(sinkToImgLeft);
        srcDevice->addSink(sinkToEncode);
        srcDecode->addSink(sinkToImgRight);

        sinkToImgLeft->setImage(launch->imageLeft);
        sinkToImgRight->setImage(launch->imageRight);

        sinkToImgLeft->start();
        sinkToImgRight->start();
        sinkToEncode->start();
        srcDecode->start();
        srcDevice->start();
        sinkToImgLeft->start();
        sinkToImgRight->start();

        g_main_loop_run (launch->loop);
    });
}
