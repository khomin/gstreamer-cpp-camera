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
  utils/stringf.h
  utils/decoder_config.h
  utils/encoder_config.h
  utils/measure.h
  utils/codec_type.h
  qml.qrc
)

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
  set(LIBRARIES ${LIBRARIES}
    -liconv
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libglib-2.0.dylib
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgobject-2.0.dylib
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstapp-1.0.dylib
    /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstreamer-1.0.dylib
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
  #
  # none
  #
endif ()

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
