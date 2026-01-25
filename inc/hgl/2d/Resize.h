#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<hgl/math/Clamp.h>
#include<hgl/math/VectorOperations.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<algorithm>
#include<cmath>
#include<numbers>

/**
 * 图像缩放模块
 *
 * 提供各种插值方法用于图像缩放。
 * 支持最近邻、双线性、三次方、Lanczos、Mitchell-Netravali和自适应滤波。
 *
 * 示例用法：
 * ```cpp
 * BitmapRGB8 source;
 * source.Create(100, 100);
 *
 * // 使用双线性滤波缩放（质量和速度平衡）
 * auto resized = hgl::bitmap::resize::Resize(source, 200, 150);
 *
 * // 高质量Lanczos降采样
 * auto downscaled = hgl::bitmap::resize::Resize(source, 50, 50, FilterType::Lanczos3);
 *
 * // Mitchell-Netravali滤波（锐度和平滑度平衡）
 * auto mitchell = hgl::bitmap::resize::Resize(source, 150, 150, FilterType::MitchellNetravali);
 *
 * // 自适应模式（自动选择最佳滤波器）
 * auto adaptive = hgl::bitmap::resize::Resize(source, 75, 75, FilterType::Adaptive);
 *
 * // 使用最近邻按比例缩放（适用于像素艺术）
 * auto scaled = hgl::bitmap::resize::ResizeScale(source, 2.0f, FilterType::NearestNeighbor);
 * ```
 */

namespace hgl::bitmap::resize
{
    /**
     * 插值滤波器类型
     */
    enum class FilterType
    {
        NearestNeighbor,  // 快速，保留硬边缘（适用于像素艺术）
        Bilinear,         // 质量和性能平衡
        Bicubic,          // 使用三次插值的高质量
        Lanczos2,         // Lanczos a=2（高质量，更锐利）
        Lanczos3,         // Lanczos a=3（最高质量，非常锐利）
        MitchellNetravali,// Mitchell-Netravali滤波（B=1/3, C=1/3）- 锐度和平滑度平衡
        Adaptive          // 根据缩放比例自动选择最佳滤波器
    };

    // ===================== 采样函数 =====================

    /**
     * 使用最近邻采样像素
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
     * 使用双线性插值采样像素
     */
    template<typename T, uint C>
    T SampleBilinear(const Bitmap<T, C>& source, float x, float y)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();

        // 获取整数和分数部分
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        float fx = x - x0;
        float fy = y - y0;

        // 限制坐标
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        x0 = std::clamp(x0, 0, width - 1);
        x1 = std::clamp(x1, 0, width - 1);
        y0 = std::clamp(y0, 0, height - 1);
        y1 = std::clamp(y1, 0, height - 1);

        // 获取四个角像素
        T p00 = data[y0 * width + x0];
        T p10 = data[y0 * width + x1];
        T p01 = data[y1 * width + x0];
        T p11 = data[y1 * width + x1];

