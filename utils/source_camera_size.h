#ifndef SOURCE_SIZE_H
#define SOURCE_SIZE_H

#include <gst/gst.h>
#include <vector>
#include <utility>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

class SourceCameraInfo {
public:
    typedef struct Size {
        int width{};
        int height{};
        std::string description;
    } Size;

    static std::vector<Size> getCamerasDesktop() {
        std::vector<Size> res;
        GstDeviceMonitor *monitor = gst_device_monitor_new();
        auto monCaps = gst_caps_new_empty_simple("video/x-raw");
        gst_device_monitor_add_filter(monitor, "Video/Source", monCaps);
        gst_caps_unref(monCaps);
        if (!gst_device_monitor_start(monitor)) {
            std::cerr << "monitor couldn't start" << std::endl;
            return res;
        }
        GList *devices = gst_device_monitor_get_devices(monitor);
        GList *devIter;
        for (devIter = g_list_first(devices); devIter != nullptr; devIter = g_list_next(devIter)) {
            auto device = (GstDevice *) devIter->data;
            if (device == nullptr) continue;
            auto name = gst_device_get_display_name(device);
            auto caps = gst_device_get_caps((GstDevice *) device);
            if (caps != nullptr) {
                GstStructure *cap_struct = gst_caps_get_structure(caps, 0);
                std::string caps_string = gst_structure_to_string(cap_struct);
                std::cout << "device caps: " << caps_string.c_str() << std::endl;
                auto size = Size();
                gst_structure_get_int(cap_struct, "width", &size.width);
                gst_structure_get_int(cap_struct, "height", &size.height);
                auto prop = gst_device_get_properties(device);
#if __APPLE__
                size.description = "1";
#elif _WIN32
                size.description = "1";
#elif defined (__linux__)
                if(prop != nullptr) {
                    std::string path = gst_structure_get_string(prop, "object.path");
                    if (path != "v4l2:/dev/video0") {
                        size.description = "1";
                    }
                }
#endif
                res.emplace_back(size);
                if(prop != nullptr) gst_structure_free(prop);
                gst_caps_unref(caps);
            }
            g_free(name);
        }
        g_list_free(devices);
        gst_device_monitor_stop(monitor);
        return res;
    }
};

#endif // SOURCE_SIZE_H
