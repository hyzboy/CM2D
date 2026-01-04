cmake_minimum_required(VERSION 3.28)

project(CM2D)

include(path_config.cmake)
CM2DSetup(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CM2D_ROOT_SOURCE_PATH})

# Optional: Build examples
option(CM2D_BUILD_EXAMPLES "Build CM2D examples" OFF)
if(CM2D_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

