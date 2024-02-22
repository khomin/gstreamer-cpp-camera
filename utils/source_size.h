#ifndef SOURCE_SIZE_H
#define SOURCE_SIZE_H

#include <gst/gst.h>
#include <vector>
#include <utility>
#include <iostream>

#ifdef __ANDROID__
#include "jni/jni_api.h"
#endif

class SourceSizeInfo {
public:
    typedef struct Size {
        int width{};
        int height{};
        std::string description;
    } Size;

    static std::vector<Size> getSize() {
        std::vector<Size> res;
#ifdef __ANDROID__
        auto instance = AndroidVideoDevicePlatform::instance();
        auto size = instance->getCameraSize("0");
        auto size_ = Size();
        size_.description = "0";
        size_.width = size.first;
        size_.height = size.second;
        res.emplace_back(size_);
        size = instance->getCameraSize("1");
        size_.description = "1";
        size_.width = size.first;
        size_.height = size.second;
        res.emplace_back(size_);
#else
        GstDeviceMonitor *monitor = gst_device_monitor_new();
        if (!gst_device_monitor_start(monitor)) {
            std::cerr << "monitor couldn't start" << std::endl;
            return res;
        }
        GList *devices = gst_device_monitor_get_devices(monitor);
        GList *devIter;
        for (devIter = g_list_first(devices); devIter != nullptr; devIter = g_list_next(devIter)) {
            auto device = (GstDevice *) devIter->data;
            if (device == NULL)
                continue;
            auto caps = gst_device_get_caps((GstDevice *) device);
            if (caps != nullptr) {
                GstStructure *capStr = gst_caps_get_structure(caps, 0);
                auto size = Size();
                gst_structure_get_int(capStr, "width", &size.width);
                gst_structure_get_int(capStr, "height", &size.height);
                std::string capsString = gst_structure_to_string(capStr);
                std::cout << "device caps: " << capsString.c_str() << std::endl;
                gst_caps_unref(caps);
                if (size.width != 0 && size.height) {
                    auto name = gst_device_get_display_name(device);
                    auto prop = gst_device_get_properties(device);
                    std::string path = gst_structure_get_string(prop, "object.path");
#if __APPLE__

#elif _WIN32

#elif defined (__ANDROID__)

#elif defined (__linux__)
                    if (path == "v4l2:/dev/video0") {
                        size.description = "0";
                    } else {
                        size.description = "1";
                    }
#endif
                    g_free(name);
                    gst_structure_free(prop);
                    res.emplace_back(size);
                }
            }
        }
        g_list_free(devices);
        gst_device_monitor_stop(monitor);
#endif
        return res;
    }
};

#endif // SOURCE_SIZE_H