cmake_minimum_required(VERSION 3.14)

set(PROJECT GSTREAMER_CPP_CAMERA_EXAMPLES)
project(${PROJECT} VERSION 0.1 LANGUAGES CXX)

add_subdirectory(example/video_src_to_view)
add_subdirectory(example/video_src_to_file)
add_subdirectory(example/video_src_to_encoder)
add_subdirectory(example/play_wav)
add_subdirectory(example/audio_src_to_sink)
