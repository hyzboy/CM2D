#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<cstring>
#include<tuple>

/**
 * 通道分离模块
 *
 * 提供将多通道位图分离为单通道或多通道位图的功能。支持多种分离模式，例如：
 * - RGB -> R + G + B（3 个单通道）
 * - RGBA -> RGB + A（3 通道 + 1 通道）
 * - RGBA -> R + G + B + A（4 个单通道）
 * - YUV -> Y + UV（1 通道 + 2 通道）
 *
 * 所有函数均为值语义：返回按值持有的 Bitmap，无需手动释放。
 *
 * 示例用法：
 * ```cpp
 * BitmapRGBA8 source;
 * source.Create(100, 100);
 *
 * // 将 RGBA 分离为 4 个独立通道
 * auto [r, g, b, a] = hgl::bitmap::channel::SplitRGBA(source);
 *
 * // 将 RGBA 分离为 RGB + A
 * auto [rgb, alpha] = hgl::bitmap::channel::SplitRGBA_To_RGB_A(source);
 *
 * // 将 RGB 分离为单独通道
 * auto [red, green, blue] = hgl::bitmap::channel::SplitRGB(source);
 * ```
 */

namespace hgl::bitmap::channel
{
    /**
     * 将 RGBA 位图分离为 4 个单通道位图（R、G、B、A）
     * @param src 源 RGBA 位图，像素类型为 Color4ub
     * @return 4 个单通道位图（R、G、B、A）的元组
     */
    inline std::tuple<BitmapGrey8, BitmapGrey8, BitmapGrey8, BitmapGrey8> SplitRGBA(const BitmapRGBA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const Color4ub* src_data = reinterpret_cast<const Color4ub*>(src.GetData());

        BitmapGrey8 r_channel, g_channel, b_channel, a_channel;

        if (!src_data || w <= 0 || h <= 0)
            return {};

        if (!r_channel.Create(w, h) || !g_channel.Create(w, h) ||
            !b_channel.Create(w, h) || !a_channel.Create(w, h))
            return {};

        uint8* r_data = r_channel.GetData();
        uint8* g_data = g_channel.GetData();
        uint8* b_data = b_channel.GetData();
        uint8* a_data = a_channel.GetData();

        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            r_data[i] = src_data[i].r;
            g_data[i] = src_data[i].g;
            b_data[i] = src_data[i].b;
            a_data[i] = src_data[i].a;
        }

