cmake_minimum_required(VERSION 3.28)

# Example executable for terrain generation
add_executable(TerrainGenExample TerrainGenExample.cpp)

# Link against CM2D library
target_link_libraries(TerrainGenExample CM2D)

# ImageMagick example (only if ImageMagick is enabled)
if(CM2D_USE_IMAGEMAGICK AND ImageMagick_FOUND)
    add_executable(ImageMagickExample ImageMagickExample.cpp)
    target_link_libraries(ImageMagickExample CM2D)
endif()
