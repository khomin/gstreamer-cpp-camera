cmake_minimum_required(VERSION 3.14)

set(PROJECT test2)
project(${PROJECT} VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (USE_QT)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
  find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)
  add_definitions(-DUSE_QT)
endif ()

set(PROJECT_SOURCES
  main.cpp
  # sources
  source/source_base.cpp
  source/source_base.h
  source/source_device.h
  source/source_device.cpp
  source/source_app.cpp
  source/source_app.h
  source/source_decode.h
  source/source_decode.cpp
  source/source_audio.cpp
  source/source_audio.h
  # sinks
  sink/sink_base.h
  sink/sink_base.cpp
  sink/sink_image.h
  sink/sink_image.cpp
  sink/sink_file.h
  sink/sink_file.cpp
  sink/sink_encode.h
  sink/sink_encode.cpp
  sink/sink_audio.cpp
  sink/sink_audio.h
  sink/sink_callback.cpp
  sink/sink_callback.h
  # utils
  utils/decoder_config.h
  utils/encoder_config.h
  utils/measure.h
  utils/codec_type.h
  utils/ivideo_device_platform.h
  utils/source_size.h
  config.h
)

if (USE_QT)
  message("Using Qt")
  set(PROJECT_SOURCES ${PROJECT_SOURCES}
    qml.qrc
    # image
    image_provider/image_provider.cpp
    image_provider/image_provider.h
    image_provider/live_image.cpp
    image_provider/live_image.h
  )
else ()
  message("Not using Qt")
  set(PROJECT_SOURCES ${PROJECT_SOURCES}
    image_provider/image_videosink.cpp
    image_provider/image_videosink.h
    utils/ivideo_device_platform.cpp
    utils/ivideo_device_platform.h
  )
endif ()

if (USE_QT)
  if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT}
      MANUAL_FINALIZATION
      ${PROJECT_SOURCES}
    )
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

if (USE_QT)
  set_target_properties(${PROJECT} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
  )
endif ()

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
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/*.a
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/*.a
  )
  list(REMOVE_ITEM GSTREAMER_LIBS "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopencore-amrwb.a")

  set(LIBRARIES ${LIBRARIES}
    -liconv
    ${GSTREAMER_LIBS}
    "-framework Cocoa"
    "-framework Foundation"
    "-framework ScreenCaptureKit"
    "-framework AVFoundation"
    "-framework AudioToolbox"
    "-framework VideoToolbox"
    "-framework GLKit"
    "-framework OpenGL"
    "-framework CoreMedia"
    "-framework CoreAudio"
    "-framework CoreVideo"
    "-framework IOSurface"
    "-framework QuartzCore"
    "-framework Metal"
    "-framework IOKit"
  )
elseif (UNIX AND NOT APPLE)
  if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
      set(ARCH_NAME "arm64")
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
      set(ARCH_NAME "x86_64")
    else()
      error("unknown architecture ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    set(GSTREAMER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib_pack/gstreamer/android/)
    set(LIBRARIES_INCLUDE
      ${GSTREAMER_ROOT_PATH}/${ARCH_NAME}/lib/glib-2.0/include/
      ${GSTREAMER_ROOT_PATH}/${ARCH_NAME}/include/gstreamer-1.0/
      ${GSTREAMER_ROOT_PATH}/${ARCH_NAME}/include/glib-2.0/glib/
      ${GSTREAMER_ROOT_PATH}/${ARCH_NAME}/include/glib-2.0/
      ${GSTREAMER_ROOT_PATH}/${ARCH_NAME}/include/
      )
    file(GLOB_RECURSE GSTREAMER_LIBS
      ${GSTREAMER_ROOT_PATH}/${ARCH_NAME}/*.a
    )
    set(LIBRARIES ${LIBRARIES}
      GLESv2 EGL -lOpenSLES
      ${GSTREAMER_LIBS}
    )
  else()
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
  endif()
elseif (WIN32)
  if (NOT DEFINED GSTREAMER_PATH)
    set(GSTREAMER_PATH "E:/gstreamer/1.0/mingw_x86_64/")
  endif ()
  set(LIBRARIES_INCLUDE
    ${GSTREAMER_PATH}/include/gstreamer-1.0
    ${GSTREAMER_PATH}/include/gstreamer-1.0/gst
    ${GSTREAMER_PATH}/include/glib-2.0
    ${GSTREAMER_PATH}/lib/glib-2.0/include
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
  )
endif ()

if (USE_QT)
  target_link_libraries(${PROJECT} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    ${LIBRARIES}
  )
else ()
  target_link_libraries(${PROJECT} PRIVATE
    ${LIBRARIES}
  )
endif ()

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

if (USE_QT)
  if (QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT})
    qt_finalize_executable(${PROJECT})
  endif ()
endif ()
