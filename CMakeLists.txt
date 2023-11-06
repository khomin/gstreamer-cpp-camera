cmake_minimum_required(VERSION 3.14)

set(PROJECT test2)

project(${PROJECT} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

set(PROJECT_SOURCES
  main.cpp
  # sources
  source/source_base.cpp
  source/source_base.h
  source/source_device.h
  source/source_device.cpp
  source/source_decode.h
  source/source_decode.cpp
  # sinks
  sink/sink_base.h
  sink/sink_base.cpp
  sink/sink_image.h
  sink/sink_image.cpp
  sink/sink_file.h
  sink/sink_file.cpp
  sink/sink_encode.h
  sink/sink_encode.cpp
  # image
  image_provider/image_provider.cpp
  image_provider/image_provider.h
  image_provider/live_image.cpp
  image_provider/live_image.h
  # utils
  utils/decoder_config.h
  utils/encoder_config.h
  utils/measure.h
  utils/codec_type.h
  qml.qrc
)

add_subdirectory(utils/fmt)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(${PROJECT}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )
  # Define target properties for Android with Qt 6 as:
  #    set_property(TARGET ${PROJECT} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
  # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else ()
  if (ANDROID)
    add_library(${PROJECT} SHARED
      ${PROJECT_SOURCES}
    )
  else ()
    add_executable(${PROJECT}
      ${PROJECT_SOURCES}
    )
  endif ()
endif ()

set_target_properties(${PROJECT} PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)

if (IOS)
  #
  # none
  #
elseif (APPLE)
  set(LIBRARIES_INCLUDE
    /Library/Frameworks/GStreamer.framework/Versions/1.0/include
    /Library/Frameworks/GStreamer.framework/Versions/1.0/Headers
  )
  file(GLOB_RECURSE GSTREAMER_LIBS
    /Library/Frameworks/GStreamer.framework/Versions/1.0/*.a
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/*.a
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/*.a
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gio/modules/*.a
  )

  set(LIBRARIES ${LIBRARIES}
    -liconv
    ${GSTREAMER_LIBS}
    "-framework Cocoa"
    "-framework Security"
    "-framework Foundation"
    "-framework ServiceManagement"
    "-framework ScreenCaptureKit"
    "-framework AVFoundation"
    "-framework AudioToolbox"
    "-framework AGL"
    "-framework VideoToolbox"
    "-framework GLKit"
    "-framework OpenGL"
    "-framework CoreMedia"
    "-framework CoreVideo"
    "-framework IOSurface"
    "-framework QuartzCore"
    "-framework Metal"
    "-framework MetalKit"
    "-framework MetalFX"
    "-framework IOKit"
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
  )
  set(LIBRARIES_INCLUDE
    /usr/lib/x86_64-linux-gnu/glib-2.0/include
    /usr/include/glib-2.0
    /usr/local/include/gstreamer-1.0
    /usr/include/gstreamer-1.0/
  )
elseif (WIN32)
  if (NOT DEFINED GSTREAMER_PATH)
    set(GSTREAMER_PATH "E:/gstreamer/1.0/mingw_x86_64/bin")
  endif ()
  set(LIBRARIES_INCLUDE
    ${GSTREAMER_PATH}/include/gstreamer-1.0
    ${GSTREAMER_PATH}/include/gstreamer-1.0/gst
    ${GSTREAMER_PATH}/include/glib-2.0
    ${GSTREAMER_PATH}/lib/glib-2.0/include
  )
  set(LIBRARIES ${LIBRARIES}
    -liphlpapi
    -lwinmm
    -limm32
    -lws2_32 -liphlpapi -lpsapi -lIphlpapi
    -static-libgcc -static-libstdc++
    ${GSTREAMER_PATH}/libass-9.dll
    ${GSTREAMER_PATH}/libbz2.dll
    ${GSTREAMER_PATH}/libcairo-2.dll
    ${GSTREAMER_PATH}/libcairo-gobject-2.dll
    ${GSTREAMER_PATH}/libcairo-script-interpreter-2.dll
    ${GSTREAMER_PATH}/libcharset-1.dll
    ${GSTREAMER_PATH}/libcroco-0.6-3.dll
    ${GSTREAMER_PATH}/libcrypto-1_1-x64.dll
    ${GSTREAMER_PATH}/libdav1d.dll
    ${GSTREAMER_PATH}/libdv-4.dll
    ${GSTREAMER_PATH}/libexpat-1.dll
    ${GSTREAMER_PATH}/libffi-7.dll
    ${GSTREAMER_PATH}/libFLAC-8.dll
    ${GSTREAMER_PATH}/libfontconfig-1.dll
    ${GSTREAMER_PATH}/libfreetype-6.dll
    ${GSTREAMER_PATH}/libfribidi-0.dll
    ${GSTREAMER_PATH}/libgcc_s_seh-1.dll
    ${GSTREAMER_PATH}/libgdk_pixbuf-2.0-0.dll
    ${GSTREAMER_PATH}/libgio-2.0-0.dll
    ${GSTREAMER_PATH}/libglib-2.0-0.dll
    ${GSTREAMER_PATH}/libgmodule-2.0-0.dll
    ${GSTREAMER_PATH}/libgobject-2.0-0.dll
    ${GSTREAMER_PATH}/libgraphene-1.0-0.dll
    ${GSTREAMER_PATH}/libgstadaptivedemux-1.0-0.dll
    ${GSTREAMER_PATH}/libgstallocators-1.0-0.dll
    ${GSTREAMER_PATH}/libgstapp-1.0-0.dll
    ${GSTREAMER_PATH}/libgstaudio-1.0-0.dll
    ${GSTREAMER_PATH}/libgstbadaudio-1.0-0.dll
    ${GSTREAMER_PATH}/libgstbase-1.0-0.dll
    ${GSTREAMER_PATH}/libgstbasecamerabinsrc-1.0-0.dll
    ${GSTREAMER_PATH}/libgstcheck-1.0-0.dll
    ${GSTREAMER_PATH}/libgstcodecparsers-1.0-0.dll
    ${GSTREAMER_PATH}/libgstcodecs-1.0-0.dll
    ${GSTREAMER_PATH}/libgstcontroller-1.0-0.dll
    ${GSTREAMER_PATH}/libgstcuda-1.0-0.dll
    ${GSTREAMER_PATH}/libgstd3d11-1.0-0.dll
    ${GSTREAMER_PATH}/libgstfft-1.0-0.dll
    ${GSTREAMER_PATH}/libgstgl-1.0-0.dll
    ${GSTREAMER_PATH}/libgstinsertbin-1.0-0.dll
    ${GSTREAMER_PATH}/libgstisoff-1.0-0.dll
    ${GSTREAMER_PATH}/libgstmpegts-1.0-0.dll
    ${GSTREAMER_PATH}/libgstnet-1.0-0.dll
    ${GSTREAMER_PATH}/libgstpbutils-1.0-0.dll
    ${GSTREAMER_PATH}/libgstphotography-1.0-0.dll
    ${GSTREAMER_PATH}/libgstplay-1.0-0.dll
    ${GSTREAMER_PATH}/libgstplayer-1.0-0.dll
    ${GSTREAMER_PATH}/libgstreamer-1.0-0.dll
    ${GSTREAMER_PATH}/libgstriff-1.0-0.dll
    ${GSTREAMER_PATH}/libgstrtp-1.0-0.dll
    ${GSTREAMER_PATH}/libgstrtsp-1.0-0.dll
    ${GSTREAMER_PATH}/libgstrtspserver-1.0-0.dll
    ${GSTREAMER_PATH}/libgstsctp-1.0-0.dll
    ${GSTREAMER_PATH}/libgstsdp-1.0-0.dll
    ${GSTREAMER_PATH}/libgsttag-1.0-0.dll
    ${GSTREAMER_PATH}/libgsttranscoder-1.0-0.dll
    ${GSTREAMER_PATH}/libgsturidownloader-1.0-0.dll
    ${GSTREAMER_PATH}/libgstvideo-1.0-0.dll
    ${GSTREAMER_PATH}/libgstwebrtc-1.0-0.dll
    ${GSTREAMER_PATH}/libgstwebrtcnice-1.0-0.dll
    ${GSTREAMER_PATH}/libgthread-2.0-0.dll
    ${GSTREAMER_PATH}/libharfbuzz-0.dll
    ${GSTREAMER_PATH}/libiconv-2.dll
    ${GSTREAMER_PATH}/libintl-8.dll
    ${GSTREAMER_PATH}/libjpeg-8.dll
    ${GSTREAMER_PATH}/libjson-glib-1.0-0.dll
    ${GSTREAMER_PATH}/libkate-1.dll
    ${GSTREAMER_PATH}/libltc-11.dll
    ${GSTREAMER_PATH}/libmp3lame-0.dll
    ${GSTREAMER_PATH}/libmpg123-0.dll
    ${GSTREAMER_PATH}/libnice-10.dll
    ${GSTREAMER_PATH}/libogg-0.dll
    ${GSTREAMER_PATH}/liboggkate-1.dll
    ${GSTREAMER_PATH}/libopenh264-6.dll
    ${GSTREAMER_PATH}/libopenjp2.dll
    ${GSTREAMER_PATH}/libopus-0.dll
    ${GSTREAMER_PATH}/liborc-0.4-0.dll
    ${GSTREAMER_PATH}/liborc-test-0.4-0.dll
    ${GSTREAMER_PATH}/libpango-1.0-0.dll
    ${GSTREAMER_PATH}/libpangocairo-1.0-0.dll
    ${GSTREAMER_PATH}/libpangoft2-1.0-0.dll
    ${GSTREAMER_PATH}/libpangowin32-1.0-0.dll
    ${GSTREAMER_PATH}/libpcre2-8-0.dll
    ${GSTREAMER_PATH}/libpixman-1-0.dll
    ${GSTREAMER_PATH}/libpng16-16.dll
    ${GSTREAMER_PATH}/libpsl-5.dll
    ${GSTREAMER_PATH}/librsvg-2-2.dll
    ${GSTREAMER_PATH}/libsbc-1.dll
    ${GSTREAMER_PATH}/libSoundTouch-1.dll
    ${GSTREAMER_PATH}/libsoup-2.4-1.dll
    ${GSTREAMER_PATH}/libspandsp-2.dll
    ${GSTREAMER_PATH}/libspeex-1.dll
    ${GSTREAMER_PATH}/libsqlite3-0.dll
    ${GSTREAMER_PATH}/libsrt.dll
    ${GSTREAMER_PATH}/libsrtp2-1.dll
    ${GSTREAMER_PATH}/libssl-1_1-x64.dll
    ${GSTREAMER_PATH}/libstdc++-6.dll
    ${GSTREAMER_PATH}/libtag.dll
    ${GSTREAMER_PATH}/libtheora-0.dll
    ${GSTREAMER_PATH}/libtheoradec-1.dll
    ${GSTREAMER_PATH}/libtheoraenc-1.dll
    ${GSTREAMER_PATH}/libtiff-5.dll
    ${GSTREAMER_PATH}/libturbojpeg-0.dll
    ${GSTREAMER_PATH}/libvorbis-0.dll
    ${GSTREAMER_PATH}/libvorbisenc-2.dll
    ${GSTREAMER_PATH}/libvorbisfile-3.dll
    ${GSTREAMER_PATH}/libwavpack.dll
    ${GSTREAMER_PATH}/libwebrtc_audio_processing-0.dll
    ${GSTREAMER_PATH}/libwinpthread-1.dll
    ${GSTREAMER_PATH}/libxml2-2.dll
    ${GSTREAMER_PATH}/libz-1.dll
    ${GSTREAMER_PATH}/libzbar-0.dll
  )
endif ()

# common libraries (all platforms)
set(LIBRARIES_INCLUDE ${LIBRARIES_INCLUDE}
  utils/fmt/include
)
set(LIBRARIES ${LIBRARIES}
  fmt
)

target_link_libraries(${PROJECT} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Quick
  ${LIBRARIES}
)

# macos need info.plist
if (APPLE)
  set_target_properties(${PROJECT} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
  )
endif ()

target_include_directories(${PROJECT} PUBLIC
  ./
  ./source
  ./sink
  ./image_provider
  ./config
  ${LIBRARIES_INCLUDE}
)

install(TARGETS ${PROJECT}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (QT_VERSION_MAJOR EQUAL 6)
  qt_import_qml_plugins(${PROJECT})
  qt_finalize_executable(${PROJECT})
endif ()
