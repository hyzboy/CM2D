#ifdef _WIN32

#include<hgl/2d/BitmapPlatform.h>
#include<windows.h>

namespace hgl::bitmap
{
    template<typename T, uint C>
    BitmapWindows<T, C>::BitmapWindows()
        : Bitmap<T, C>()
    {
        hBitmap = nullptr;
        memDC = nullptr;
        hOldBitmap = nullptr;
    }

    template<typename T, uint C>
    BitmapWindows<T, C>::~BitmapWindows()
    {
        // Must set data to nullptr before releasing platform resources to prevent base class double-free
        this->data = nullptr;

        if (memDC)
        {
            if (hOldBitmap)
                SelectObject(memDC, hOldBitmap);
            DeleteDC(memDC);
        }

        if (hBitmap)
            DeleteObject(hBitmap);
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::CreateDIB(uint w, uint h, HDC hdc)
    {
        if (!w || !h)
            return false;

        // Clean up old resources
        if (memDC)
        {
            if (hOldBitmap)
                SelectObject(memDC, hOldBitmap);
            DeleteDC(memDC);
            memDC = nullptr;
            hOldBitmap = nullptr;
        }

        if (hBitmap)
        {
            DeleteObject(hBitmap);
            hBitmap = nullptr;
        }

        this->data = nullptr;
        this->width = w;
        this->height = h;

        // Create memory DC
        HDC screenDC = hdc ? hdc : GetDC(nullptr);
        memDC = CreateCompatibleDC(screenDC);
        if (!memDC)
        {
            if (!hdc)
                ReleaseDC(nullptr, screenDC);
            return false;
        }

        // Prepare BITMAPINFO
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -(int)h;  // Negative value indicates top-down DIB
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = sizeof(T) * 8;
        bmi.bmiHeader.biCompression = BI_RGB;

        // Create DIB Section
        void* bits = nullptr;
        hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

        if (!hdc)
            ReleaseDC(nullptr, screenDC);

        if (!hBitmap || !bits)
        {
            if (memDC)
            {
                DeleteDC(memDC);
                memDC = nullptr;
            }
            return false;
        }

        // Select DIB Section into memory DC
        hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

        // Point base class data pointer to DIB Section memory
        this->data = (T*)bits;

        return true;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::BlitTo(HDC hdc, int dx, int dy, int dw, int dh, int sx, int sy) const
    {
        if (!memDC || !hBitmap || !hdc)
            return false;

        return BitBlt(hdc, dx, dy, dw, dh, memDC, sx, sy, SRCCOPY) != FALSE;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::StretchBlitTo(HDC hdc, int dx, int dy, int dw, int dh, 
                                           int sx, int sy, int sw, int sh) const
    {
        if (!memDC || !hBitmap || !hdc)
            return false;

        int oldMode = SetStretchBltMode(hdc, HALFTONE);
        bool result = StretchBlt(hdc, dx, dy, dw, dh, memDC, sx, sy, sw, sh, SRCCOPY) != FALSE;
        SetStretchBltMode(hdc, oldMode);

        return result;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::AlphaBlitTo(HDC hdc, int dx, int dy, int dw, int dh, 
                                         int sx, int sy, int sw, int sh, uint8 alpha) const
    {
        if (!memDC || !hBitmap || !hdc)
            return false;

        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = alpha;
        blend.AlphaFormat = (C == 4) ? AC_SRC_ALPHA : 0;  // Only RGBA format uses per-pixel alpha

        return AlphaBlend(hdc, dx, dy, dw, dh, memDC, sx, sy, sw, sh, blend) != FALSE;
    }

    // Explicit instantiation of common types
    template class BitmapWindows<math::Vector4u8, 4>;
    template class BitmapWindows<math::Vector3u8, 3>;
    template class BitmapWindows<math::Vector2u8, 2>;
    template class BitmapWindows<uint8, 1>;

} // namespace hgl::bitmap

#endif // _WIN32
