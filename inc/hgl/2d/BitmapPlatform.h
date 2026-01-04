#pragma once

#include<hgl/2d/Bitmap.h>

// Platform-specific includes
#ifdef _WIN32
    #include<windows.h>
#endif

#ifdef __APPLE__
    #include <CoreGraphics/CoreGraphics.h>
    #include <ApplicationServices/ApplicationServices.h>
#endif

#if defined(__linux__) && defined(HGL_X11_SUPPORT)
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/extensions/XShm.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
#endif

#if defined(__linux__) && defined(HGL_WAYLAND_SUPPORT)
    #include <wayland-client.h>
#endif

namespace hgl::bitmap
{
#ifdef _WIN32
    /**
     * Windows platform bitmap class - Supports DIB (Device Independent Bitmap)
     * Can be drawn directly to HDC using GDI functions
     */
    template<typename T, uint C>
    class BitmapWindows : public Bitmap<T, C>
    {
    protected:
        HBITMAP hBitmap;
        HDC memDC;
        HBITMAP hOldBitmap;

    public:
        BitmapWindows();
        virtual ~BitmapWindows();

        /**
         * Create DIB Section
         * @param w Width
         * @param h Height
         * @param hdc Optional device context for compatibility (default nullptr uses screen DC)
         * @return Whether creation was successful
         */
        bool CreateDIB(uint w, uint h, HDC hdc = nullptr);

        /**
         * Draw to target DC using BitBlt
         * @param hdc Target device context
         * @param dx Destination X coordinate
         * @param dy Destination Y coordinate
         * @param dw Destination width
         * @param dh Destination height
         * @param sx Source X coordinate (default 0)
         * @param sy Source Y coordinate (default 0)
         * @return Whether drawing was successful
         */
        bool BlitTo(HDC hdc, int dx, int dy, int dw, int dh, int sx = 0, int sy = 0) const;

        /**
         * Stretch draw to target DC using StretchBlt
         * @param hdc Target device context
         * @param dx Destination X coordinate
         * @param dy Destination Y coordinate
         * @param dw Destination width
         * @param dh Destination height
         * @param sx Source X coordinate
         * @param sy Source Y coordinate
         * @param sw Source width
         * @param sh Source height
         * @return Whether drawing was successful
         */
        bool StretchBlitTo(HDC hdc, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh) const;

        /**
         * Alpha blend draw using AlphaBlend
         * @param hdc Target device context
         * @param dx Destination X coordinate
         * @param dy Destination Y coordinate
         * @param dw Destination width
         * @param dh Destination height
         * @param sx Source X coordinate
         * @param sy Source Y coordinate
         * @param sw Source width
         * @param sh Source height
         * @param alpha Overall alpha value (0-255, default 255)
         * @return Whether drawing was successful
         */
        bool AlphaBlitTo(HDC hdc, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, uint8 alpha = 255) const;

        HBITMAP GetHBitmap() const { return hBitmap; }
        HDC GetMemDC() const { return memDC; }
    };

    using BitmapRGBA8Windows = BitmapWindows<math::Vector4u8, 4>;
    using BitmapRGB8Windows = BitmapWindows<math::Vector3u8, 3>;
    using BitmapRG8Windows = BitmapWindows<math::Vector2u8, 2>;
    using BitmapGrey8Windows = BitmapWindows<uint8, 1>;
#endif // _WIN32

#ifdef __APPLE__
    /**
     * macOS platform bitmap class - Supports CoreGraphics
     * Can be drawn to CGContext
     */
    template<typename T, uint C>
    class BitmapMacOS : public Bitmap<T, C>
    {
    protected:
        CGContextRef cgContext;
        CGColorSpaceRef colorSpace;

    public:
        BitmapMacOS();
        virtual ~BitmapMacOS();

        /**
         * Create CoreGraphics context
         * @param w Width
         * @param h Height
         * @return Whether creation was successful
         */
        bool CreateCGContext(uint w, uint h);

        /**
         * Create CGImage object
         * @return CGImage reference (caller responsible for releasing)
         */
        CGImageRef CreateCGImage() const;

        /**
         * Draw to target context
         * @param destContext Target context
         * @param destRect Destination rectangle
         * @return Whether drawing was successful
         */
        bool DrawToContext(CGContextRef destContext, CGRect destRect) const;

        /**
         * Draw to target context (with source rectangle)
         * @param destContext Target context
         * @param destRect Destination rectangle
         * @param sourceRect Source rectangle
         * @return Whether drawing was successful
         */
        bool DrawToContext(CGContextRef destContext, CGRect destRect, CGRect sourceRect) const;

        CGContextRef GetCGContext() const { return cgContext; }
    };

    using BitmapRGBA8MacOS = BitmapMacOS<math::Vector4u8, 4>;
    using BitmapRGB8MacOS = BitmapMacOS<math::Vector3u8, 3>;
    using BitmapRG8MacOS = BitmapMacOS<math::Vector2u8, 2>;
    using BitmapGrey8MacOS = BitmapMacOS<uint8, 1>;
#endif // __APPLE__

#if defined(__linux__) && defined(HGL_X11_SUPPORT)
    /**
     * Linux X11 platform bitmap class - Supports XImage and MIT-SHM
     * Can be drawn to X11 Window
     */
    template<typename T, uint C>
    class BitmapX11 : public Bitmap<T, C>
    {
    protected:
        Display* display;
        XImage* ximage;
        GC gc;
        bool using_shm;
        XShmSegmentInfo shminfo;

