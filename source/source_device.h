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
    enum class SourceDeviceType { Camera1, Camera2, Screen, Test };
    enum class OptionType { None, TimeOverlay };

    explicit SourceDevice(int width, int height, int framerate, SourceDeviceType type, OptionType option = OptionType::None);
    SourceDevice() = delete;
    virtual ~SourceDevice();

    void start() override;
    void pause() override;
    void onConfigChanged(std::function<void(int , int)> cb);

    void putVideoFrame(uint8_t *data, uint32_t len, int width, int height);

    void setDevicePlatformInterface(IVideoDevicePlatform* v);

    std::pair<int,int> getSize();
    SourceDeviceType getType();

private:
    static GstFlowReturn on_sample(GstElement * elt, SourceDevice* data);

    IVideoDevicePlatform* m_device_platform_interface = nullptr;
    std::function<void(int ,int)> m_config_changed = nullptr;
    bool m_first_frame = true;
    SourceDeviceType m_dev_type {};
    int width{}; int height{};

    static constexpr auto TAG = "SourceDevice: ";
};

#endif // SOURCE_DEV_H
