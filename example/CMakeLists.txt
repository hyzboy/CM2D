cmake_minimum_required(VERSION 3.28)

# Example executable for terrain generation
add_executable(TerrainGenExample TerrainGenExample.cpp)

# Link against CM2D library
target_link_libraries(TerrainGenExample CM2D)
