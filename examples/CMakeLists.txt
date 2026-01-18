cmake_minimum_required(VERSION 3.28)

# Root examples
add_executable(TerrainGenExample TerrainGenExample.cpp)
target_link_libraries(TerrainGenExample CM2D)

# ImageMagick example (only if ImageMagick is enabled)
if(CM2D_USE_IMAGEMAGICK AND ImageMagick_FOUND)
    add_executable(ImageMagickExample ImageMagickExample.cpp)
    target_link_libraries(ImageMagickExample CM2D)
endif()

# Graphics examples subdirectory
add_subdirectory(graphics)

# Pick examples subdirectory
add_subdirectory(pick)
