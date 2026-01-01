#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<algorithm>
#include<cmath>

/**
 * Image Resize Module
 * 
 * Provides various interpolation methods for image scaling.
 * Supports nearest neighbor, bilinear, and bicubic interpolation.
 * 
 * Example usage:
 * ```cpp
 * BitmapRGB8 source;
 * source.Create(100, 100);
 * 
 * // Resize to specific dimensions with bilinear filtering
 * auto resized = hgl::bitmap::resize::Resize(source, 200, 150);
 * 
 * // Scale by factor with nearest neighbor (for pixel art)
 * auto scaled = hgl::bitmap::resize::ResizeScale(source, 2.0f, FilterType::NearestNeighbor);
 * ```
 */

namespace hgl::bitmap::resize
{
    /**
     * Interpolation filter types
     */
    enum class FilterType 
    { 
        NearestNeighbor,  // Fast, preserves hard edges (good for pixel art)
        Bilinear,         // Balanced quality and performance
        Bicubic           // Highest quality, slower
    };

    // ===================== Helper Functions =====================
    
    /**
     * Clamp value to byte range [0, 255]
     */
    inline uint8 ClampByte(float value)
    {
        return static_cast<uint8>(std::clamp(value, 0.0f, 255.0f));
    }

    /**
     * Linear interpolation for scalar types
     */
    template<typename T>
    inline T Lerp(const T& a, const T& b, float t)
    {
        return a + (b - a) * t;
    }

    /**
     * Linear interpolation for Vector2u8
     */
    inline math::Vector2u8 Lerp(const math::Vector2u8& a, const math::Vector2u8& b, float t)
    {
        return math::Vector2u8(
            ClampByte(a.x + (b.x - a.x) * t),
            ClampByte(a.y + (b.y - a.y) * t)
        );
    }

    /**
     * Linear interpolation for Vector3u8 (RGB)
     */
    inline math::Vector3u8 Lerp(const math::Vector3u8& a, const math::Vector3u8& b, float t)
    {
        return math::Vector3u8(
            ClampByte(a.r + (b.r - a.r) * t),
            ClampByte(a.g + (b.g - a.g) * t),
            ClampByte(a.b + (b.b - a.b) * t)
        );
    }

    /**
     * Linear interpolation for Vector4u8 (RGBA)
     */
    inline math::Vector4u8 Lerp(const math::Vector4u8& a, const math::Vector4u8& b, float t)
    {
        return math::Vector4u8(
            ClampByte(a.r + (b.r - a.r) * t),
            ClampByte(a.g + (b.g - a.g) * t),
            ClampByte(a.b + (b.b - a.b) * t),
            ClampByte(a.a + (b.a - a.a) * t)
        );
    }

    /**
     * Linear interpolation for Vector2f
     */
    inline math::Vector2f Lerp(const math::Vector2f& a, const math::Vector2f& b, float t)
    {
        return math::Vector2f(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t
        );
    }

    /**
     * Linear interpolation for Vector3f (RGB float)
     */
    inline math::Vector3f Lerp(const math::Vector3f& a, const math::Vector3f& b, float t)
    {
        return math::Vector3f(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }

    /**
     * Linear interpolation for Vector4f (RGBA float)
     */
    inline math::Vector4f Lerp(const math::Vector4f& a, const math::Vector4f& b, float t)
    {
        return math::Vector4f(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        );
    }

    // ===================== Sampling Functions =====================

    /**
     * Sample pixel with nearest neighbor
     */
    template<typename T, uint C>
    T SampleNearest(const Bitmap<T, C>& source, float x, float y)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        
        int ix = static_cast<int>(x + 0.5f);
        int iy = static_cast<int>(y + 0.5f);
        
        ix = std::clamp(ix, 0, width - 1);
        iy = std::clamp(iy, 0, height - 1);
        
        return source.GetData()[iy * width + ix];
    }

    /**
     * Sample pixel with bilinear interpolation
     */
    template<typename T, uint C>
    T SampleBilinear(const Bitmap<T, C>& source, float x, float y)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();
        
        // Get integer and fractional parts
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        float fx = x - x0;
        float fy = y - y0;
        
        // Clamp coordinates
        int x1 = x0 + 1;
        int y1 = y0 + 1;
        
        x0 = std::clamp(x0, 0, width - 1);
        x1 = std::clamp(x1, 0, width - 1);
        y0 = std::clamp(y0, 0, height - 1);
        y1 = std::clamp(y1, 0, height - 1);
        
        // Get four corner pixels
        T p00 = data[y0 * width + x0];
        T p10 = data[y0 * width + x1];
        T p01 = data[y1 * width + x0];
        T p11 = data[y1 * width + x1];
        
