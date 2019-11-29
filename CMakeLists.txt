cmake_minimum_required(VERSION 3.0)

project(CM2D)

include(path_config.cmake)
CM2DSetup(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CM2D_ROOT_SOURCE_PATH})