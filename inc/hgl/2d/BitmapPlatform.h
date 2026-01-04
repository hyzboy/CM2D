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
     * Windows平台位图类 - 支持DIB (Device Independent Bitmap)
     * 可通过GDI函数直接绘制到HDC
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
         * 创建DIB Section
         * @param w 宽度
         * @param h 高度
         * @param hdc 可选的设备上下文，用于兼容性（默认为nullptr使用屏幕DC）
         * @return 是否创建成功
         */
        bool CreateDIB(uint w, uint h, HDC hdc = nullptr);

        /**
         * 使用BitBlt绘制到目标DC
         * @param hdc 目标设备上下文
         * @param dx 目标X坐标
         * @param dy 目标Y坐标
         * @param dw 目标宽度
         * @param dh 目标高度
         * @param sx 源X坐标（默认0）
         * @param sy 源Y坐标（默认0）
         * @return 是否绘制成功
         */
        bool BlitTo(HDC hdc, int dx, int dy, int dw, int dh, int sx = 0, int sy = 0) const;

        /**
         * 使用StretchBlt拉伸绘制到目标DC
         * @param hdc 目标设备上下文
         * @param dx 目标X坐标
         * @param dy 目标Y坐标
         * @param dw 目标宽度
         * @param dh 目标高度
         * @param sx 源X坐标
         * @param sy 源Y坐标
         * @param sw 源宽度
         * @param sh 源高度
         * @return 是否绘制成功
         */
        bool StretchBlitTo(HDC hdc, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh) const;

        /**
         * 使用AlphaBlend进行Alpha混合绘制
         * @param hdc 目标设备上下文
         * @param dx 目标X坐标
         * @param dy 目标Y坐标
         * @param dw 目标宽度
         * @param dh 目标高度
         * @param sx 源X坐标
         * @param sy 源Y坐标
         * @param sw 源宽度
         * @param sh 源高度
         * @param alpha 整体Alpha值（0-255，默认255）
         * @return 是否绘制成功
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
     * macOS平台位图类 - 支持CoreGraphics
     * 可绘制到CGContext
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
         * 创建CoreGraphics上下文
         * @param w 宽度
         * @param h 高度
         * @return 是否创建成功
         */
        bool CreateCGContext(uint w, uint h);

        /**
         * 创建CGImage对象
         * @return CGImage引用（调用者负责释放）
         */
        CGImageRef CreateCGImage() const;

        /**
         * 绘制到目标上下文
         * @param destContext 目标上下文
         * @param destRect 目标矩形
         * @return 是否绘制成功
         */
        bool DrawToContext(CGContextRef destContext, CGRect destRect) const;

        /**
         * 绘制到目标上下文（带源矩形）
         * @param destContext 目标上下文
         * @param destRect 目标矩形
         * @param sourceRect 源矩形
         * @return 是否绘制成功
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
     * Linux X11平台位图类 - 支持XImage和MIT-SHM
     * 可绘制到X11 Window
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
         * 创建XImage
         * @param w 宽度
         * @param h 高度
         * @param dpy 显示连接（默认nullptr会打开默认显示）
         * @param screen 屏幕编号（默认0）
         * @param try_shm 是否尝试使用MIT-SHM扩展（默认true）
         * @return 是否创建成功
         */
        bool CreateXImage(uint w, uint h, Display* dpy = nullptr, int screen = 0, bool try_shm = true);

        /**
         * 绘制到窗口（指定区域）
         * @param window 目标窗口
         * @param dx 目标X坐标
         * @param dy 目标Y坐标
         * @param sx 源X坐标
         * @param sy 源Y坐标
         * @param w 宽度
         * @param h 高度
         * @return 是否绘制成功
         */
        bool PutToWindow(Window window, int dx, int dy, int sx, int sy, uint w, uint h) const;

        /**
         * 绘制整个位图到窗口
         * @param window 目标窗口
         * @param dx 目标X坐标（默认0）
         * @param dy 目标Y坐标（默认0）
         * @return 是否绘制成功
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
     * Linux Wayland平台位图类 - 支持wl_shm
     * 可附加到Wayland Surface
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
         * 创建共享内存缓冲区
         * @param w 宽度
         * @param h 高度
         * @param wayland_shm Wayland共享内存对象
         * @return 是否创建成功
         */
        bool CreateShmBuffer(uint w, uint h, struct wl_shm* wayland_shm);

        /**
         * 附加到surface并提交
         * @param surface 目标surface
         * @param x X偏移（默认0）
         * @param y Y偏移（默认0）
         * @return 是否成功
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
