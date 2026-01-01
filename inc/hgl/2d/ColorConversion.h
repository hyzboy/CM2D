#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<hgl/math/Color.h>
#include<hgl/math/Clamp.h>
#include<cmath>
#include<algorithm>

/**
 * Color Conversion Module
 * 
 * Provides various color space and format conversions for bitmaps.
 * Supports RGB/RGBA/Grayscale conversions and sRGB/Linear color space transformations.
 * 
 * Example usage:
 * ```cpp
 * // RGB to RGBA with opaque alpha
 * BitmapRGB8 rgb_image;
 * auto rgba_image = hgl::bitmap::color::ConvertRGBToRGBA(rgb_image, 255);
 * 
 * // RGB to Grayscale using ITU-R BT.601 standard
 * auto grey_image = hgl::bitmap::color::ConvertRGBToGrey(rgb_image);
 * 
 * // Extract red channel
 * auto red_channel = hgl::bitmap::color::ExtractChannel(rgb_image, 0);
 * 
 * // sRGB to Linear color space
 * auto linear = hgl::bitmap::color::ConvertSRGB8ToLinearF(rgb_image);
 * ```
 */

namespace hgl::bitmap::color
{
    // ===================== RGB ↔ RGBA Conversions =====================

    /**
     * Convert RGB to RGBA
     * 
     * @param source Source RGB bitmap
     * @param alpha Alpha value to use for all pixels (default: 255 = opaque)
     * @return RGBA bitmap
     */
    inline BitmapRGBA8 ConvertRGBToRGBA(const BitmapRGB8& source, uint8 alpha = 255)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGBA8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector3u8* src = source.GetData();
        math::Vector4u8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i].r;
            dst[i].g = src[i].g;
            dst[i].b = src[i].b;
            dst[i].a = alpha;
        }
        
        return result;
    }

    /**
     * Convert RGBA to RGB (discards alpha channel)
     * 
     * @param source Source RGBA bitmap
     * @return RGB bitmap
     */
    inline BitmapRGB8 ConvertRGBAToRGB(const BitmapRGBA8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGB8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector4u8* src = source.GetData();
        math::Vector3u8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i].r;
            dst[i].g = src[i].g;
            dst[i].b = src[i].b;
        }
        
        return result;
    }

    // ===================== RGB ↔ Grayscale Conversions =====================

    /**
     * Convert RGB to Grayscale using ITU-R BT.601 standard
     * 
     * Uses luminance weights: Y = 0.299R + 0.587G + 0.114B
     * 
     * @param source Source RGB bitmap
     * @return Grayscale bitmap
     */
    inline BitmapGrey8 ConvertRGBToGrey(const BitmapRGB8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapGrey8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector3u8* src = source.GetData();
        uint8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            // ITU-R BT.601 luma coefficients
            float grey = 0.299f * src[i].r + 0.587f * src[i].g + 0.114f * src[i].b;
            dst[i] = hgl::math::ClampU8(grey);
        }
        
        return result;
    }

    /**
     * Convert RGBA to Grayscale using ITU-R BT.601 standard (discards alpha)
     * 
     * @param source Source RGBA bitmap
     * @return Grayscale bitmap
     */
    inline BitmapGrey8 ConvertRGBAToGrey(const BitmapRGBA8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapGrey8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector4u8* src = source.GetData();
        uint8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            float grey = 0.299f * src[i].r + 0.587f * src[i].g + 0.114f * src[i].b;
            dst[i] = hgl::math::ClampU8(grey);
        }
        
        return result;
    }

    /**
     * Convert Grayscale to RGB (replicate grey value to all channels)
     * 
     * @param source Source grayscale bitmap
     * @return RGB bitmap
     */
    inline BitmapRGB8 ConvertGreyToRGB(const BitmapGrey8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGB8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const uint8* src = source.GetData();
        math::Vector3u8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i];
            dst[i].g = src[i];
            dst[i].b = src[i];
        }
        
        return result;
    }

    /**
     * Convert Grayscale to RGBA (replicate grey value to RGB, set alpha)
     * 
     * @param source Source grayscale bitmap
     * @param alpha Alpha value to use (default: 255 = opaque)
     * @return RGBA bitmap
     */
    inline BitmapRGBA8 ConvertGreyToRGBA(const BitmapGrey8& source, uint8 alpha = 255)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGBA8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const uint8* src = source.GetData();
        math::Vector4u8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i];
            dst[i].g = src[i];
            dst[i].b = src[i];
            dst[i].a = alpha;
        }
        
        return result;
    }

    // ===================== Channel Extraction =====================

    /**
     * Extract a single channel from a multi-channel bitmap
     * 
     * @param source Source bitmap
     * @param channel_index Channel index to extract (0=R/X, 1=G/Y, 2=B/Z, 3=A/W)
     * @return Single-channel bitmap
     * 
     * Note: For RGB, channels are 0=R, 1=G, 2=B
     *       For RGBA, channels are 0=R, 1=G, 2=B, 3=A
     */
    template<typename T, uint C>
    Bitmap<uint8, 1> ExtractChannel(const Bitmap<T, C>& source, uint channel_index)
    {
        static_assert(C > 1, "Source must be multi-channel bitmap");
        
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        Bitmap<uint8, 1> result;
        if (width == 0 || height == 0 || !source.GetData() || channel_index >= C)
            return result;
        
        result.Create(width, height);
        
        const T* src = source.GetData();
        uint8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            // Access channel based on type
            if constexpr (C == 2)
            {
                if (channel_index == 0)
                    dst[i] = static_cast<uint8>(src[i].x);
                else
                    dst[i] = static_cast<uint8>(src[i].y);
            }
            else if constexpr (C == 3)
            {
                if (channel_index == 0)
                    dst[i] = static_cast<uint8>(src[i].r);
                else if (channel_index == 1)
                    dst[i] = static_cast<uint8>(src[i].g);
                else
                    dst[i] = static_cast<uint8>(src[i].b);
            }
            else if constexpr (C == 4)
            {
                if (channel_index == 0)
                    dst[i] = static_cast<uint8>(src[i].r);
                else if (channel_index == 1)
                    dst[i] = static_cast<uint8>(src[i].g);
                else if (channel_index == 2)
                    dst[i] = static_cast<uint8>(src[i].b);
                else
                    dst[i] = static_cast<uint8>(src[i].a);
            }
        }
        
        return result;
    }

    // ===================== sRGB ↔ Linear Color Space =====================

    /**
     * Convert sRGB8 to Linear float color space
     * 
     * @param source Source sRGB bitmap (0-255)
     * @return Linear float bitmap (0.0-1.0)
     */
    inline BitmapRGB32F ConvertSRGB8ToLinearF(const BitmapRGB8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGB32F result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector3u8* src = source.GetData();
        math::Vector3f* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].x = hgl::math::SRGBByteToLinear(src[i].r);
            dst[i].y = hgl::math::SRGBByteToLinear(src[i].g);
            dst[i].z = hgl::math::SRGBByteToLinear(src[i].b);
        }
        
        return result;
    }

    /**
     * Convert sRGBA8 to Linear float color space
     * 
     * @param source Source sRGBA bitmap (0-255)
     * @return Linear float bitmap (0.0-1.0)
     * 
     * Note: Alpha is converted linearly (not gamma corrected)
     */
    inline BitmapRGBA32F ConvertSRGBA8ToLinearF(const BitmapRGBA8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGBA32F result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector4u8* src = source.GetData();
        math::Vector4f* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].x = hgl::math::SRGBByteToLinear(src[i].r);
            dst[i].y = hgl::math::SRGBByteToLinear(src[i].g);
            dst[i].z = hgl::math::SRGBByteToLinear(src[i].b);
            dst[i].w = src[i].a / 255.0f; // Alpha is linear
        }
        
        return result;
    }

    /**
     * Convert Linear float to sRGB8 color space
     * 
     * @param source Source linear float bitmap (0.0-1.0)
     * @return sRGB bitmap (0-255)
     */
    inline BitmapRGB8 ConvertLinearFToSRGB8(const BitmapRGB32F& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGB8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector3f* src = source.GetData();
        math::Vector3u8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = hgl::math::LinearToSRGBByte(src[i].x);
            dst[i].g = hgl::math::LinearToSRGBByte(src[i].y);
            dst[i].b = hgl::math::LinearToSRGBByte(src[i].z);
        }
        
        return result;
    }

    /**
     * Convert Linear float to sRGBA8 color space
     * 
     * @param source Source linear float bitmap (0.0-1.0)
     * @return sRGBA bitmap (0-255)
     * 
     * Note: Alpha is converted linearly (not gamma corrected)
     */
    inline BitmapRGBA8 ConvertLinearFToSRGBA8(const BitmapRGBA32F& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        BitmapRGBA8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        
        result.Create(width, height);
        
        const math::Vector4f* src = source.GetData();
        math::Vector4u8* dst = result.GetData();
        
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = hgl::math::LinearToSRGBByte(src[i].x);
            dst[i].g = hgl::math::LinearToSRGBByte(src[i].y);
            dst[i].b = hgl::math::LinearToSRGBByte(src[i].z);
            dst[i].a = hgl::math::ClampU8(src[i].w * 255.0f); // Alpha is linear
        }
        
        return result;
    }

} // namespace hgl::bitmap::color