        return {std::move(r_channel), std::move(g_channel), std::move(b_channel), std::move(a_channel)};
    }

    /**
     * 将 RGB 位图分离为 3 个单通道位图（R、G、B）
     * @param src 源 RGB 位图，像素类型为 Color3ub
     * @return 3 个单通道位图（R、G、B）的元组
     */
    inline std::tuple<BitmapGrey8, BitmapGrey8, BitmapGrey8> SplitRGB(const BitmapRGB8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const Color3ub* src_data = reinterpret_cast<const Color3ub*>(src.GetData());

        BitmapGrey8 r_channel, g_channel, b_channel;

        if (!src_data || w <= 0 || h <= 0)
            return {};

        if (!r_channel.Create(w, h) || !g_channel.Create(w, h) || !b_channel.Create(w, h))
            return {};

        uint8* r_data = r_channel.GetData();
        uint8* g_data = g_channel.GetData();
        uint8* b_data = b_channel.GetData();

        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            r_data[i] = src_data[i].r;
            g_data[i] = src_data[i].g;
            b_data[i] = src_data[i].b;
        }

        return {std::move(r_channel), std::move(g_channel), std::move(b_channel)};
    }

    /**
     * 将 RG 位图分离为 2 个单通道位图（R、G）
     * @param src 源 RG 位图，像素类型为 Vector2u8
     * @return 2 个单通道位图（R、G）的元组
     */
    inline std::tuple<BitmapGrey8, BitmapGrey8> SplitRG(const BitmapRG8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const math::Vector2u8* src_data = src.GetData();

        BitmapGrey8 r_channel, g_channel;

        if (!src_data || w <= 0 || h <= 0)
            return {};

        if (!r_channel.Create(w, h) || !g_channel.Create(w, h))
            return {};

        uint8* r_data = r_channel.GetData();
        uint8* g_data = g_channel.GetData();

        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            r_data[i] = src_data[i].r;
            g_data[i] = src_data[i].g;
        }

        return {std::move(r_channel), std::move(g_channel)};
    }

    /**
     * 将 RGBA 位图分离为 RGB（三通道）+ A（单通道）
     * @param src 源 RGBA 位图，像素类型为 Color4ub
     * @return RGB 位图和 Alpha 通道位图的元组
     */
    inline std::tuple<BitmapRGB8, BitmapGrey8> SplitRGBA_To_RGB_A(const BitmapRGBA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const Color4ub* src_data = reinterpret_cast<const Color4ub*>(src.GetData());

        BitmapRGB8 rgb_bitmap;
        BitmapGrey8 a_channel;

        if (!src_data || w <= 0 || h <= 0)
            return {};

        if (!rgb_bitmap.Create(w, h) || !a_channel.Create(w, h))
            return {};

        Color3ub* rgb_data = reinterpret_cast<Color3ub*>(rgb_bitmap.GetData());
        uint8* a_data = a_channel.GetData();

        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            rgb_data[i].r = src_data[i].r;
            rgb_data[i].g = src_data[i].g;
            rgb_data[i].b = src_data[i].b;
            a_data[i] = src_data[i].a;
        }

        return {std::move(rgb_bitmap), std::move(a_channel)};
    }

    /**
     * 从 RGBA 位图中按索引提取单个通道
     * @param src 源 RGBA 位图
     * @param channel_index 通道索引（0=R，1=G，2=B，3=A）
     * @return 单通道位图，若索引无效则返回空位图
     */
    inline BitmapGrey8 ExtractChannel(const BitmapRGBA8& src, uint channel_index)
    {
        BitmapGrey8 channel;

        if (channel_index >= 4)
            return channel;

        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const Color4ub* src_data = reinterpret_cast<const Color4ub*>(src.GetData());

        if (!src_data || w <= 0 || h <= 0)
            return channel;

        if (!channel.Create(w, h))
            return channel;

        uint8* channel_data = channel.GetData();
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            if (channel_index == 0)
                channel_data[i] = src_data[i].r;
            else if (channel_index == 1)
                channel_data[i] = src_data[i].g;
            else if (channel_index == 2)
                channel_data[i] = src_data[i].b;
            else
                channel_data[i] = src_data[i].a;
        }

        return channel;
    }

    /**
     * 从 RGB 位图中按索引提取单个通道
     * @param src 源 RGB 位图
     * @param channel_index 通道索引（0=R，1=G，2=B）
     * @return 单通道位图，若索引无效则返回空位图
     */
    inline BitmapGrey8 ExtractChannel(const BitmapRGB8& src, uint channel_index)
    {
        BitmapGrey8 channel;

        if (channel_index >= 3)
            return channel;

        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const Color3ub* src_data = reinterpret_cast<const Color3ub*>(src.GetData());

        if (!src_data || w <= 0 || h <= 0)
            return channel;

        if (!channel.Create(w, h))
            return channel;

        uint8* channel_data = channel.GetData();
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            if (channel_index == 0)
                channel_data[i] = src_data[i].r;
            else if (channel_index == 1)
                channel_data[i] = src_data[i].g;
            else
                channel_data[i] = src_data[i].b;
        }

        return channel;
    }

    /**
     * 从 RG 位图中按索引提取单个通道
     * @param src 源 RG 位图
     * @param channel_index 通道索引（0=R，1=G）
     * @return 单通道位图，若索引无效则返回空位图
     */
    inline BitmapGrey8 ExtractChannel(const BitmapRG8& src, uint channel_index)
    {
        BitmapGrey8 channel;

        if (channel_index >= 2)
            return channel;

        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const math::Vector2u8* src_data = src.GetData();

        if (!src_data || w <= 0 || h <= 0)
            return channel;

        if (!channel.Create(w, h))
            return channel;

        uint8* channel_data = channel.GetData();
        const int total = w * h;

        for (int i = 0; i < total; ++i)
        {
            channel_data[i] = (channel_index == 0) ? src_data[i].r : src_data[i].g;
        }

        return channel;
    }

    /**
     * 从单通道位图中提取通道（直接复制）
     * @param src 源单通道位图
     * @param channel_index 必须为 0
     * @return 源位图的副本，若索引无效则返回空位图
     */
    inline BitmapGrey8 ExtractChannel(const BitmapGrey8& src, uint channel_index)
    {
        BitmapGrey8 channel;

        if (channel_index != 0)
            return channel;

        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const uint8* src_data = src.GetData();

        if (!src_data || w <= 0 || h <= 0)
            return channel;

        if (!channel.Create(w, h))
            return channel;

        uint8* channel_data = channel.GetData();
        const int total = w * h;

        // 使用 memcpy 高效复制
        memcpy(channel_data, src_data, total * sizeof(uint8));

        return channel;
    }

    /**
     * 常用场景的便捷封装函数
     */

    // 从 RGBA 提取 R 通道
    inline BitmapGrey8 ExtractR(const BitmapRGBA8& src) { return ExtractChannel(src, 0); }

    // 从 RGBA 提取 G 通道
    inline BitmapGrey8 ExtractG(const BitmapRGBA8& src) { return ExtractChannel(src, 1); }

    // 从 RGBA 提取 B 通道
    inline BitmapGrey8 ExtractB(const BitmapRGBA8& src) { return ExtractChannel(src, 2); }

    // 从 RGBA 提取 A 通道
    inline BitmapGrey8 ExtractA(const BitmapRGBA8& src) { return ExtractChannel(src, 3); }

    // 从 RGB 提取 R 通道
    inline BitmapGrey8 ExtractR(const BitmapRGB8& src) { return ExtractChannel(src, 0); }

    // 从 RGB 提取 G 通道
    inline BitmapGrey8 ExtractG(const BitmapRGB8& src) { return ExtractChannel(src, 1); }

    // 从 RGB 提取 B 通道
    inline BitmapGrey8 ExtractB(const BitmapRGB8& src) { return ExtractChannel(src, 2); }

    // 从 RG 提取 R 通道
    inline BitmapGrey8 ExtractR(const BitmapRG8& src) { return ExtractChannel(src, 0); }

    // 从 RG 提取 G 通道
    inline BitmapGrey8 ExtractG(const BitmapRG8& src) { return ExtractChannel(src, 1); }

} // namespace hgl::bitmap::channel
