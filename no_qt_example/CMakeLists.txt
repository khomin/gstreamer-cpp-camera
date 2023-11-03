cmake_minimum_required(VERSION 3.14)

set(PROJECT untitled_no_qt)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT} VERSION 0.1 LANGUAGES CXX)

set(PROJECT_SOURCES
  main.cpp
)

add_executable(${PROJECT}
  ${PROJECT_SOURCES}
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
  if (NOT DEFINED GSTREAMER_PATH)
    set(GSTREAMER_PATH "E:/gstreamer/1.0/mingw_x86_64")
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
    ${GSTREAMER_PATH}/bin/libass-9.dll
    ${GSTREAMER_PATH}/bin/libbz2.dll
    ${GSTREAMER_PATH}/bin/libcairo-2.dll
    ${GSTREAMER_PATH}/bin/libcairo-gobject-2.dll
    ${GSTREAMER_PATH}/bin/libcairo-script-interpreter-2.dll
    ${GSTREAMER_PATH}/bin/libcharset-1.dll
    ${GSTREAMER_PATH}/bin/libcroco-0.6-3.dll
    ${GSTREAMER_PATH}/bin/libcrypto-1_1-x64.dll
    ${GSTREAMER_PATH}/bin/libdav1d.dll
    ${GSTREAMER_PATH}/bin/libdv-4.dll
    ${GSTREAMER_PATH}/bin/libexpat-1.dll
    ${GSTREAMER_PATH}/bin/libffi-7.dll
    ${GSTREAMER_PATH}/bin/libFLAC-8.dll
    ${GSTREAMER_PATH}/bin/libfontconfig-1.dll
    ${GSTREAMER_PATH}/bin/libfreetype-6.dll
    ${GSTREAMER_PATH}/bin/libfribidi-0.dll
    ${GSTREAMER_PATH}/bin/libgcc_s_seh-1.dll
    ${GSTREAMER_PATH}/bin/libgdk_pixbuf-2.0-0.dll
    ${GSTREAMER_PATH}/bin/libgio-2.0-0.dll
    ${GSTREAMER_PATH}/bin/libglib-2.0-0.dll
    ${GSTREAMER_PATH}/bin/libgmodule-2.0-0.dll
    ${GSTREAMER_PATH}/bin/libgobject-2.0-0.dll
    ${GSTREAMER_PATH}/bin/libgraphene-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstadaptivedemux-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstallocators-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstapp-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstaudio-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstbadaudio-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstbase-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstbasecamerabinsrc-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstcheck-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstcodecparsers-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstcodecs-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstcontroller-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstcuda-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstd3d11-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstfft-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstgl-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstinsertbin-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstisoff-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstmpegts-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstnet-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstpbutils-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstphotography-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstplay-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstplayer-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstreamer-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstriff-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstrtp-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstrtsp-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstrtspserver-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstsctp-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstsdp-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgsttag-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgsttranscoder-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgsturidownloader-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstvideo-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstwebrtc-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgstwebrtcnice-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libgthread-2.0-0.dll
    ${GSTREAMER_PATH}/bin/libharfbuzz-0.dll
    ${GSTREAMER_PATH}/bin/libiconv-2.dll
    ${GSTREAMER_PATH}/bin/libintl-8.dll
    ${GSTREAMER_PATH}/bin/libjpeg-8.dll
    ${GSTREAMER_PATH}/bin/libjson-glib-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libkate-1.dll
    ${GSTREAMER_PATH}/bin/libltc-11.dll
    ${GSTREAMER_PATH}/bin/libmp3lame-0.dll
    ${GSTREAMER_PATH}/bin/libmpg123-0.dll
    ${GSTREAMER_PATH}/bin/libnice-10.dll
    ${GSTREAMER_PATH}/bin/libogg-0.dll
    ${GSTREAMER_PATH}/bin/liboggkate-1.dll
    ${GSTREAMER_PATH}/bin/libopenh264-6.dll
    ${GSTREAMER_PATH}/bin/libopenjp2.dll
    ${GSTREAMER_PATH}/bin/libopus-0.dll
    ${GSTREAMER_PATH}/bin/liborc-0.4-0.dll
    ${GSTREAMER_PATH}/bin/liborc-test-0.4-0.dll
    ${GSTREAMER_PATH}/bin/libpango-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libpangocairo-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libpangoft2-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libpangowin32-1.0-0.dll
    ${GSTREAMER_PATH}/bin/libpcre2-8-0.dll
    ${GSTREAMER_PATH}/bin/libpixman-1-0.dll
    ${GSTREAMER_PATH}/bin/libpng16-16.dll
    ${GSTREAMER_PATH}/bin/libpsl-5.dll
    ${GSTREAMER_PATH}/bin/librsvg-2-2.dll
    ${GSTREAMER_PATH}/bin/libsbc-1.dll
    ${GSTREAMER_PATH}/bin/libSoundTouch-1.dll
    ${GSTREAMER_PATH}/bin/libsoup-2.4-1.dll
    ${GSTREAMER_PATH}/bin/libspandsp-2.dll
    ${GSTREAMER_PATH}/bin/libspeex-1.dll
    ${GSTREAMER_PATH}/bin/libsqlite3-0.dll
    ${GSTREAMER_PATH}/bin/libsrt.dll
    ${GSTREAMER_PATH}/bin/libsrtp2-1.dll
    ${GSTREAMER_PATH}/bin/libssl-1_1-x64.dll
    ${GSTREAMER_PATH}/bin/libstdc++-6.dll
    ${GSTREAMER_PATH}/bin/libtag.dll
    ${GSTREAMER_PATH}/bin/libtheora-0.dll
    ${GSTREAMER_PATH}/bin/libtheoradec-1.dll
    ${GSTREAMER_PATH}/bin/libtheoraenc-1.dll
    ${GSTREAMER_PATH}/bin/libtiff-5.dll
    ${GSTREAMER_PATH}/bin/libturbojpeg-0.dll
    ${GSTREAMER_PATH}/bin/libvorbis-0.dll
    ${GSTREAMER_PATH}/bin/libvorbisenc-2.dll
    ${GSTREAMER_PATH}/bin/libvorbisfile-3.dll
    ${GSTREAMER_PATH}/bin/libwavpack.dll
    ${GSTREAMER_PATH}/bin/libwebrtc_audio_processing-0.dll
    ${GSTREAMER_PATH}/bin/libwinpthread-1.dll
    ${GSTREAMER_PATH}/bin/libxml2-2.dll
    ${GSTREAMER_PATH}/bin/libz-1.dll
    ${GSTREAMER_PATH}/bin/libzbar-0.dll
  )
endif ()

target_include_directories(${PROJECT} PUBLIC
  ./
  ${LIBRARIES_INCLUDE}
)

target_link_libraries(${PROJECT}
  ${LIBRARIES}
)
