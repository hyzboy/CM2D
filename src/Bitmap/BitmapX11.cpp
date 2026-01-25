#if defined(__linux__) && defined(HGL_X11_SUPPORT)

#include<hgl/2d/BitmapPlatform.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/extensions/XShm.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<cstring>
#include<cstdlib>

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
        // Save data pointer for non-SHM mode cleanup
        void* dataToFree = (!using_shm) ? this->data : nullptr;

        // Must set data to nullptr before releasing platform resources to prevent base class double-free
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
                // In non-SHM mode, we need to manually free memory
                // XDestroyImage normally frees data, but we have detached it
                ximage->data = nullptr;
                XDestroyImage(ximage);
                if (dataToFree)
                {
                    free(dataToFree);
                }
            }
            ximage = nullptr;
        }

        if (gc != None)
        {
            XFreeGC(display, gc);
            gc = None;
        }

        // Note: We don't close display as it may be externally provided
    }

    template<typename T, uint C>
    bool BitmapX11<T, C>::CreateXImage(uint w, uint h, Display* dpy, int screen, bool try_shm)
    {
        if (!w || !h)
            return false;

        // Clean up old resources
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

        // If no display provided, open default display
        bool own_display = false;
        if (!dpy)
        {
            dpy = XOpenDisplay(nullptr);
            if (!dpy)
                return false;
            own_display = true;
        }

        display = dpy;

        // Get visual information
        Visual* visual = DefaultVisual(display, screen);
        int depth = DefaultDepth(display, screen);

        // Try to use MIT-SHM extension
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

                            // Create GC
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

        // Fall back to regular XImage
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

        // Create GC
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

    // Explicit instantiation of common types
    template class BitmapX11<math::Vector4u8, 4>;
    template class BitmapX11<math::Vector3u8, 3>;
    template class BitmapX11<math::Vector2u8, 2>;
    template class BitmapX11<uint8, 1>;

} // namespace hgl::bitmap

#endif // defined(__linux__) && defined(HGL_X11_SUPPORT)
