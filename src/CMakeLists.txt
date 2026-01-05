set(CM2D_HEADER_PATH ${CM2D_ROOT_INCLUDE_PATH}/hgl/2d)

# ==================== Header Files Classification ====================

# Core Bitmap Headers
set(CM2D_BITMAP_HEADERS 
    ${CM2D_HEADER_PATH}/Bitmap.h
    ${CM2D_HEADER_PATH}/BitmapLoad.h
    ${CM2D_HEADER_PATH}/BitmapSave.h
    ${CM2D_HEADER_PATH}/BitmapPlatform.h
    ${CM2D_HEADER_PATH}/BitmapImageMagick.h
)

# Image Format Headers
set(CM2D_FORMAT_HEADERS
    ${CM2D_HEADER_PATH}/TGA.h
)

# Image Processing Headers
set(CM2D_PROCESSING_HEADERS
    ${CM2D_HEADER_PATH}/Blend.h
    ${CM2D_HEADER_PATH}/ColorConversion.h
    ${CM2D_HEADER_PATH}/DrawGeometry.h
    ${CM2D_HEADER_PATH}/MipMap.h
    ${CM2D_HEADER_PATH}/Resize.h
    ${CM2D_HEADER_PATH}/Transform.h
    ${CM2D_HEADER_PATH}/Tile.h
)

# Channel Operation Headers
set(CM2D_CHANNEL_HEADERS
    ${CM2D_HEADER_PATH}/ChannelMerge.h
    ${CM2D_HEADER_PATH}/ChannelOps.h
    ${CM2D_HEADER_PATH}/ChannelSplit.h
)

# Terrain Generation Headers
set(CM2D_TERRAIN_HEADERS
    ${CM2D_HEADER_PATH}/TerrainMap.h
    ${CM2D_HEADER_PATH}/NoiseMap.h
)

# Misc Headers
set(CM2D_MISC_HEADERS
    ${CM2D_HEADER_PATH}/VSBase.h
)

# ==================== Source Files Classification ====================

# Pixel Format Source Files
set(CM2D_PIXEL_SOURCE 
    PixelFormat/rgb.cpp
    PixelFormat/rgba.cpp
)

# Base Bitmap Source Files (platform-independent)
set(CM2D_BITMAP_BASE_SOURCE 
    Bitmap/tga.cpp 
    Bitmap/BitmapTGAStream.cpp
)

# Platform-specific Bitmap Source Files
set(CM2D_BITMAP_PLATFORM_SOURCE "")
set(CM2D_PLATFORM_LIBS "")

# Windows platform
if(WIN32)
    list(APPEND CM2D_BITMAP_PLATFORM_SOURCE Bitmap/BitmapWindows.cpp)
    list(APPEND CM2D_PLATFORM_LIBS gdi32 msimg32)
endif()

# macOS platform
if(APPLE)
    list(APPEND CM2D_BITMAP_PLATFORM_SOURCE Bitmap/BitmapMacOS.cpp)
    find_library(COREGRAPHICS_LIBRARY CoreGraphics)
    find_library(APPSERVICES_LIBRARY ApplicationServices)
    list(APPEND CM2D_PLATFORM_LIBS ${COREGRAPHICS_LIBRARY} ${APPSERVICES_LIBRARY})
endif()

# Linux platform
if(UNIX AND NOT APPLE)
    # X11 support option
    option(CM2D_USE_X11 "Enable X11 support" ON)
    if(CM2D_USE_X11)
        find_package(X11)
        if(X11_FOUND)
            list(APPEND CM2D_BITMAP_PLATFORM_SOURCE Bitmap/BitmapX11.cpp)
            add_definitions(-DHGL_X11_SUPPORT)
            list(APPEND CM2D_PLATFORM_LIBS ${X11_LIBRARIES})
            if(X11_XShm_FOUND)
                list(APPEND CM2D_PLATFORM_LIBS ${X11_Xext_LIB})
            endif()
        else()
            message(WARNING "X11 not found, X11 support disabled")
        endif()
    endif()
    
    # Wayland support option
    option(CM2D_USE_WAYLAND "Enable Wayland support" OFF)
    if(CM2D_USE_WAYLAND)
        find_package(PkgConfig)
        if(PkgConfig_FOUND)
            pkg_check_modules(WAYLAND wayland-client)
            if(WAYLAND_FOUND)
                list(APPEND CM2D_BITMAP_PLATFORM_SOURCE Bitmap/BitmapWayland.cpp)
                add_definitions(-DHGL_WAYLAND_SUPPORT)
                list(APPEND CM2D_PLATFORM_LIBS ${WAYLAND_LIBRARIES})
                include_directories(${WAYLAND_INCLUDE_DIRS})
            else()
                message(WARNING "Wayland not found, Wayland support disabled")
            endif()
        else()
            message(WARNING "PkgConfig not found, Wayland support disabled")
        endif()
    endif()
