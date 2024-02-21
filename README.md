# Gstreamer wrappers over
**It uses Appsrc, Appsink**

# How to use
**Let's say you need camera as a source, you want to encode its frames, decode and show on the screen**<br>
**The chain will be:**<br>
```Camera -> Encoder -> Decoder -> Image```
```
auto srcFromWebc = std::make_shared<SourceDevice>(SourceDevice::SourceDeviceType::Camera1, SourceDevice::OptionType::TimeOverlay);
auto sinkToEncode = std::make_shared<SinkEncode>(EncoderConfig::make(CodecType::CodecAvc, 2560 /4,1600 /4, 20, 400));
auto sinkToImg = std::make_shared<SinkImage>(SinkImage::ImageType::Full);

srcFromScreen->addSink(sinkToEncode);
srcDecode->addSink(sinkToImg);

sinkToEncode->setOnEncoded([&](uint8_t* data, uint32_t len, uint32_t pts, uint32_t dts) {
  srcDecode->putData(data, len);
});
sinkToImg->setImage(image);
sinkToImg->start();
sinkToEncode->start();
srcFromScreen->start();
srcDecode->start();

g_main_loop_run (loop);
```

**Qt5 is needed**