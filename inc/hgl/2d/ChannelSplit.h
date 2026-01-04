#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>

/**
 * Channel Split Module
 * 
 * Provides functionality to split multi-channel bitmaps into separate single-channel 
 * or multi-channel bitmaps. Supports various splitting patterns like:
 * - RGB -> R + G + B (3 single channels)
 * - RGBA -> RGB + A (3-channel + 1-channel)
 * - RGBA -> R + G + B + A (4 single channels)
 * - YUV -> Y + UV (1-channel + 2-channel)
 * 
 * Example usage:
 * ```cpp
 * BitmapRGBA8 source;
 * source.Create(100, 100);
 * 
 * // Split RGBA into 4 separate channels
 * auto [r, g, b, a] = hgl::bitmap::channel::SplitRGBA(source);
 * 
 * // Split RGBA into RGB + A
 * auto [rgb, alpha] = hgl::bitmap::channel::SplitRGBA_To_RGB_A(source);
 * 
 * // Split RGB into individual channels
 * auto [red, green, blue] = hgl::bitmap::channel::SplitRGB(source);
 * ```
 */

namespace hgl::bitmap::channel
{
    /**
     * Split RGBA bitmap into 4 separate single-channel bitmaps (R, G, B, A)
     * @param src Source RGBA bitmap
     * @return Tuple of 4 single-channel bitmaps (R, G, B, A)
     */
    template<typename T, uint C>
    auto SplitRGBA(const Bitmap<T, C>& src) -> std::tuple<BitmapGrey8*, BitmapGrey8*, BitmapGrey8*, BitmapGrey8*>
    {
        static_assert(C == 4, "Source bitmap must have 4 channels");
        
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const T* src_data = src.GetData();
        
        if (!src_data || w <= 0 || h <= 0)
            return {nullptr, nullptr, nullptr, nullptr};
        
        BitmapGrey8* r_channel = new BitmapGrey8();
        BitmapGrey8* g_channel = new BitmapGrey8();
        BitmapGrey8* b_channel = new BitmapGrey8();
        BitmapGrey8* a_channel = new BitmapGrey8();
        
        r_channel->Create(w, h);
        g_channel->Create(w, h);
        b_channel->Create(w, h);
        a_channel->Create(w, h);
        
        uint8* r_data = r_channel->GetData();
        uint8* g_data = g_channel->GetData();
        uint8* b_data = b_channel->GetData();
        uint8* a_data = a_channel->GetData();
        
        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            r_data[i] = src_data[i].r;
            g_data[i] = src_data[i].g;
            b_data[i] = src_data[i].b;
            a_data[i] = src_data[i].a;
        }
        
