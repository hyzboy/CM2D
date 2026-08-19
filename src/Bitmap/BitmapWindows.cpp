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

        scratchBitmap = nullptr;
        scratchDC = nullptr;
        scratchOldBitmap = nullptr;
        scratchBits = nullptr;
        scratchW = scratchH = 0;
    }

    template<typename T, uint C>
    BitmapWindows<T, C>::~BitmapWindows()
    {
        // 释放 DIB 资源并置 data=nullptr，防止基类析构 delete[] 掉 CreateDIBSection 分配的内存
        Clear();
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::Create(uint w, uint h)
    {
        return CreateDIB(w, h);
    }

    template<typename T, uint C>
    void BitmapWindows<T, C>::Clear()
    {
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

        if (scratchDC)
        {
            if (scratchOldBitmap)
                SelectObject(scratchDC, scratchOldBitmap);
            DeleteDC(scratchDC);
            scratchDC = nullptr;
            scratchOldBitmap = nullptr;
        }

        if (scratchBitmap)
        {
            DeleteObject(scratchBitmap);
            scratchBitmap = nullptr;
        }

        scratchBits = nullptr;
        scratchW = scratchH = 0;

        this->data = nullptr;
        this->width = this->height = 0;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::EnsureScratch(uint w, uint h) const
    {
        if (scratchDC && scratchW == (int)w && scratchH == (int)h)
            return true;

        if (scratchDC)
        {
            if (scratchOldBitmap)
                SelectObject(scratchDC, scratchOldBitmap);
            DeleteDC(scratchDC);
            scratchDC = nullptr;
            scratchOldBitmap = nullptr;
        }

        if (scratchBitmap)
        {
            DeleteObject(scratchBitmap);
            scratchBitmap = nullptr;
        }

        scratchBits = nullptr;
        scratchW = scratchH = 0;

        HDC screenDC = GetDC(nullptr);
        scratchDC = CreateCompatibleDC(screenDC);

        if (scratchDC)
        {
            // 32bpp top-down DIB（BGRX 字节序）
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = w;
            bmi.bmiHeader.biHeight = -(int)h;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;

            void *bits = nullptr;
            scratchBitmap = CreateDIBSection(scratchDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
            scratchBits = bits;
        }

        if (screenDC)
            ReleaseDC(nullptr, screenDC);

        if (!scratchBitmap || !scratchBits)
        {
            if (scratchDC)
            {
                DeleteDC(scratchDC);
                scratchDC = nullptr;
            }
            return false;
        }

        scratchOldBitmap = (HBITMAP)SelectObject(scratchDC, scratchBitmap);
        scratchW = (int)w;
        scratchH = (int)h;
        return true;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::SwizzleToScratch(uint8 alpha, bool premultiply) const
    {
        if (!this->data || !scratchBits)
            return false;
        if (scratchW != this->width || scratchH != this->height)
            return false;

        ConvertPixelsToBGRX<T, C>(this->data, static_cast<uint32 *>(scratchBits),
                                  this->width, this->height, alpha, premultiply);
        return true;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::CreateDIB(uint w, uint h, HDC hdc)
    {
        if (!w || !h)
            return false;

        // Clean up old resources (including scratch)
        Clear();

        this->width = w;
        this->height = h;

        // Create memory DC
        HDC screenDC = hdc ? hdc : GetDC(nullptr);
        memDC = CreateCompatibleDC(screenDC);

        if (!memDC)
        {
            if (!hdc && screenDC)
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
        void *bits = nullptr;
        hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

        if (!hdc && screenDC)
            ReleaseDC(nullptr, screenDC);

        if (!hBitmap || !bits)
        {
            if (hBitmap)
            {
                DeleteObject(hBitmap);
                hBitmap = nullptr;
            }
            if (memDC)
            {
                DeleteDC(memDC);
                memDC = nullptr;
            }
            this->width = this->height = 0;
            return false;
        }

        // Select DIB Section into memory DC
        hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

        // Point base class data pointer to DIB Section memory
        this->data = (T *)bits;

        return true;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::BlitTo(HDC hdc, int dx, int dy, int dw, int dh, int sx, int sy) const
    {
        if (!memDC || !hBitmap || !hdc)
            return false;

        if (!EnsureScratch(this->width, this->height))
            return false;

        if (!SwizzleToScratch(255, false))
            return false;

        return BitBlt(hdc, dx, dy, dw, dh, scratchDC, sx, sy, SRCCOPY) != FALSE;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::StretchBlitTo(HDC hdc, int dx, int dy, int dw, int dh,
                                            int sx, int sy, int sw, int sh) const
    {
        if (!memDC || !hBitmap || !hdc)
            return false;

        if (!EnsureScratch(this->width, this->height))
            return false;

        if (!SwizzleToScratch(255, false))
            return false;

        int oldMode = SetStretchBltMode(hdc, HALFTONE);
        bool result = StretchBlt(hdc, dx, dy, dw, dh, scratchDC, sx, sy, sw, sh, SRCCOPY) != FALSE;
        SetStretchBltMode(hdc, oldMode);

        return result;
    }

    template<typename T, uint C>
    bool BitmapWindows<T, C>::AlphaBlitTo(HDC hdc, int dx, int dy, int dw, int dh,
                                          int sx, int sy, int sw, int sh, uint8 alpha) const
    {
        if (!memDC || !hBitmap || !hdc)
            return false;

        if (!EnsureScratch(this->width, this->height))
            return false;

        // C==4: 全局 alpha 已预乘进像素，SourceConstantAlpha 必须为 255 防二次乘
        // C!=4: 无每像素 alpha，全局 alpha 由 SourceConstantAlpha 承担
        const bool premultiply = (C == 4);

        if (!SwizzleToScratch(alpha, premultiply))
            return false;

        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = premultiply ? 255 : alpha;
        blend.AlphaFormat = (C == 4) ? AC_SRC_ALPHA : 0;

        return AlphaBlend(hdc, dx, dy, dw, dh, scratchDC, sx, sy, sw, sh, blend) != FALSE;
    }

    // Explicit instantiation must match the public aliases:
    // BitmapRGBA8Windows=BitmapWindows<Color4ub,4>, BitmapRGB8Windows=BitmapWindows<Color3ub,3>,
    // BitmapRG8Windows=BitmapWindows<math::Vector2u8,2>, BitmapGrey8Windows=BitmapWindows<uint8,1>
    template class BitmapWindows<Color4ub, 4>;
    template class BitmapWindows<Color3ub, 3>;
    template class BitmapWindows<math::Vector2u8, 2>;
    template class BitmapWindows<uint8, 1>;

} // namespace hgl::bitmap

#endif // _WIN32
