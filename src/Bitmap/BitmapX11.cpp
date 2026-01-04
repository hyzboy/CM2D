#if defined(__linux__) && defined(HGL_X11_SUPPORT)

#include<hgl/2d/BitmapPlatform.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/extensions/XShm.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<cstring>

namespace hgl::bitmap
{
    template<typename T, uint C>
    BitmapX11<T, C>::BitmapX11()
        : Bitmap<T, C>()
    {
        display = nullptr;
        ximage = nullptr;
        gc = None;
        using_shm = false;
        memset(&shminfo, 0, sizeof(shminfo));
    }

    template<typename T, uint C>
    BitmapX11<T, C>::~BitmapX11()
    {
        // 必须在释放平台资源前将data设为nullptr，防止基类重复释放
        this->data = nullptr;

        if (ximage)
        {
            if (using_shm)
            {
                XShmDetach(display, &shminfo);
                XDestroyImage(ximage);
                shmdt(shminfo.shmaddr);
                shmctl(shminfo.shmid, IPC_RMID, 0);
            }
            else
            {
                // 非SHM模式下，XDestroyImage会释放data，所以我们需要先detach
                ximage->data = nullptr;
                XDestroyImage(ximage);
            }
            ximage = nullptr;
        }

        if (gc != None)
        {
            XFreeGC(display, gc);
            gc = None;
        }

        // 注意：我们不关闭display，因为它可能是外部传入的
    }

    template<typename T, uint C>
    bool BitmapX11<T, C>::CreateXImage(uint w, uint h, Display* dpy, int screen, bool try_shm)
    {
        if (!w || !h)
            return false;

        // 清理旧资源
        if (ximage)
        {
            if (using_shm)
            {
                XShmDetach(display, &shminfo);
                XDestroyImage(ximage);
                shmdt(shminfo.shmaddr);
                shmctl(shminfo.shmid, IPC_RMID, 0);
            }
            else
            {
                ximage->data = nullptr;
                XDestroyImage(ximage);
            }
            ximage = nullptr;
        }

        if (gc != None)
        {
            XFreeGC(display, gc);
            gc = None;
        }

        this->data = nullptr;
        this->width = w;
        this->height = h;

        // 如果没有提供display，打开默认显示
        bool own_display = false;
        if (!dpy)
        {
            dpy = XOpenDisplay(nullptr);
            if (!dpy)
                return false;
            own_display = true;
        }

        display = dpy;

        // 获取视觉信息
        Visual* visual = DefaultVisual(display, screen);
        int depth = DefaultDepth(display, screen);

        // 尝试使用MIT-SHM扩展
        using_shm = false;
        if (try_shm && XShmQueryExtension(display))
        {
            ximage = XShmCreateImage(display, visual, depth, ZPixmap, nullptr, &shminfo, w, h);
            if (ximage)
            {
                shminfo.shmid = shmget(IPC_PRIVATE, ximage->bytes_per_line * ximage->height, IPC_CREAT | 0777);
                if (shminfo.shmid >= 0)
                {
                    shminfo.shmaddr = (char*)shmat(shminfo.shmid, nullptr, 0);
                    if (shminfo.shmaddr != (char*)-1)
                    {
                        ximage->data = shminfo.shmaddr;
                        shminfo.readOnly = False;

                        if (XShmAttach(display, &shminfo))
                        {
                            using_shm = true;
                            this->data = (T*)ximage->data;

                            // 创建GC
                            gc = XCreateGC(display, DefaultRootWindow(display), 0, nullptr);

                            return true;
                        }
                    }
                    shmctl(shminfo.shmid, IPC_RMID, 0);
                }
                XDestroyImage(ximage);
                ximage = nullptr;
            }
        }

        // 回退到普通XImage
        size_t dataSize = w * h * sizeof(T);
        void* imageData = malloc(dataSize);
        if (!imageData)
        {
            if (own_display)
                XCloseDisplay(display);
            return false;
        }

        memset(imageData, 0, dataSize);

        ximage = XCreateImage(display, visual, depth, ZPixmap, 0, (char*)imageData, w, h, 32, 0);
        if (!ximage)
        {
            free(imageData);
            if (own_display)
                XCloseDisplay(display);
            return false;
        }

        this->data = (T*)imageData;

        // 创建GC
        gc = XCreateGC(display, DefaultRootWindow(display), 0, nullptr);

        return true;
    }

    template<typename T, uint C>
    bool BitmapX11<T, C>::PutToWindow(Window window, int dx, int dy, int sx, int sy, uint w, uint h) const
    {
        if (!display || !ximage || !this->data || gc == None)
            return false;

        if (using_shm)
        {
            XShmPutImage(display, window, gc, ximage, sx, sy, dx, dy, w, h, False);
        }
        else
        {
            XPutImage(display, window, gc, ximage, sx, sy, dx, dy, w, h);
        }

        XFlush(display);
        return true;
    }

    template<typename T, uint C>
    bool BitmapX11<T, C>::PutToWindow(Window window, int dx, int dy) const
    {
        if (!this->data)
            return false;

        return PutToWindow(window, dx, dy, 0, 0, this->width, this->height);
    }

    // 显式实例化常用类型
    template class BitmapX11<math::Vector4u8, 4>;
    template class BitmapX11<math::Vector3u8, 3>;
    template class BitmapX11<math::Vector2u8, 2>;
    template class BitmapX11<uint8, 1>;

} // namespace hgl::bitmap

#endif // defined(__linux__) && defined(HGL_X11_SUPPORT)
