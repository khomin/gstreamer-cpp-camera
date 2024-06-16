cmake_minimum_required(VERSION 3.14)

set(PROJECT VideoSrcToFile)
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
