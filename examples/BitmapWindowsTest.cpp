/**
 * BitmapWindows Verification Test
 * - DIB pixel roundtrip
 * - RGB->BGRX swizzle correctness (GDI DIB byte order)
 * - Premultiplied-alpha conversion (AC_SRC_ALPHA requirement)
 * - Create()/Clear() override: DIB-safe lifecycle (was delete[] on DIB memory)
 * - Link check: BlitTo/StretchBlitTo/AlphaBlitTo resolve for the public
 *   aliases (previously the explicit instantiation used math::Vector4u8
 *   while the aliases use Color4ub -> LNK2019)
 */

#include<hgl/2d/BitmapPlatform.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<iostream>
#include<vector>
#include<cstdint>

using namespace hgl;
using namespace hgl::bitmap;

static int failures = 0;

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { std::cerr << "FAIL: " << msg << std::endl; ++failures; } \
        else { std::cout << "  ok: " << msg << std::endl; } \
    } while (0)

int main()
{
    // ==================== CreateDIB + pixel roundtrip ====================
    std::cout << "CreateDIB..." << std::endl;

    BitmapRGBA8Windows bmp;
    CHECK(bmp.CreateDIB(64, 64), "CreateDIB(64,64)");
    CHECK(bmp.GetWidth() == 64 && bmp.GetHeight() == 64, "DIB size");
    CHECK(bmp.GetData() != nullptr, "DIB data pointer");

    bmp.GetData()[0] = Color4ub(1, 2, 3, 4);
    CHECK(bmp.GetData()[0] == Color4ub(1, 2, 3, 4), "pixel roundtrip through DIB memory");

    // ==================== RGB->BGRX swizzle ====================
    std::cout << "ConvertPixelsToBGRX..." << std::endl;

    std::vector<uint32> dst(4);
    const Color4ub px[4] = {
        {255, 0, 0, 255},     // red
        {0, 255, 0, 255},     // green
        {0, 0, 255, 255},     // blue
        {255, 255, 255, 255}  // white
    };

    ConvertPixelsToBGRX<Color4ub, 4>(px, dst.data(), 4, 1, 255, false);

    // DIB 32bpp layout: byte0=B, byte1=G, byte2=R, byte3=X  =>  X<<24|R<<16|G<<8|B
    CHECK(dst[0] == 0xFFFF0000u, "swizzle: red lands in R channel (was B before fix)");
    CHECK(dst[1] == 0xFF00FF00u, "swizzle: green lands in G channel");
    CHECK(dst[2] == 0xFF0000FFu, "swizzle: blue lands in B channel");
    CHECK(dst[3] == 0xFFFFFFFFu, "swizzle: white");

    // ==================== grayscale passthrough ====================
    std::cout << "Grayscale..." << std::endl;

    const uint8 grey[2] = { 200, 100 };
    std::vector<uint32> dst_grey(2);
    ConvertPixelsToBGRX<uint8, 1>(grey, dst_grey.data(), 2, 1, 255, false);
    CHECK(dst_grey[0] == 0xFFC8C8C8u, "grey: 200 -> 0xFFC8C8C8");
    CHECK(dst_grey[1] == 0xFF646464u, "grey: 100 -> 0xFF646464");

    // ==================== premultiplied alpha ====================
    std::cout << "Premultiply..." << std::endl;

    const Color4ub px_red[1] = { {255, 0, 0, 255} };
    std::vector<uint32> dst_pm(1);
    ConvertPixelsToBGRX<Color4ub, 4>(px_red, dst_pm.data(), 1, 1, 128, true);
    // a_scale=128/255: r' = 255*128/255 = 128 (0x80), a' = 128 (0x80)
    CHECK(dst_pm[0] == 0x80800000u, "premultiply: red@128 -> 0x80800000");

    const Color4ub px_half[1] = { {255, 0, 0, 128} };
    ConvertPixelsToBGRX<Color4ub, 4>(px_half, dst_pm.data(), 1, 1, 255, true);
    // a=128: r' = 255*128/255 = 128, a' = 128
    CHECK(dst_pm[0] == 0x80800000u, "premultiply: pixel alpha 128 -> 0x80800000");

    // ==================== Create/Clear override (DIB-safe) ====================
    std::cout << "Create/Clear override..." << std::endl;

    BitmapRGBA8Windows bmp2;
    CHECK(bmp2.Create(32, 32), "Create(32,32) routed to DIB");
    CHECK(bmp2.GetData() != nullptr, "Create -> DIB memory (not new[] heap)");
    bmp2.GetData()[5] = Color4ub(9, 9, 9, 9);

    bmp2.Clear();
    CHECK(bmp2.GetData() == nullptr, "Clear frees DIB and nulls data");
    CHECK(bmp2.GetWidth() == 0 && bmp2.GetHeight() == 0, "Clear zeroes size");

    CHECK(bmp2.Create(16, 16), "re-Create after Clear");
    CHECK(bmp2.GetData() != nullptr && bmp2.GetWidth() == 16, "re-Create works");

    // inherited GetData(x,y) still works on DIB memory
    CHECK(bmp2.GetData(3, 3) != nullptr, "GetData(x,y) on DIB");

    // ==================== link check (LNK2019 regression) ====================
    std::cout << "Link check..." << std::endl;

    bool (BitmapRGBA8Windows::*fn_blit)(HDC, int, int, int, int, int, int) const = &BitmapRGBA8Windows::BlitTo;
    bool (BitmapRGBA8Windows::*fn_stretch)(HDC, int, int, int, int, int, int, int, int) const = &BitmapRGBA8Windows::StretchBlitTo;
    bool (BitmapRGBA8Windows::*fn_alpha)(HDC, int, int, int, int, int, int, int, int, uint8) const = &BitmapRGBA8Windows::AlphaBlitTo;

    CHECK(fn_blit != nullptr, "BlitTo resolves for BitmapRGBA8Windows");
    CHECK(fn_stretch != nullptr, "StretchBlitTo resolves");
    CHECK(fn_alpha != nullptr, "AlphaBlitTo resolves");

    // ==================== lifecycle stress ====================
    std::cout << "Lifecycle stress..." << std::endl;

    BitmapRGBA8Windows bmp3;
    bool stress_ok = true;
    for (int i = 0; i < 100; ++i)
    {
        if (!bmp3.Create(8 + i % 32, 8 + i % 16)) { stress_ok = false; break; }
        bmp3.Clear();
    }
    CHECK(stress_ok, "100x Create/Clear cycle without crash");

    if (failures == 0)
    {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed" << std::endl;
    return 1;
}