    public:
        BitmapX11();
        virtual ~BitmapX11();

        /**
         * Create XImage
         * @param w Width
         * @param h Height
         * @param dpy Display connection (default nullptr opens default display)
         * @param screen Screen number (default 0)
         * @param try_shm Whether to try MIT-SHM extension (default true)
         * @return Whether creation was successful
         */
        bool CreateXImage(uint w, uint h, Display* dpy = nullptr, int screen = 0, bool try_shm = true);

        /**
         * Draw to window (specified region)
         * @param window Target window
         * @param dx Destination X coordinate
         * @param dy Destination Y coordinate
         * @param sx Source X coordinate
         * @param sy Source Y coordinate
         * @param w Width
         * @param h Height
         * @return Whether drawing was successful
         */
        bool PutToWindow(Window window, int dx, int dy, int sx, int sy, uint w, uint h) const;

        /**
         * Draw entire bitmap to window
         * @param window Target window
         * @param dx Destination X coordinate (default 0)
         * @param dy Destination Y coordinate (default 0)
         * @return Whether drawing was successful
         */
        bool PutToWindow(Window window, int dx = 0, int dy = 0) const;

        Display* GetDisplay() const { return display; }
        XImage* GetXImage() const { return ximage; }
        bool IsUsingShm() const { return using_shm; }
    };

    using BitmapRGBA8X11 = BitmapX11<math::Vector4u8, 4>;
    using BitmapRGB8X11 = BitmapX11<math::Vector3u8, 3>;
    using BitmapRG8X11 = BitmapX11<math::Vector2u8, 2>;
    using BitmapGrey8X11 = BitmapX11<uint8, 1>;
#endif // HGL_X11_SUPPORT

#if defined(__linux__) && defined(HGL_WAYLAND_SUPPORT)
    /**
     * Linux Wayland platform bitmap class - Supports wl_shm
     * Can be attached to Wayland Surface
     */
    template<typename T, uint C>
    class BitmapWayland : public Bitmap<T, C>
    {
    protected:
        struct wl_shm* shm;
        struct wl_buffer* buffer;
        struct wl_shm_pool* pool;
        int shm_fd;
        void* shm_data;
        size_t shm_size;

    public:
        BitmapWayland();
        virtual ~BitmapWayland();

        /**
         * Create shared memory buffer
         * @param w Width
         * @param h Height
         * @param wayland_shm Wayland shared memory object
         * @return Whether creation was successful
         */
        bool CreateShmBuffer(uint w, uint h, struct wl_shm* wayland_shm);

        /**
         * Attach to surface and commit
         * @param surface Target surface
         * @param x X offset (default 0)
         * @param y Y offset (default 0)
         * @return Whether successful
         */
        bool AttachAndCommit(struct wl_surface* surface, int32_t x = 0, int32_t y = 0) const;

        struct wl_buffer* GetWlBuffer() const { return buffer; }
    };

    using BitmapRGBA8Wayland = BitmapWayland<math::Vector4u8, 4>;
    using BitmapRGB8Wayland = BitmapWayland<math::Vector3u8, 3>;
    using BitmapRG8Wayland = BitmapWayland<math::Vector2u8, 2>;
    using BitmapGrey8Wayland = BitmapWayland<uint8, 1>;
#endif // HGL_WAYLAND_SUPPORT

    // Platform-adaptive type aliases
#ifdef _WIN32
    template<typename T, uint C> using BitmapPlatform = BitmapWindows<T, C>;
    using BitmapRGBA8Platform = BitmapRGBA8Windows;
    using BitmapRGB8Platform = BitmapRGB8Windows;
    using BitmapRG8Platform = BitmapRG8Windows;
    using BitmapGrey8Platform = BitmapGrey8Windows;
#elif defined(__APPLE__)
    template<typename T, uint C> using BitmapPlatform = BitmapMacOS<T, C>;
    using BitmapRGBA8Platform = BitmapRGBA8MacOS;
    using BitmapRGB8Platform = BitmapRGB8MacOS;
    using BitmapRG8Platform = BitmapRG8MacOS;
    using BitmapGrey8Platform = BitmapGrey8MacOS;
#elif defined(__linux__) && defined(HGL_X11_SUPPORT)
    template<typename T, uint C> using BitmapPlatform = BitmapX11<T, C>;
    using BitmapRGBA8Platform = BitmapRGBA8X11;
    using BitmapRGB8Platform = BitmapRGB8X11;
    using BitmapRG8Platform = BitmapRG8X11;
    using BitmapGrey8Platform = BitmapGrey8X11;
#elif defined(__linux__) && defined(HGL_WAYLAND_SUPPORT)
    template<typename T, uint C> using BitmapPlatform = BitmapWayland<T, C>;
    using BitmapRGBA8Platform = BitmapRGBA8Wayland;
    using BitmapRGB8Platform = BitmapRGB8Wayland;
    using BitmapRG8Platform = BitmapRG8Wayland;
    using BitmapGrey8Platform = BitmapGrey8Wayland;
#endif

} // namespace hgl::bitmap