        // Bilinear interpolation
        T top = Lerp(p00, p10, fx);
        T bottom = Lerp(p01, p11, fx);
        return Lerp(top, bottom, fy);
    }

    /**
     * Cubic interpolation weight function
     */
    inline float CubicWeight(float x)
    {
        x = std::abs(x);
        if (x <= 1.0f)
            return 1.5f * x * x * x - 2.5f * x * x + 1.0f;
        else if (x < 2.0f)
            return -0.5f * x * x * x + 2.5f * x * x - 4.0f * x + 2.0f;
        else
            return 0.0f;
    }

    /**
     * Sample pixel with bicubic interpolation
     */
    template<typename T, uint C>
    T SampleBicubic(const Bitmap<T, C>& source, float x, float y)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();
        
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        
        // For uint8 types, we need to accumulate as floats
        float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
        float weight_sum = 0.0f;
        
        // Sample 4x4 neighborhood
        for (int dy = -1; dy <= 2; ++dy)
        {
            for (int dx = -1; dx <= 2; ++dx)
            {
                int sx = std::clamp(x0 + dx, 0, width - 1);
                int sy = std::clamp(y0 + dy, 0, height - 1);
                
                float wx = CubicWeight(x - (x0 + dx));
                float wy = CubicWeight(y - (y0 + dy));
                float w = wx * wy;
                
                const T& pixel = data[sy * width + sx];
                
                // Handle different pixel types
                if constexpr (C == 1)
                {
                    // Grayscale or single channel
                    if constexpr (std::is_same_v<T, uint8>)
                        r_sum += pixel * w;
                    else
                        r_sum += pixel * w;
                }
                else if constexpr (C == 2)
                {
                    r_sum += pixel.x * w;
                    g_sum += pixel.y * w;
                }
                else if constexpr (C == 3)
                {
                    r_sum += pixel.r * w;
                    g_sum += pixel.g * w;
                    b_sum += pixel.b * w;
                }
                else if constexpr (C == 4)
                {
                    r_sum += pixel.r * w;
                    g_sum += pixel.g * w;
                    b_sum += pixel.b * w;
                    a_sum += pixel.a * w;
                }
                
                weight_sum += w;
            }
        }
        
        // Normalize and construct result
        if (weight_sum > 0.0f)
        {
            r_sum /= weight_sum;
            g_sum /= weight_sum;
            b_sum /= weight_sum;
            a_sum /= weight_sum;
        }
        
        // Construct result based on type
        if constexpr (C == 1)
        {
            if constexpr (std::is_same_v<T, uint8>)
                return ClampByte(r_sum);
            else if constexpr (std::is_same_v<T, float>)
                return r_sum;
            else
                return static_cast<T>(r_sum);
        }
        else if constexpr (C == 2)
        {
            if constexpr (std::is_same_v<T, math::Vector2u8>)
                return math::Vector2u8(ClampByte(r_sum), ClampByte(g_sum));
            else
                return T(r_sum, g_sum);
        }
        else if constexpr (C == 3)
        {
            if constexpr (std::is_same_v<T, math::Vector3u8>)
                return math::Vector3u8(ClampByte(r_sum), ClampByte(g_sum), ClampByte(b_sum));
            else
                return T(r_sum, g_sum, b_sum);
        }
        else if constexpr (C == 4)
        {
            if constexpr (std::is_same_v<T, math::Vector4u8>)
                return math::Vector4u8(ClampByte(r_sum), ClampByte(g_sum), ClampByte(b_sum), ClampByte(a_sum));
            else
                return T(r_sum, g_sum, b_sum, a_sum);
        }
        
        return T{};
    }

    // ===================== Main Resize Functions =====================

    /**
     * Resize bitmap to specified dimensions
     * 
     * @param source Source bitmap
     * @param new_width Target width
     * @param new_height Target height
     * @param filter Interpolation filter to use
     * @return Resized bitmap
     */
    template<typename T, uint C>
    Bitmap<T, C> Resize(const Bitmap<T, C>& source, 
                        int new_width, int new_height,
                        FilterType filter = FilterType::Bilinear)
    {
        const int src_width = source.GetWidth();
        const int src_height = source.GetHeight();
        
        if (src_width == 0 || src_height == 0 || new_width <= 0 || new_height <= 0)
            return Bitmap<T, C>();
        
        if (!source.GetData())
            return Bitmap<T, C>();
        
        // If size is the same, just copy
        if (src_width == new_width && src_height == new_height)
        {
            Bitmap<T, C> result;
            result.Create(new_width, new_height);
            // Using memcpy for POD types (consistent with Bitmap::Flip implementation)
            memcpy(result.GetData(), source.GetData(), source.GetTotalBytes());
            return result;
        }
        
        Bitmap<T, C> result;
        result.Create(new_width, new_height);
        T* dst_data = result.GetData();
        
        // Calculate scale factors
        float x_scale = static_cast<float>(src_width) / new_width;
        float y_scale = static_cast<float>(src_height) / new_height;
        
        // Resample each pixel
        for (int dst_y = 0; dst_y < new_height; ++dst_y)
        {
            for (int dst_x = 0; dst_x < new_width; ++dst_x)
            {
                // Calculate source coordinates (center of pixel)
                float src_x = (dst_x + 0.5f) * x_scale - 0.5f;
                float src_y = (dst_y + 0.5f) * y_scale - 0.5f;
                
                // Sample based on filter type
                T pixel;
                switch (filter)
                {
                    case FilterType::NearestNeighbor:
                        pixel = SampleNearest(source, src_x, src_y);
                        break;
                    case FilterType::Bilinear:
                        pixel = SampleBilinear(source, src_x, src_y);
                        break;
                    case FilterType::Bicubic:
                        pixel = SampleBicubic(source, src_x, src_y);
                        break;
                }
                
                dst_data[dst_y * new_width + dst_x] = pixel;
            }
        }
        
        return result;
    }

    /**
     * Resize bitmap by scale factor
     * 
     * @param source Source bitmap
     * @param scale Scale factor (2.0 = double size, 0.5 = half size)
     * @param filter Interpolation filter to use
     * @return Resized bitmap
     */
    template<typename T, uint C>
    Bitmap<T, C> ResizeScale(const Bitmap<T, C>& source, 
                             float scale,
                             FilterType filter = FilterType::Bilinear)
    {
        if (scale <= 0.0f)
            return Bitmap<T, C>();
        
        int new_width = static_cast<int>(source.GetWidth() * scale);
        int new_height = static_cast<int>(source.GetHeight() * scale);
        
        return Resize(source, new_width, new_height, filter);
    }

} // namespace hgl::bitmap::resize
