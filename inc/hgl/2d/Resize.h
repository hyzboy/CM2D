#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<hgl/math/Clamp.h>
#include<hgl/math/VectorOperations.h>
#include<algorithm>
#include<cmath>
#include<numbers>

/**
 * Image Resize Module
 * 
 * Provides various interpolation methods for image scaling.
 * Supports nearest neighbor, bilinear, bicubic, Lanczos, Mitchell-Netravali, and adaptive filtering.
 * 
 * Example usage:
 * ```cpp
 * BitmapRGB8 source;
 * source.Create(100, 100);
 * 
 * // Resize with bilinear filtering (balanced quality/speed)
 * auto resized = hgl::bitmap::resize::Resize(source, 200, 150);
 * 
 * // High-quality Lanczos downscaling
 * auto downscaled = hgl::bitmap::resize::Resize(source, 50, 50, FilterType::Lanczos3);
 * 
 * // Mitchell-Netravali filter (balanced sharpness/smoothness)
 * auto mitchell = hgl::bitmap::resize::Resize(source, 150, 150, FilterType::MitchellNetravali);
 * 
 * // Adaptive mode (automatically selects best filter)
 * auto adaptive = hgl::bitmap::resize::Resize(source, 75, 75, FilterType::Adaptive);
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
        Bicubic,          // High quality with cubic interpolation
        Lanczos2,         // Lanczos with a=2 (high quality, sharper)
        Lanczos3,         // Lanczos with a=3 (highest quality, very sharp)
        MitchellNetravali,// Mitchell-Netravali filter (B=1/3, C=1/3) - balanced sharpness and smoothness
        Adaptive          // Automatically select best filter based on scale ratio
    };

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
        T top = hgl::math::lerp(p00, p10, fx);
        T bottom = hgl::math::lerp(p01, p11, fx);
        return hgl::math::lerp(top, bottom, fy);
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
     * Lanczos kernel function
     * 
     * @param x Distance from sample point
     * @param a Lanczos kernel size (typically 2 or 3)
     */
    inline float LanczosWeight(float x, int a)
    {
        x = std::abs(x);
        if (x < 1e-6f)
            return 1.0f;
        if (x >= a)
            return 0.0f;
        
        float pi_x = std::numbers::pi_v<float> * x;
        return (a * std::sin(pi_x) * std::sin(pi_x / a)) / (pi_x * pi_x);
    }

    /**
     * Sinc function for Lanczos kernel
     */
    inline float Sinc(float x)
    {
        if (std::abs(x) < 1e-6f)
            return 1.0f;
        float pi_x = std::numbers::pi_v<float> * x;
        return std::sin(pi_x) / pi_x;
    }

    /**
     * Mitchell-Netravali cubic filter weight function
     * 
     * Uses B=1/3, C=1/3 parameters (Mitchell-Netravali recommended values)
     * Provides a good balance between sharpness and smoothness
     * 
     * @param x Distance from sample point
     * @return Filter weight
     */
    inline float MitchellNetravaliWeight(float x)
    {
        x = std::abs(x);
        
        // Mitchell-Netravali parameters
        constexpr float B = 1.0f / 3.0f;
        constexpr float C = 1.0f / 3.0f;
        
        if (x < 1.0f)
        {
            // For |x| < 1
            float x2 = x * x;
            float x3 = x2 * x;
            return ((12.0f - 9.0f * B - 6.0f * C) * x3 + 
                    (-18.0f + 12.0f * B + 6.0f * C) * x2 + 
                    (6.0f - 2.0f * B)) / 6.0f;
        }
        else if (x < 2.0f)
        {
            // For 1 <= |x| < 2
            float x2 = x * x;
            float x3 = x2 * x;
            return ((-B - 6.0f * C) * x3 + 
                    (6.0f * B + 30.0f * C) * x2 + 
                    (-12.0f * B - 48.0f * C) * x + 
                    (8.0f * B + 24.0f * C)) / 6.0f;
        }
        
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
                return hgl::math::ClampU8(r_sum);
            else if constexpr (std::is_same_v<T, float>)
                return r_sum;
            else
                return static_cast<T>(r_sum);
        }
        else if constexpr (C == 2)
        {
            if constexpr (std::is_same_v<T, math::Vector2u8>)
                return math::Vector2u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum));
            else
                return T(r_sum, g_sum);
        }
        else if constexpr (C == 3)
        {
            if constexpr (std::is_same_v<T, math::Vector3u8>)
                return math::Vector3u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum));
            else
                return T(r_sum, g_sum, b_sum);
        }
        else if constexpr (C == 4)
        {
            if constexpr (std::is_same_v<T, math::Vector4u8>)
                return math::Vector4u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum), hgl::math::ClampU8(a_sum));
            else
                return T(r_sum, g_sum, b_sum, a_sum);
        }
        
        return T{};
    }

    /**
     * Sample pixel with Lanczos interpolation
     * 
     * @param source Source bitmap
     * @param x X coordinate (can be fractional)
     * @param y Y coordinate (can be fractional)
     * @param a Lanczos kernel size (2 or 3)
     */
    template<typename T, uint C>
    T SampleLanczos(const Bitmap<T, C>& source, float x, float y, int a)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();
        
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        
        // For uint8 types, we need to accumulate as floats
        float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
        float weight_sum = 0.0f;
        
        // Sample neighborhood based on kernel size (a)
        for (int dy = -a + 1; dy <= a; ++dy)
        {
            for (int dx = -a + 1; dx <= a; ++dx)
            {
                int sx = std::clamp(x0 + dx, 0, width - 1);
                int sy = std::clamp(y0 + dy, 0, height - 1);
                
                float wx = LanczosWeight(x - (x0 + dx), a);
                float wy = LanczosWeight(y - (y0 + dy), a);
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
                return hgl::math::ClampU8(r_sum);
            else if constexpr (std::is_same_v<T, float>)
                return r_sum;
            else
                return static_cast<T>(r_sum);
        }
        else if constexpr (C == 2)
        {
            if constexpr (std::is_same_v<T, math::Vector2u8>)
                return math::Vector2u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum));
            else
                return T(r_sum, g_sum);
        }
        else if constexpr (C == 3)
        {
            if constexpr (std::is_same_v<T, math::Vector3u8>)
                return math::Vector3u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum));
            else
                return T(r_sum, g_sum, b_sum);
        }
        else if constexpr (C == 4)
        {
            if constexpr (std::is_same_v<T, math::Vector4u8>)
                return math::Vector4u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum), hgl::math::ClampU8(a_sum));
            else
                return T(r_sum, g_sum, b_sum, a_sum);
        }
        
        return T{};
    }

    /**
     * Sample pixel with Mitchell-Netravali interpolation
     * 
     * Uses Mitchell-Netravali cubic filter (B=1/3, C=1/3) for high-quality resampling.
     * Provides a good balance between sharpness and smoothness.
     * 
     * @param source Source bitmap
     * @param x X coordinate (can be fractional)
     * @param y Y coordinate (can be fractional)
     */
    template<typename T, uint C>
    T SampleMitchellNetravali(const Bitmap<T, C>& source, float x, float y)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();
        
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        
        // For uint8 types, we need to accumulate as floats
        float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
        float weight_sum = 0.0f;
        
        // Sample 4x4 neighborhood (Mitchell-Netravali has support of 2)
        for (int dy = -1; dy <= 2; ++dy)
        {
            for (int dx = -1; dx <= 2; ++dx)
            {
                int sx = std::clamp(x0 + dx, 0, width - 1);
                int sy = std::clamp(y0 + dy, 0, height - 1);
                
                float wx = MitchellNetravaliWeight(x - (x0 + dx));
                float wy = MitchellNetravaliWeight(y - (y0 + dy));
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
                return hgl::math::ClampU8(r_sum);
            else if constexpr (std::is_same_v<T, float>)
                return r_sum;
            else
                return static_cast<T>(r_sum);
        }
        else if constexpr (C == 2)
        {
            if constexpr (std::is_same_v<T, math::Vector2u8>)
                return math::Vector2u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum));
            else
                return T(r_sum, g_sum);
        }
        else if constexpr (C == 3)
        {
            if constexpr (std::is_same_v<T, math::Vector3u8>)
                return math::Vector3u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum));
            else
                return T(r_sum, g_sum, b_sum);
        }
        else if constexpr (C == 4)
        {
            if constexpr (std::is_same_v<T, math::Vector4u8>)
                return math::Vector4u8(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum), hgl::math::ClampU8(a_sum));
            else
                return T(r_sum, g_sum, b_sum, a_sum);
        }
        
        return T{};
    }

    /**
     * Determine the best filter type for given scale ratio (used by Adaptive mode)
     * 
     * @param scale_ratio Scaling ratio (new_size / old_size)
     * @return Recommended filter type
     */
    inline FilterType DetermineAdaptiveFilter(float scale_ratio)
    {
        // For significant downscaling, use Lanczos for best quality
        if (scale_ratio <= 0.5f)
            return FilterType::Lanczos3;
        
        // For moderate downscaling, use Bicubic
        if (scale_ratio < 0.75f)
            return FilterType::Bicubic;
        
        // For slight downscaling or upscaling up to 2x, use Bilinear
        if (scale_ratio <= 2.0f)
            return FilterType::Bilinear;
        
        // For significant upscaling (>2x), use Bicubic for smoother results
        if (scale_ratio <= 4.0f)
            return FilterType::Bicubic;
        
        // For extreme upscaling, use Nearest Neighbor to preserve clarity
        return FilterType::NearestNeighbor;
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
        
        // For adaptive mode, determine the best filter based on scale ratio
        FilterType actual_filter = filter;
        if (filter == FilterType::Adaptive)
        {
            float avg_scale = (1.0f / x_scale + 1.0f / y_scale) / 2.0f;
            actual_filter = DetermineAdaptiveFilter(avg_scale);
        }
        
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
                switch (actual_filter)
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
                    case FilterType::Lanczos2:
                        pixel = SampleLanczos(source, src_x, src_y, 2);
                        break;
                    case FilterType::Lanczos3:
                        pixel = SampleLanczos(source, src_x, src_y, 3);
                        break;
                    case FilterType::MitchellNetravali:
                        pixel = SampleMitchellNetravali(source, src_x, src_y);
                        break;
                    case FilterType::Adaptive:
                        // Should not reach here as we resolve adaptive above
                        pixel = SampleBilinear(source, src_x, src_y);
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
