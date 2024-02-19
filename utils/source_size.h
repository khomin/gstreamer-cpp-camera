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
        auto size = instance->getCameraSize("1");
        auto size_ = Size();
        size_.description = "1";
        size_.width = size.first;
        size_.height = size.second;
        res.emplace_back(size_);
        size = instance->getCameraSize("2");
        size_.description = "2";
        size_.width = size.first;
        size_.height = size.second;
        res.emplace_back(size_);
#else
        GstDeviceMonitor* monitor= gst_device_monitor_new();
        if(!gst_device_monitor_start(monitor)){
            std::cerr << "WARNING: Monitor couldn't started!!" << std::endl;
        }
        GList* devices = gst_device_monitor_get_devices(monitor);
        for(auto it= devices->data; it!= nullptr; it=devices->next) {
            auto size = Size();
            auto caps = gst_device_get_caps((GstDevice*)it);
            GstStructure *capStr = gst_caps_get_structure(caps, 0);
            gst_structure_get_int(capStr, "width", &size.width);
            gst_structure_get_int(capStr, "height", &size.height);
            gst_caps_unref(caps);
            auto name = gst_device_get_display_name((GstDevice*)it);
            size.description = name;
            res.emplace_back(size);
            g_free(name)
        }
#endif
        return res;
    }
};

#endif // SOURCE_SIZE_H