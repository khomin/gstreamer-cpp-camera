cmake_minimum_required(VERSION 3.14)

set(PROJECT play_wav)
project(${PROJECT} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

add_subdirectory(../ MyVariables)

qt_add_executable(${PROJECT}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

if (APPLE AND IOS)
    #
    # not supported
    #
elseif (APPLE)
    set(LIBRARIES_INCLUDE ${LIBRARIES_INCLUDE}
      /Library/Frameworks/GStreamer.framework/Versions/1.0/include
      /Library/Frameworks/GStreamer.framework/Versions/1.0/Headers
    )
    file(GLOB_RECURSE GSTREAMER_LIBS
      /Library/Frameworks/GStreamer.framework/Versions/1.0/*.a
      /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/*.a
      /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/*.a
      /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gio/modules/*.a
    )
    list(REMOVE_ITEM GSTREAMER_LIBS
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopencore-amrwb.a"
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libavcodec.a"
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libavdevice.a"
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libavfilter.a"
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libavformat.a"
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libavutil.a"
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libswresample.a"
        "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libswscale.a"
    )
    set(LIBRARIES ${LIBRARIES}
        ${GSTREAMER_LIBS}
    )
elseif (ANDROID OR __ANDROID__)
    #
    # not supported
    #
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(SOURCES ${SOURCES}
        ./api/api_lib.cpp
        ./api/api_lib.h
        ./api/desktop/desktop_lib.cpp
        ./api/desktop/desktop_lib.h
        ./local_notification/local_notification.cpp
        ./local_notification/local_notification.h
        ./local_notification/notification_interface.h
    )
    set(LIBRARIES_INCLUDE ${LIBRARIES_INCLUDE}
        /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /usr/include/gstreamer-1.0/
        /usr/include/glib-2.0/glib/
        /usr/include/glib-2.0/
        /usr/include/
    )
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB REQUIRED glib-2.0)
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
    pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)
    set(LIBRARIES ${LIBRARIES}
        -lpthread
        -lz
        -lX11
        ${GST_APP_LIBRARIES}
        ${GST_VIDEO_LIBRARIES}
    )
elseif (WIN32)
    # not supported
endif ()

target_link_libraries(${PROJECT} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    ${LIBRARIES}
)

if (APPLE)
    # macos need info.plist
    set_target_properties(${PROJECT} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
    set_target_properties(${PROJECT} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
endif ()

target_include_directories(${PROJECT} PUBLIC
    ${LIBRARIES_INCLUDE}
)