endif()

# ImageMagick support option
option(CM2D_USE_IMAGEMAGICK "Enable ImageMagick support for additional image formats" OFF)
if(CM2D_USE_IMAGEMAGICK)
    find_package(ImageMagick COMPONENTS Magick++)
    if(ImageMagick_FOUND)
        list(APPEND CM2D_BITMAP_BASE_SOURCE Bitmap/BitmapImageMagick.cpp)
        add_definitions(-DHGL_IMAGEMAGICK_SUPPORT)
        message(STATUS "ImageMagick found: ${ImageMagick_VERSION}")
        message(STATUS "ImageMagick include: ${ImageMagick_INCLUDE_DIRS}")
        message(STATUS "ImageMagick libraries: ${ImageMagick_LIBRARIES}")
    else()
        message(WARNING "ImageMagick not found, ImageMagick support disabled")
    endif()
endif()

# Combine all bitmap sources
set(CM2D_BITMAP_SOURCE ${CM2D_BITMAP_BASE_SOURCE} ${CM2D_BITMAP_PLATFORM_SOURCE})

# Terrain Generation Source Files
set(CM2D_TERRAIN_SOURCE
    Terrain/NoiseGenerator.cpp
    Terrain/PerlinNoise.cpp
    Terrain/SimplexNoise.cpp
    Terrain/VoronoiNoise.cpp
    Terrain/HeightMap.cpp
    Terrain/BiomeMap.cpp
    Terrain/TerrainGenerator.cpp
)

# ==================== Source Groups for IDE Organization ====================

# Header File Groups
SOURCE_GROUP("Header Files\\Bitmap" FILES ${CM2D_BITMAP_HEADERS})
SOURCE_GROUP("Header Files\\Image Format" FILES ${CM2D_FORMAT_HEADERS})
SOURCE_GROUP("Header Files\\Image Processing" FILES ${CM2D_PROCESSING_HEADERS})
SOURCE_GROUP("Header Files\\Channel Operations" FILES ${CM2D_CHANNEL_HEADERS})
SOURCE_GROUP("Header Files\\Terrain" FILES ${CM2D_TERRAIN_HEADERS})
SOURCE_GROUP("Header Files\\Misc" FILES ${CM2D_MISC_HEADERS})

# Source File Groups
SOURCE_GROUP("Source Files\\Pixel Format" FILES ${CM2D_PIXEL_SOURCE})
SOURCE_GROUP("Source Files\\Bitmap\\Core" FILES ${CM2D_BITMAP_BASE_SOURCE})
SOURCE_GROUP("Source Files\\Bitmap\\Platform" FILES ${CM2D_BITMAP_PLATFORM_SOURCE})
SOURCE_GROUP("Source Files\\Terrain" FILES ${CM2D_TERRAIN_SOURCE})

# ==================== Library Target ====================

add_cm_library(CM2D "CM" 
    ${CM2D_BITMAP_HEADERS}
    ${CM2D_FORMAT_HEADERS}
    ${CM2D_PROCESSING_HEADERS}
    ${CM2D_CHANNEL_HEADERS}
    ${CM2D_TERRAIN_HEADERS}
    ${CM2D_MISC_HEADERS}
    ${CM2D_PIXEL_SOURCE}
    ${CM2D_BITMAP_SOURCE}
    ${CM2D_TERRAIN_SOURCE}
)

# ==================== Library Dependencies ====================

# Link platform-specific libraries
if(DEFINED CM2D_PLATFORM_LIBS)
    target_link_libraries(CM2D PRIVATE ${CM2D_PLATFORM_LIBS})
endif()

# Link ImageMagick if enabled
if(CM2D_USE_IMAGEMAGICK AND ImageMagick_FOUND)
    target_include_directories(CM2D PRIVATE ${ImageMagick_INCLUDE_DIRS})
    target_link_libraries(CM2D PRIVATE ${ImageMagick_LIBRARIES})
endif()
