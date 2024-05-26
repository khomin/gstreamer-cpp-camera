#ifndef LAUNCH_H_
#define LAUNCH_H_

#include "image_provider/image_provider_abstract.h"
#include "image_provider/live_image.h"
#include "image_provider/image_provider.h"
#include "image_provider/image_videosink.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/app.h>
#include <gst/video/video.h>
#include <gst/gstplugin.h>
#include <iostream>
#include <thread>
#include <memory>
#include <functional>

class Launch {
public:
    Launch();
    int runLoop(int argc, char *argv[], std::function<void()> v);
    
    std::shared_ptr<ImageProvider> imageLeft;
    std::shared_ptr<ImageProvider> imageRight;
    GMainLoop* loop;
};

#endif
