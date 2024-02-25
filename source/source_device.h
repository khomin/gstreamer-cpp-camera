#ifndef SOURCE_DEV_H
#define SOURCE_DEV_H

#include "source_base.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/app.h>
#include <functional>
#include <utils/ivideo_device_platform.h>

class SourceDevice : public SourceBase {
public:
    enum class SourceDeviceType { Idle, Camera1, Camera2, Screen, Test };
    enum class OptionType { None, TimeOverlay };

    explicit SourceDevice(int width, int height, int framerate, SourceDeviceType type, OptionType option = OptionType::None);
    SourceDevice() = delete;
    virtual ~SourceDevice();

    void start() override;
    void pause() override;

    void putVideoFrame(uint8_t *data, uint32_t len, int width, int height);

    std::pair<int,int> getSize();
    SourceDeviceType getType();

private:
    static GstFlowReturn on_sample(GstElement * elt, void* data);
    SourceDeviceType m_dev_type {};
    int width{}; int height{};

    static constexpr auto TAG = "SourceDevice: ";
};

#endif // SOURCE_DEV_H
