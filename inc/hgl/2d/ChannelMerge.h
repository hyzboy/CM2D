#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>

/**
 * 通道合并模块
 *
 * 提供将多个单通道位图合并为多通道位图的功能。
 * 这是通道分离操作的逆过程。
 *
 * 示例用法：
 * ```cpp
 * BitmapGrey8 r, g, b, a;
 * // ... 初始化各通道 ...
 *
 * // 合并 3 个通道为 RGB
 * auto rgb = hgl::bitmap::channel::MergeRGB(r, g, b);
 *
 * // 合并 4 个通道为 RGBA
 * auto rgba = hgl::bitmap::channel::MergeRGBA(r, g, b, a);
 *
 * // 合并 RGB + A 为 RGBA
 * BitmapRGB8 rgb_bitmap;
 * BitmapGrey8 alpha;
 * auto rgba2 = hgl::bitmap::channel::MergeRGB_A_To_RGBA(rgb_bitmap, alpha);
 * ```
 */

namespace hgl::bitmap::channel
{
    /**
     * 合并 4 个单通道位图为 RGBA 位图
     * @param r 红色通道
     * @param g 绿色通道
     * @param b 蓝色通道
     * @param a Alpha 通道
     * @return RGBA 位图，若输入无效或尺寸不一致则返回 nullptr
     */
    inline BitmapRGBA8* MergeRGBA(const BitmapGrey8& r, const BitmapGrey8& g,
                                  const BitmapGrey8& b, const BitmapGrey8& a)
    {
        const int w = r.GetWidth();
        const int h = r.GetHeight();

        // 检查所有通道的尺寸是否相同
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
     * 合并 3 个单通道位图为 RGB 位图
     * @param r 红色通道
     * @param g 绿色通道
     * @param b 蓝色通道
     * @return RGB 位图，若输入无效或尺寸不一致则返回 nullptr
     */
    inline BitmapRGB8* MergeRGB(const BitmapGrey8& r, const BitmapGrey8& g, const BitmapGrey8& b)
    {
        const int w = r.GetWidth();
        const int h = r.GetHeight();

        // 检查所有通道的尺寸是否相同
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
     * 合并 2 个单通道位图为 RG 位图
     * @param r 红色/第一个通道
     * @param g 绿色/第二个通道
     * @return RG 位图，若输入无效或尺寸不一致则返回 nullptr
     */
    inline BitmapRG8* MergeRG(const BitmapGrey8& r, const BitmapGrey8& g)
    {
        const int w = r.GetWidth();
        const int h = r.GetHeight();

        // 检查两个通道的尺寸是否相同
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
     * 合并 RGB 位图与 Alpha 通道为 RGBA 位图
     * @param rgb RGB 位图
     * @param a Alpha 通道
     * @return RGBA 位图，若输入无效或尺寸不一致则返回 nullptr
     */
    inline BitmapRGBA8* MergeRGB_A_To_RGBA(const BitmapRGB8& rgb, const BitmapGrey8& a)
    {
        const int w = rgb.GetWidth();
        const int h = rgb.GetHeight();

        // 检查尺寸是否匹配
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
     * 合并 RG 位图与 B 通道为 RGB 位图
     * 适用于 YUV 等格式（如 Y+UV），需要补充第三通道
     * @param rg RG 位图（或 YU）
     * @param b 蓝色/第三通道（或 V）
     * @return RGB 位图，若输入无效或尺寸不一致则返回 nullptr
     */
    inline BitmapRGB8* MergeRG_B_To_RGB(const BitmapRG8& rg, const BitmapGrey8& b)
    {
        const int w = rg.GetWidth();
        const int h = rg.GetHeight();

        // 检查尺寸是否匹配
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
