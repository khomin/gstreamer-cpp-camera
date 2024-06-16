cmake_minimum_required(VERSION 3.14)

set(PROJECT_SOURCES
    ./main.cpp
    ../launch.cpp
    ../launch.h
    ../control.h
    ../qml.qrc
    # image
    ../../image_provider/image_provider.cpp
    ../../image_provider/image_provider.h
    ../../image_provider/live_image.cpp
    ../../image_provider/live_image.h
    ../../image_provider/image_videosink.cpp
    ../../image_provider/image_videosink.h
    ../../source/source_base.cpp
    ../../source/source_base.h
    ../../source/source_device.h
    ../../source/source_device.cpp
    ../../source/source_app.cpp
    ../../source/source_app.h
    ../../source/source_decode.h
    ../../source/source_decode.cpp
    ../../source/source_audio.cpp
    ../../source/source_audio.h
    # sinks
    ../../sink/sink_base.h
    ../../sink/sink_base.cpp
    ../../sink/sink_image.h
    ../../sink/sink_image.cpp
    ../../sink/sink_file.h
    ../../sink/sink_file.cpp
    ../../sink/sink_encode.h
    ../../sink/sink_encode.cpp
    ../../sink/sink_audio.cpp
    ../../sink/sink_audio.h
    ../../sink/sink_callback.cpp
    ../../sink/sink_callback.h
    # utils
    ../../utils/decoder_config.h
    ../../utils/encoder_config.h
    ../../utils/measure.h
    ../../utils/codec_type.h
    ../../utils/source_camera_size.h
    ../../utils/gst_static_plugins.cpp
    ../../utils/gst_static_plugins.h
    # source-sink
    ../../source_to_sink/play_wav.cpp
    ../../source_to_sink/play_wav.h
    ../../config.h
    CACHE INTERNAL ""
)

set(LIBRARIES_INCLUDE
    ./
    ../
    ../../
    ../../source
    ../../sink
    ../../image_provider
    ../../config
    CACHE INTERNAL ""
)

if (APPLE)
    set(LIBRARIES_INCLUDE ${LIBRARIES_INCLUDE}
      /Library/Frameworks/GStreamer.framework/Versions/1.0/include
      /Library/Frameworks/GStreamer.framework/Versions/1.0/Headers
      CACHE INTERNAL ""
    )
    file(GLOB_RECURSE GSTREAMER_LIBS
      /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/*.a
      /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/*.a
      CACHE INTERNAL ""
    )
    set(LIBRARIES
        ${GSTREAMER_LIBS}
        -liconv
        "-framework Cocoa"
        "-framework Foundation"
        "-framework AVFoundation"
        "-framework AudioToolbox"
        "-framework VideoToolbox"
        "-framework GLKit"
        "-framework OpenGL"
        "-framework OpenCL"
        "-framework CoreAudio"
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework IOSurface"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework IOKit"
        CACHE INTERNAL ""
    )
elseif (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB REQUIRED glib-2.0)
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
    pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)
    set(LIBRARIES
        ${GST_APP_LIBRARIES}
        ${GST_VIDEO_LIBRARIES}
        -lxcb -ldl -pthread
        CACHE INTERNAL ""
    )
    set(LIBRARIES_INCLUDE ${LIBRARIES_INCLUDE}
        /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /usr/include/glib-2.0
        /usr/local/include/gstreamer-1.0
        /usr/include/gstreamer-1.0/
        CACHE INTERNAL ""
    )
elseif (WIN32)
    if (NOT DEFINED GSTREAMER_PATH)
        set(GSTREAMER_PATH "E:/gstreamer/1.0/mingw_x86_64/")
    endif ()
    set(LIBRARIES_INCLUDE ${LIBRARIES_INCLUDE}
        ${GSTREAMER_PATH}/include/gstreamer-1.0
        ${GSTREAMER_PATH}/include/gstreamer-1.0/gst
        ${GSTREAMER_PATH}/include/glib-2.0
        ${GSTREAMER_PATH}/lib/glib-2.0/include
        CACHE INTERNAL ""
    )
    file(GLOB_RECURSE GSTREAMER_LIBS
        ${GSTREAMER_PATH}/lib/*.a
        ${GSTREAMER_PATH}/lib/gstreamer-1.0/*.a
    )
    list(REMOVE_ITEM GSTREAMER_LIBS "${GSTREAMER_PATH}/lib/libstdc++.a")
    list(REMOVE_ITEM GSTREAMER_LIBS "${GSTREAMER_PATH}/lib/libmingw32.a")
    set(LIBRARIES ${LIBRARIES}
        -liphlpapi
        -lwinmm
        -limm32
        -lws2_32 -liphlpapi -lpsapi -lIphlpapi
        -static-libgcc -static-libstdc++
        ${GSTREAMER_LIBS}
        CACHE INTERNAL ""
    )
endif ()
