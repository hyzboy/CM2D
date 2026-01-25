#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>

/**
 * Channel Merge Module
 *
 * Provides functionality to merge/combine separate single-channel bitmaps
 * into multi-channel bitmaps. This is the reverse operation of channel splitting.
 *
 * Example usage:
 * ```cpp
 * BitmapGrey8 r, g, b, a;
 * // ... initialize channels ...
 *
 * // Merge 3 channels into RGB
 * auto rgb = hgl::bitmap::channel::MergeRGB(r, g, b);
 *
 * // Merge 4 channels into RGBA
 * auto rgba = hgl::bitmap::channel::MergeRGBA(r, g, b, a);
 *
 * // Merge RGB + A into RGBA
 * BitmapRGB8 rgb_bitmap;
 * BitmapGrey8 alpha;
 * auto rgba2 = hgl::bitmap::channel::MergeRGB_A_To_RGBA(rgb_bitmap, alpha);
 * ```
 */

namespace hgl::bitmap::channel
{
    /**
     * Merge 4 single-channel bitmaps into RGBA bitmap
     * @param r Red channel
     * @param g Green channel
     * @param b Blue channel
     * @param a Alpha channel
     * @return RGBA bitmap, or nullptr if inputs are invalid or dimensions don't match
     */
    inline BitmapRGBA8* MergeRGBA(const BitmapGrey8& r, const BitmapGrey8& g,
                                  const BitmapGrey8& b, const BitmapGrey8& a)
    {
        const int w = r.GetWidth();
        const int h = r.GetHeight();

        // Check all channels have same dimensions
        if (w != g.GetWidth() || w != b.GetWidth() || w != a.GetWidth() ||
            h != g.GetHeight() || h != b.GetHeight() || h != a.GetHeight())
            return nullptr;

        const uint8* r_data = r.GetData();
        const uint8* g_data = g.GetData();
        const uint8* b_data = b.GetData();
        const uint8* a_data = a.GetData();

        if (!r_data || !g_data || !b_data || !a_data || w <= 0 || h <= 0)
            return nullptr;

        BitmapRGBA8* result = new BitmapRGBA8();
        if (!result->Create(w, h))
        {
            delete result;
            return nullptr;
        }

        Color4ub* result_data = reinterpret_cast<Color4ub*>(result->GetData());
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            result_data[i].r = r_data[i];
            result_data[i].g = g_data[i];
            result_data[i].b = b_data[i];
            result_data[i].a = a_data[i];
        }

        return result;
    }

    /**
     * Merge 3 single-channel bitmaps into RGB bitmap
     * @param r Red channel
     * @param g Green channel
     * @param b Blue channel
     * @return RGB bitmap, or nullptr if inputs are invalid or dimensions don't match
     */
    inline BitmapRGB8* MergeRGB(const BitmapGrey8& r, const BitmapGrey8& g, const BitmapGrey8& b)
    {
        const int w = r.GetWidth();
        const int h = r.GetHeight();

        // Check all channels have same dimensions
        if (w != g.GetWidth() || w != b.GetWidth() ||
            h != g.GetHeight() || h != b.GetHeight())
            return nullptr;

        const uint8* r_data = r.GetData();
        const uint8* g_data = g.GetData();
        const uint8* b_data = b.GetData();

        if (!r_data || !g_data || !b_data || w <= 0 || h <= 0)
            return nullptr;

        BitmapRGB8* result = new BitmapRGB8();
        if (!result->Create(w, h))
        {
            delete result;
            return nullptr;
        }

        Color3ub* result_data = reinterpret_cast<Color3ub*>(result->GetData());
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            result_data[i].r = r_data[i];
            result_data[i].g = g_data[i];
            result_data[i].b = b_data[i];
        }

        return result;
    }

    /**
     * Merge 2 single-channel bitmaps into RG bitmap
     * @param r Red/First channel
     * @param g Green/Second channel
     * @return RG bitmap, or nullptr if inputs are invalid or dimensions don't match
     */
    inline BitmapRG8* MergeRG(const BitmapGrey8& r, const BitmapGrey8& g)
    {
        const int w = r.GetWidth();
        const int h = r.GetHeight();

        // Check both channels have same dimensions
        if (w != g.GetWidth() || h != g.GetHeight())
            return nullptr;

        const uint8* r_data = r.GetData();
        const uint8* g_data = g.GetData();

        if (!r_data || !g_data || w <= 0 || h <= 0)
            return nullptr;

        BitmapRG8* result = new BitmapRG8();
        if (!result->Create(w, h))
        {
            delete result;
            return nullptr;
        }

        math::Vector2u8* result_data = result->GetData();
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            result_data[i].r = r_data[i];
            result_data[i].g = g_data[i];
        }

        return result;
    }

    /**
     * Merge RGB bitmap and Alpha channel into RGBA bitmap
     * @param rgb RGB bitmap
     * @param a Alpha channel
     * @return RGBA bitmap, or nullptr if inputs are invalid or dimensions don't match
     */
    inline BitmapRGBA8* MergeRGB_A_To_RGBA(const BitmapRGB8& rgb, const BitmapGrey8& a)
    {
        const int w = rgb.GetWidth();
        const int h = rgb.GetHeight();

        // Check dimensions match
        if (w != a.GetWidth() || h != a.GetHeight())
            return nullptr;

        const Color3ub* rgb_data = reinterpret_cast<const Color3ub*>(rgb.GetData());
        const uint8* a_data = a.GetData();

        if (!rgb_data || !a_data || w <= 0 || h <= 0)
            return nullptr;

        BitmapRGBA8* result = new BitmapRGBA8();
        if (!result->Create(w, h))
        {
            delete result;
            return nullptr;
        }

        Color4ub* result_data = reinterpret_cast<Color4ub*>(result->GetData());
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            result_data[i].r = rgb_data[i].r;
            result_data[i].g = rgb_data[i].g;
            result_data[i].b = rgb_data[i].b;
            result_data[i].a = a_data[i];
        }

        return result;
    }

    /**
     * Merge RG bitmap and B channel into RGB bitmap
     * Useful for YUV-like formats where you have Y+UV and want to add a third channel
     * @param rg RG bitmap (or YU)
     * @param b Blue/Third channel (or V)
     * @return RGB bitmap, or nullptr if inputs are invalid or dimensions don't match
     */
    inline BitmapRGB8* MergeRG_B_To_RGB(const BitmapRG8& rg, const BitmapGrey8& b)
    {
        const int w = rg.GetWidth();
        const int h = rg.GetHeight();

        // Check dimensions match
        if (w != b.GetWidth() || h != b.GetHeight())
            return nullptr;

        const math::Vector2u8* rg_data = rg.GetData();
        const uint8* b_data = b.GetData();

        if (!rg_data || !b_data || w <= 0 || h <= 0)
            return nullptr;

        BitmapRGB8* result = new BitmapRGB8();
        if (!result->Create(w, h))
        {
            delete result;
            return nullptr;
        }

        Color3ub* result_data = reinterpret_cast<Color3ub*>(result->GetData());
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            result_data[i].r = rg_data[i].r;
            result_data[i].g = rg_data[i].g;
            result_data[i].b = b_data[i];
        }

        return result;
    }

} // namespace hgl::bitmap::channel
