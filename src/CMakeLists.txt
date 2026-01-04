file(GLOB CM2D_HEADER ${CM2D_ROOT_INCLUDE_PATH}/hgl/2d/*.h)

file(GLOB CM2D_PIXEL_SOURCE PixelFormat/*.cpp)

# 基础Bitmap源文件（排除平台特定文件）
file(GLOB CM2D_BITMAP_BASE_SOURCE 
    Bitmap/tga.cpp 
    Bitmap/BitmapTGAStream.cpp
)

# 平台特定源文件
set(CM2D_BITMAP_PLATFORM_SOURCE "")

# Windows平台
if(WIN32)
    list(APPEND CM2D_BITMAP_PLATFORM_SOURCE Bitmap/BitmapWindows.cpp)
    set(CM2D_PLATFORM_LIBS "")
    list(APPEND CM2D_PLATFORM_LIBS gdi32 msimg32)
endif()

# macOS平台
if(APPLE)
    list(APPEND CM2D_BITMAP_PLATFORM_SOURCE Bitmap/BitmapMacOS.cpp)
    set(CM2D_PLATFORM_LIBS "")
    find_library(COREGRAPHICS_LIBRARY CoreGraphics)
    find_library(APPSERVICES_LIBRARY ApplicationServices)
    list(APPEND CM2D_PLATFORM_LIBS ${COREGRAPHICS_LIBRARY} ${APPSERVICES_LIBRARY})
endif()

# Linux平台
if(UNIX AND NOT APPLE)
    set(CM2D_PLATFORM_LIBS "")
    
    # X11支持选项
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
    
    # Wayland支持选项
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

set(CM2D_BITMAP_SOURCE ${CM2D_BITMAP_BASE_SOURCE} ${CM2D_BITMAP_PLATFORM_SOURCE})

SOURCE_GROUP("Header Files" FILES ${CM2D_HEADER})
SOURCE_GROUP("PixelFormat" FILES ${CM2D_PIXEL_SOURCE})
SOURCE_GROUP("Bitmap" FILES ${CM2D_BITMAP_SOURCE})

add_cm_library(CM2D "CM" ${CM2D_HEADER} ${CM2D_PIXEL_SOURCE} ${CM2D_BITMAP_SOURCE})

# 链接平台特定库
if(DEFINED CM2D_PLATFORM_LIBS)
    target_link_libraries(CM2D ${CM2D_PLATFORM_LIBS})
endif()
