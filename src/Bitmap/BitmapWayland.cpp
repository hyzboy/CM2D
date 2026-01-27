#if defined(__linux__) && defined(HGL_WAYLAND_SUPPORT)

#include<hgl/2d/BitmapPlatform.h>
#include<wayland-client.h>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<cstring>
#include<errno.h>
#include<cstdio>
#include<cstdlib>

#ifdef __linux__
#include<sys/syscall.h>
#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif
#endif

namespace hgl::bitmap
{
    // Create anonymous file for shared memory
    static int create_anonymous_file(size_t size)
    {
        int fd = -1;
        char name[32];

        // Try to use memfd_create (Linux 3.17+)
        #ifdef __NR_memfd_create
        fd = syscall(__NR_memfd_create, "wl_shm", MFD_CLOEXEC);
        if (fd >= 0)
            return fd;
        #endif

        // Fall back to traditional method
        const char* path = getenv("XDG_RUNTIME_DIR");
        if (!path)
            return -1;

        snprintf(name, sizeof(name), "%s/wl_shm-XXXXXX", path);
        fd = mkstemp(name);
        if (fd < 0)
            return -1;

        unlink(name);

        if (ftruncate(fd, size) < 0)
        {
            close(fd);
            return -1;
        }

        return fd;
    }

    template<typename T, uint C>
    BitmapWayland<T, C>::BitmapWayland()
        : Bitmap<T, C>()
    {
        shm = nullptr;
        buffer = nullptr;
        pool = nullptr;
        shm_fd = -1;
        shm_data = nullptr;
        shm_size = 0;
    }

    template<typename T, uint C>
    BitmapWayland<T, C>::~BitmapWayland()
    {
        // Must set data to nullptr before releasing platform resources to prevent base class double-free
        this->data = nullptr;

        if (buffer)
        {
            wl_buffer_destroy(buffer);
            buffer = nullptr;
        }

        if (pool)
        {
            wl_shm_pool_destroy(pool);
            pool = nullptr;
        }

        if (shm_data && shm_data != MAP_FAILED)
        {
            munmap(shm_data, shm_size);
            shm_data = nullptr;
        }

        if (shm_fd >= 0)
        {
            close(shm_fd);
            shm_fd = -1;
        }

        shm_size = 0;
        // Note: We don't release shm as it is externally provided
    }

    template<typename T, uint C>
    bool BitmapWayland<T, C>::CreateShmBuffer(uint w, uint h, struct wl_shm* wayland_shm)
    {
        if (!w || !h || !wayland_shm)
            return false;

        // Clean up old resources
        if (buffer)
        {
            wl_buffer_destroy(buffer);
            buffer = nullptr;
        }

        if (pool)
        {
            wl_shm_pool_destroy(pool);
            pool = nullptr;
        }

        if (shm_data && shm_data != MAP_FAILED)
        {
            munmap(shm_data, shm_size);
            shm_data = nullptr;
        }

        if (shm_fd >= 0)
        {
            close(shm_fd);
            shm_fd = -1;
        }

        this->data = nullptr;
        this->width = w;
        this->height = h;
        shm = wayland_shm;

        // Calculate required memory size
        size_t stride = w * sizeof(T);
        shm_size = stride * h;

        // Create shared memory file
        shm_fd = create_anonymous_file(shm_size);
        if (shm_fd < 0)
            return false;

        // Map shared memory
        shm_data = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shm_data == MAP_FAILED)
        {
            close(shm_fd);
            shm_fd = -1;
            return false;
        }

        memset(shm_data, 0, shm_size);

        // Create wl_shm_pool
        pool = wl_shm_create_pool(shm, shm_fd, shm_size);
        if (!pool)
        {
            munmap(shm_data, shm_size);
            shm_data = nullptr;
            close(shm_fd);
            shm_fd = -1;
            return false;
        }

        // Determine pixel format
        uint32_t format = WL_SHM_FORMAT_ARGB8888;  // Default format
        if (C == 4)
            format = WL_SHM_FORMAT_ARGB8888;
        else if (C == 3)
            format = WL_SHM_FORMAT_XRGB8888;

        // Create buffer
        buffer = wl_shm_pool_create_buffer(pool, 0, w, h, stride, format);
        if (!buffer)
        {
            wl_shm_pool_destroy(pool);
            pool = nullptr;
            munmap(shm_data, shm_size);
            shm_data = nullptr;
            close(shm_fd);
            shm_fd = -1;
            return false;
        }

        // Point base class data pointer to shared memory
        this->data = (T*)shm_data;

        return true;
    }

    template<typename T, uint C>
    bool BitmapWayland<T, C>::AttachAndCommit(struct wl_surface* surface, int32_t x, int32_t y) const
    {
        if (!surface || !buffer || !this->data)
            return false;

        wl_surface_attach(surface, buffer, x, y);
        wl_surface_damage(surface, 0, 0, this->width, this->height);
        wl_surface_commit(surface);

        return true;
    }

    // Explicit instantiation of common types
    template class BitmapWayland<math::Vector4u8, 4>;
    template class BitmapWayland<math::Vector3u8, 3>;
    template class BitmapWayland<math::Vector2u8, 2>;
    template class BitmapWayland<uint8, 1>;

} // namespace hgl::bitmap

#endif // defined(__linux__) && defined(HGL_WAYLAND_SUPPORT)
