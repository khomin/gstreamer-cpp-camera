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
    utils/config.h
    qml.qrc
)
#    gst_launch_pipeline.c

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
#    add_library(${PROJECT} SHARED
#        ${PROJECT_SOURCES}
#    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${PROJECT} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT} SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(${PROJECT}
          ${PROJECT_SOURCES}
        )
    endif()
endif()

set_target_properties(${PROJECT} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)

if(IOS)
    #
    # none
    #
elseif(APPLE)
    set(LIBRARIES_INCLUDE
        /Library/Frameworks/GStreamer.framework/Versions/1.0/include
        /Library/Frameworks/GStreamer.framework/Versions/1.0/Headers
    )
    set(LIBRARIES ${LIBRARIES}
        -liconv
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libass.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libglib-2.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgobject-2.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgraphene-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsta52dec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaccurip.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstadaptivedemux-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstadaptivedemux-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstadaptivedemux2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstadder.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstadpcmdec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstadpcmenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaes.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaiff.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstalaw.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstallocators-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstallocators-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstalpha.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstalphacolor.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstamrnb.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstamrwbdec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstapetag.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstapp-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstapp-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstapp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstapplemedia.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstasf.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstasfmux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstassrender.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstaudio-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstaudio-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiobuffersplit.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudioconvert.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiofx.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiofxbad.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiolatency.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiomixer.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiomixmatrix.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudioparsers.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiorate.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudioresample.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiotestsrc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaudiovisualizers.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstauparse.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstautoconvert.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstautodetect.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstavi.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstaws.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstbadaudio-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstbadaudio-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstbase-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstbase-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstbasecamerabinsrc-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstbayer.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstbz2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcairo.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcamerabin.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcdg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcheck-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcheck-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstclaxon.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstclosedcaption.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcodecalpha.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcodecparsers-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcodecparsers-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcodecs-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcodecs-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcodectimestamper.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcoloreffects.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcompositor.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcontroller-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstcontroller-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcoreelements.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcoretracers.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstcutter.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdash.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdav1d.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdebug.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdebugutilsbad.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdecklink.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdeinterlace.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdtls.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdtmf.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdtsdec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdv.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdvbsubenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdvbsuboverlay.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdvdlpcmdec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdvdread.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdvdspu.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstdvdsub.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsteffectv.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstencoding.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstequalizer.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstfallbackswitch.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstfft-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstfft-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstffv1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstfieldanalysis.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstflac.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstflv.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstflxdec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstfmp4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstfreeverb.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstfrei0r.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgaudieffects.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgdkpixbuf.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgdp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgeometrictransform.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstges.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgif.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgio.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstgl-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstgl-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgoom.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstgoom2k1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsthls.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsthlssink3.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsthsv.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsticydemux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstid3demux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstid3tag.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstimagefreeze.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstinsertbin-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstinsertbin-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstinter.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstinterlace.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstinterleave.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstipcpipeline.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstisoff-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstisoff-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstisomp4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstivtc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstjpeg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstjpegformat.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstjson.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstkate.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstladspa.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstlame.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstlegacyrawparse.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstlevel.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstlewton.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstlibav.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstlivesync.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmatroska.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmidi.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmp4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmpegpsdemux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmpegpsmux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstmpegts-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstmpegts-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmpegtsdemux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmpegtsmux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmpg123.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmulaw.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmultifile.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmultipart.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstmxf.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstndi.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstnet-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstnet-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstnetsim.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstnice.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstnle.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstogg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstopengl.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstopenh264.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstopenjpeg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstopus.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstopusparse.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstosxaudio.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstosxvideo.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstoverlaycomposition.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstpango.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstpbutils-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstpbutils-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstpcapparse.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstphotography-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstphotography-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstplay-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstplay-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstplayback.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstplayer-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstplayer-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstpng.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstpnm.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstproxy.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstraptorq.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrav1e.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrawparse.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrealmedia.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstreamer-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstreamer-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstregex.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstremovesilence.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstreplaygain.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstreqwest.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstresindvd.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrfbsrc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstriff-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstriff-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrist.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrsaudiofx.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrsclosedcaption.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrsonvif.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrspng.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrsrtp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrstracers.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrsvg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrsvideofx.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrswebrtc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtmp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtmp2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstrtp-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstrtp-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtpmanager.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtpmanagerbad.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtponvif.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstrtsp-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstrtsp-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtsp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstrtspclientsink.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstrtspserver-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstrtspserver-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsbc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstsctp-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstsctp-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsctp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstsdp-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstsdp-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsdpelem.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsegmentclip.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstshapewipe.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstshm.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsiren.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsmooth.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsmoothstreaming.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsmpte.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsoundtouch.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsoup.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstspandsp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstspectrum.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstspeed.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstspeex.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsrt.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsrtp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsubenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstsubparse.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstswitchbin.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgsttag-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgsttag-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttaglib.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttcp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttextahead.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttextwrap.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttheora.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstthreadshare.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttimecode.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttogglerecord.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgsttranscoder-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgsttranscoder-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsttypefindfunctions.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstudp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgsturidownloader-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgsturidownloader-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsturiplaylistbin.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstvalidate-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstvalidate-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstvideo-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstvideo-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideobox.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideoconvertscale.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideocrop.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideofilter.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideofiltersbad.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideoframe_audiolevel.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideomixer.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideoparsersbad.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideorate.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideosignal.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvideotestsrc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvoaacenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvolume.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvorbis.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstvpx.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstvulkan-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstvulkan-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstwavenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstwavpack.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstwavparse.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstwebrtc-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstwebrtc-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstwebrtc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstwebrtcdsp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstwebrtchttp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstwebrtcnice-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgstwebrtcnice-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstx264.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstxingmux.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsty4mdec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgsty4menc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/gstreamer-1.0/libgstzbar.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgthread-2.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libgthread-2.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libharfbuzz.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libharfbuzz.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libintl.8.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libintl.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libjpeg.8.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libjpeg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libjson-glib-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libjson-glib-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libkate.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libkate.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libltc.11.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libltc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libMoltenVK.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libmp3lame.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libmp3lame.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libmpg123.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libmpg123.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libnice.10.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libnice.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libogg.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libogg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/liboggkate.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/liboggkate.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopencore-amrnb.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopencore-amrnb.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopencore-amrwb.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopencore-amrwb.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopenh264.6.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopenh264.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopenjp2.2.3.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopenjp2.7.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopenjp2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopus.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libopus.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/liborc-0.4.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/liborc-0.4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/liborc-test-0.4.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/liborc-test-0.4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpango-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpango-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpangocairo-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpangocairo-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpangoft2-1.0.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpangoft2-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpcre2-8.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpcre2-8.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpixman-1.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpixman-1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpng16.16.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpng16.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpsl.5.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libpsl.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/librsvg-2.2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/librsvg-2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/librtmp.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/librtmp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsbc.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsbc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libSoundTouch.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libSoundTouch.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsoup-2.4.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsoup-2.4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libspandsp.2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libspandsp.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libspeex.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libspeex.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsrt.1.4.3.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsrt.1.4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsrt.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsrtp2.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libsrtp2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libssl.1.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libssl.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libswresample.4.3.100.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libswresample.4.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libswresample.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtag.1.17.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtag.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtag.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtheora.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtheora.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtheoradec.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtheoradec.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtheoraenc.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtheoraenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtiff.5.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libtiff.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libturbojpeg.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libturbojpeg.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvo-aacenc.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvo-aacenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvorbis.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvorbis.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvorbisenc.2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvorbisenc.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvorbisfile.3.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvorbisfile.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvpx.7.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libvpx.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libwavpack.5.2.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libwavpack.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libwebrtc_audio_processing.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libwebrtc_audio_processing.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libx264.157.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libx264.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libxml2.2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libxml2.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libz.1.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libz.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libzbar.0.dylib
        /Library/Frameworks/GStreamer.framework/Versions/1.0/lib/libzbar.dylib
    )
elseif(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB REQUIRED glib-2.0)
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
    pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)
    set(LIBRARIES
        ${GST_APP_LIBRARIES} ${GST_VIDEO_LIBRARIES}
    )
    set(LIBRARIES_INCLUDE
        /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /usr/include/glib-2.0
        /usr/local/include/gstreamer-1.0
        /usr/include/gstreamer-1.0/
    )
elseif(WIN32)
    #
    # none
    #
endif()

target_link_libraries(${PROJECT} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    ${LIBRARIES}
)

# macos need info.plist
if(APPLE)
    set_target_properties(${PROJECT} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
endif()

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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT})
    qt_finalize_executable(${PROJECT})
endif()