        return {r_channel, g_channel, b_channel, a_channel};
    }

    /**
     * Split RGB bitmap into 3 separate single-channel bitmaps (R, G, B)
     * @param src Source RGB bitmap
     * @return Tuple of 3 single-channel bitmaps (R, G, B)
     */
    template<typename T, uint C>
    auto SplitRGB(const Bitmap<T, C>& src) -> std::tuple<BitmapGrey8*, BitmapGrey8*, BitmapGrey8*>
    {
        static_assert(C == 3, "Source bitmap must have 3 channels");
        
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const T* src_data = src.GetData();
        
        if (!src_data || w <= 0 || h <= 0)
            return {nullptr, nullptr, nullptr};
        
        BitmapGrey8* r_channel = new BitmapGrey8();
        BitmapGrey8* g_channel = new BitmapGrey8();
        BitmapGrey8* b_channel = new BitmapGrey8();
        
        r_channel->Create(w, h);
        g_channel->Create(w, h);
        b_channel->Create(w, h);
        
        uint8* r_data = r_channel->GetData();
        uint8* g_data = g_channel->GetData();
        uint8* b_data = b_channel->GetData();
        
        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            r_data[i] = src_data[i].r;
            g_data[i] = src_data[i].g;
            b_data[i] = src_data[i].b;
        }
        
        return {r_channel, g_channel, b_channel};
    }

    /**
     * Split RG bitmap into 2 separate single-channel bitmaps (R, G)
     * @param src Source RG bitmap
     * @return Tuple of 2 single-channel bitmaps (R, G)
     */
    template<typename T, uint C>
    auto SplitRG(const Bitmap<T, C>& src) -> std::tuple<BitmapGrey8*, BitmapGrey8*>
    {
        static_assert(C == 2, "Source bitmap must have 2 channels");
        
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const T* src_data = src.GetData();
        
        if (!src_data || w <= 0 || h <= 0)
            return {nullptr, nullptr};
        
        BitmapGrey8* r_channel = new BitmapGrey8();
        BitmapGrey8* g_channel = new BitmapGrey8();
        
        r_channel->Create(w, h);
        g_channel->Create(w, h);
        
        uint8* r_data = r_channel->GetData();
        uint8* g_data = g_channel->GetData();
        
        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            r_data[i] = src_data[i].r;
            g_data[i] = src_data[i].g;
        }
        
        return {r_channel, g_channel};
    }

    /**
     * Split RGBA bitmap into RGB (3-channel) + A (1-channel)
     * @param src Source RGBA bitmap
     * @return Tuple of RGB bitmap and Alpha channel bitmap
     */
    template<typename T, uint C>
    auto SplitRGBA_To_RGB_A(const Bitmap<T, C>& src) -> std::tuple<BitmapRGB8*, BitmapGrey8*>
    {
        static_assert(C == 4, "Source bitmap must have 4 channels");
        
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const T* src_data = src.GetData();
        
        if (!src_data || w <= 0 || h <= 0)
            return {nullptr, nullptr};
        
        BitmapRGB8* rgb_bitmap = new BitmapRGB8();
        BitmapGrey8* a_channel = new BitmapGrey8();
        
        rgb_bitmap->Create(w, h);
        a_channel->Create(w, h);
        
        math::Vector3u8* rgb_data = rgb_bitmap->GetData();
        uint8* a_data = a_channel->GetData();
        
        const int total = w * h;
        for (int i = 0; i < total; ++i)
        {
            rgb_data[i].r = src_data[i].r;
            rgb_data[i].g = src_data[i].g;
            rgb_data[i].b = src_data[i].b;
            a_data[i] = src_data[i].a;
        }
        
        return {rgb_bitmap, a_channel};
    }

    /**
     * Extract single channel from multi-channel bitmap by index
     * @param src Source bitmap
     * @param channel_index Channel index (0=R, 1=G, 2=B, 3=A)
     * @return Single-channel bitmap, or nullptr if index is invalid
     */
    template<typename T, uint C>
    BitmapGrey8* ExtractChannel(const Bitmap<T, C>& src, uint channel_index)
    {
        if (channel_index >= C)
            return nullptr;
        
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        const T* src_data = src.GetData();
        
        if (!src_data || w <= 0 || h <= 0)
            return nullptr;
        
        BitmapGrey8* channel = new BitmapGrey8();
        channel->Create(w, h);
        
        uint8* channel_data = channel->GetData();
        const int total = w * h;
        
        if constexpr (C == 1)
        {
            // Single channel, just copy
            for (int i = 0; i < total; ++i)
                channel_data[i] = src_data[i];
        }
        else if constexpr (C == 2)
        {
            for (int i = 0; i < total; ++i)
                channel_data[i] = (channel_index == 0) ? src_data[i].r : src_data[i].g;
        }
        else if constexpr (C == 3)
        {
            for (int i = 0; i < total; ++i)
            {
                if (channel_index == 0)
                    channel_data[i] = src_data[i].r;
                else if (channel_index == 1)
                    channel_data[i] = src_data[i].g;
                else
                    channel_data[i] = src_data[i].b;
            }
        }
        else if constexpr (C == 4)
        {
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
        }
        
        return channel;
    }

    /**
     * Convenient wrapper functions for common use cases
     */
    
    // Extract R channel from RGB/RGBA
    template<typename T, uint C>
    inline BitmapGrey8* ExtractR(const Bitmap<T, C>& src) { return ExtractChannel(src, 0); }
    
    // Extract G channel from RG/RGB/RGBA
    template<typename T, uint C>
    inline BitmapGrey8* ExtractG(const Bitmap<T, C>& src) { return ExtractChannel(src, 1); }
    
    // Extract B channel from RGB/RGBA
    template<typename T, uint C>
    inline BitmapGrey8* ExtractB(const Bitmap<T, C>& src) { return ExtractChannel(src, 2); }
    
    // Extract A channel from RGBA
    template<typename T, uint C>
    inline BitmapGrey8* ExtractA(const Bitmap<T, C>& src) { return ExtractChannel(src, 3); }

} // namespace hgl::bitmap::channel
