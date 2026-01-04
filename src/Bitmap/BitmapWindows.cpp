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
        // 必须在释放平台资源前将data设为nullptr，防止基类重复释放
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

        // 清理旧资源
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

        // 创建内存DC
        HDC screenDC = hdc ? hdc : GetDC(nullptr);
        memDC = CreateCompatibleDC(screenDC);
        if (!memDC)
        {
            if (!hdc)
                ReleaseDC(nullptr, screenDC);
            return false;
        }

        // 准备BITMAPINFO
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -(int)h;  // 负值表示top-down DIB
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = sizeof(T) * 8;
        bmi.bmiHeader.biCompression = BI_RGB;

        // 创建DIB Section
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

        // 将DIB Section选入内存DC
        hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

        // 将基类的data指针指向DIB Section的内存
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
        blend.AlphaFormat = (C == 4) ? AC_SRC_ALPHA : 0;  // 只有RGBA格式使用per-pixel alpha

        return AlphaBlend(hdc, dx, dy, dw, dh, memDC, sx, sy, sw, sh, blend) != FALSE;
    }

    // 显式实例化常用类型
    template class BitmapWindows<math::Vector4u8, 4>;
    template class BitmapWindows<math::Vector3u8, 3>;
    template class BitmapWindows<math::Vector2u8, 2>;
    template class BitmapWindows<uint8, 1>;

} // namespace hgl::bitmap

#endif // _WIN32