        // 双线性插值
        T top = hgl::math::lerp(p00, p10, fx);
        T bottom = hgl::math::lerp(p01, p11, fx);
        return hgl::math::lerp(top, bottom, fy);
    }

    /**
     * 三次插值权重函数
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
     * Lanczos核函数
     *
     * @param x 距离采样点的距离
     * @param a Lanczos核大小（通常为2或3）
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
     * Lanczos核的Sinc函数
     */
    inline float Sinc(float x)
    {
        if (std::abs(x) < 1e-6f)
            return 1.0f;
        float pi_x = std::numbers::pi_v<float> * x;
        return std::sin(pi_x) / pi_x;
    }

    /**
     * Mitchell-Netravali三次滤波权重函数
     *
     * 使用B=1/3, C=1/3参数（Mitchell-Netravali推荐值）
     * 在锐度和光滑度之间提供良好平衡
     *
     * @param x 距离采样点的距离
     * @return 滤波权重
     */
    inline float MitchellNetravaliWeight(float x)
    {
        x = std::abs(x);

        // Mitchell-Netravali参数
        constexpr float B = 1.0f / 3.0f;
        constexpr float C = 1.0f / 3.0f;

        if (x < 1.0f)
        {
            // 对于|x| < 1
            float x2 = x * x;
            float x3 = x2 * x;
            return ((12.0f - 9.0f * B - 6.0f * C) * x3 +
                    (-18.0f + 12.0f * B + 6.0f * C) * x2 +
                    (6.0f - 2.0f * B)) / 6.0f;
        }
        else if (x < 2.0f)
        {
            // 对于1 <= |x| < 2
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
     * 使用三次插值采样像素
     */
    template<typename T, uint C>
    T SampleBicubic(const Bitmap<T, C>& source, float x, float y)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();

        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));

        // 对于uint8类型，我们需要以浮点数累积
        float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
        float weight_sum = 0.0f;

        // 采样4x4邻域
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

                // 处理不同像素类型
                if constexpr (C == 1)
                {
                    // 灰度或单通道
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

        // 归一化并构造结果
        if (weight_sum > 0.0f)
        {
            r_sum /= weight_sum;
            g_sum /= weight_sum;
            b_sum /= weight_sum;
            a_sum /= weight_sum;
        }

        // 根据类型构造结果
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
            if constexpr (std::is_same_v<T, math::Vector3u8> || std::is_same_v<T, Color3ub>)
                return T(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum));
            else
                return T(r_sum, g_sum, b_sum);
        }
        else if constexpr (C == 4)
        {
            if constexpr (std::is_same_v<T, math::Vector4u8> || std::is_same_v<T, Color4ub>)
                return T(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum), hgl::math::ClampU8(a_sum));
            else
                return T(r_sum, g_sum, b_sum, a_sum);
        }

        return T{};
    }

    /**
     * 使用Lanczos插值采样像素
     *
     * @param source 源位图
     * @param x X坐标（可以是小数）
     * @param y Y坐标（可以是小数）
     * @param a Lanczos核大小（2或3）
     */
    template<typename T, uint C>
    T SampleLanczos(const Bitmap<T, C>& source, float x, float y, int a)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();

        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));

        // 对于uint8类型，我们需要以浮点数累积
        float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
        float weight_sum = 0.0f;

        // 根据核大小采样邻域（a）
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

                // 处理不同像素类型
                if constexpr (C == 1)
                {
                    // 灰度或单通道
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

        // 归一化并构造结果
        if (weight_sum > 0.0f)
        {
            r_sum /= weight_sum;
            g_sum /= weight_sum;
            b_sum /= weight_sum;
            a_sum /= weight_sum;
        }

        // 根据类型构造结果
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
            if constexpr (std::is_same_v<T, math::Vector3u8> || std::is_same_v<T, Color3ub>)
                return T(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum));
            else
                return T(r_sum, g_sum, b_sum);
        }
        else if constexpr (C == 4)
        {
            if constexpr (std::is_same_v<T, math::Vector4u8> || std::is_same_v<T, Color4ub>)
                return T(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum), hgl::math::ClampU8(a_sum));
            else
                return T(r_sum, g_sum, b_sum, a_sum);
        }

        return T{};
    }

    /**
     * 使用Mitchell-Netravali插值采样像素
     *
     * 使用Mitchell-Netravali三次滤波（B=1/3, C=1/3）进行高质量重采样。
     * 在锐度和光滑度之间提供良好平衡。
     *
     * @param source 源位图
     * @param x X坐标（可以是小数）
     * @param y Y坐标（可以是小数）
     */
    template<typename T, uint C>
    T SampleMitchellNetravali(const Bitmap<T, C>& source, float x, float y)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        const T* data = source.GetData();

        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));

        // 对于uint8类型，我们需要以浮点数累积
        float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f, a_sum = 0.0f;
        float weight_sum = 0.0f;

        // 采样4x4邻域（Mitchell-Netravali支持范围为2）
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

                // 处理不同像素类型
                if constexpr (C == 1)
                {
                    // 灰度或单通道
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

        // 归一化并构造结果
        if (weight_sum > 0.0f)
        {
            r_sum /= weight_sum;
            g_sum /= weight_sum;
            b_sum /= weight_sum;
            a_sum /= weight_sum;
        }

        // 根据类型构造结果
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
            if constexpr (std::is_same_v<T, math::Vector3u8> || std::is_same_v<T, Color3ub>)
                return T(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum));
            else
                return T(r_sum, g_sum, b_sum);
        }
        else if constexpr (C == 4)
        {
            if constexpr (std::is_same_v<T, math::Vector4u8> || std::is_same_v<T, Color4ub>)
                return T(hgl::math::ClampU8(r_sum), hgl::math::ClampU8(g_sum), hgl::math::ClampU8(b_sum), hgl::math::ClampU8(a_sum));
            else
                return T(r_sum, g_sum, b_sum, a_sum);
        }

        return T{};
    }

    /**
     * 根据给定的缩放比例确定最佳滤波器类型（自适应模式使用）
     *
     * @param scale_ratio 缩放比例（新尺寸/旧尺寸）
     * @return 推荐的滤波器类型
     */
    inline FilterType DetermineAdaptiveFilter(float scale_ratio)
    {
        // 对于显著降采样，使用Lanczos以获得最佳质量
        if (scale_ratio <= 0.5f)
            return FilterType::Lanczos3;

        // 对于中等降采样，使用Bicubic
        if (scale_ratio < 0.75f)
            return FilterType::Bicubic;

        // 对于轻微降采样或放大至2倍，使用Bilinear
        if (scale_ratio <= 2.0f)
            return FilterType::Bilinear;

        // 对于显著放大（>2倍），使用Bicubic以获得更平滑的结果
        if (scale_ratio <= 4.0f)
            return FilterType::Bicubic;

        // 对于极端放大，使用最近邻以保留清晰度
        return FilterType::NearestNeighbor;
    }

    // ===================== 主要缩放函数 =====================

    /**
     * 将位图缩放到指定尺寸
     *
     * @param source 源位图
     * @param new_width 目标宽度
     * @param new_height 目标高度
     * @param filter 要使用的插值滤波器
     * @return 缩放后的位图
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

        // 如果尺寸相同，只需复制
        if (src_width == new_width && src_height == new_height)
        {
            Bitmap<T, C> result;
            result.Create(new_width, new_height);
            // 使用memcpy处理POD类型（与Bitmap::Flip实现一致）
            memcpy(result.GetData(), source.GetData(), source.GetTotalBytes());
            return result;
        }

        Bitmap<T, C> result;
        result.Create(new_width, new_height);
        T* dst_data = result.GetData();

        // 计算缩放因子
        float x_scale = static_cast<float>(src_width) / new_width;
        float y_scale = static_cast<float>(src_height) / new_height;

        // 对于自适应模式，根据缩放比例确定最佳滤波器
        FilterType actual_filter = filter;
        if (filter == FilterType::Adaptive)
        {
            float avg_scale = (1.0f / x_scale + 1.0f / y_scale) / 2.0f;
            actual_filter = DetermineAdaptiveFilter(avg_scale);
        }

        // 对每个像素进行重采样
        for (int dst_y = 0; dst_y < new_height; ++dst_y)
        {
            for (int dst_x = 0; dst_x < new_width; ++dst_x)
            {
                // 计算源坐标（像素中心）
                float src_x = (dst_x + 0.5f) * x_scale - 0.5f;
                float src_y = (dst_y + 0.5f) * y_scale - 0.5f;

                // 根据滤波器类型采样
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
                        // 应该不会到达这里，因为我们在上面解析了自适应
                        pixel = SampleBilinear(source, src_x, src_y);
                        break;
                }

                dst_data[dst_y * new_width + dst_x] = pixel;
            }
        }

        return result;
    }

    /**
     * 按缩放因子缩放位图
     *
     * @param source 源位图
     * @param scale 缩放因子（2.0 = 双倍大小，0.5 = 半倍大小）
     * @param filter 要使用的插值滤波器
     * @return 缩放后的位图
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
