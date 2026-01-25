#pragma once

#include<hgl/2d/Bitmap.h>
#include<algorithm>

/**
 * 图像变换模块
 *
 * 提供位图的旋转和翻转操作。
 * 所有操作支持任意像素格式（如RGB8、RGBA8、Grey8、RGB32F等）。
 *
 * 示例用法：
 * ```cpp
 * BitmapRGB8 source;
 * source.Create(100, 100);
 *
 * // 顺时针旋转90度
 * auto rotated = hgl::bitmap::transform::Rotate(source, RotateAngle::Rotate90CW);
 *
 * // 水平翻转
 * auto flipped = hgl::bitmap::transform::Flip(source, FlipDirection::Horizontal);
 *
 * // 原地翻转（不分配新内存）
 * hgl::bitmap::transform::FlipInPlace(source, FlipDirection::Vertical);
 * ```
 */

namespace hgl::bitmap::transform
{
    /**
     * 旋转角度
     */
    enum class RotateAngle
    {
        Rotate90CW,   // 顺时针旋转90度
        Rotate180,    // 旋转180度
        Rotate90CCW   // 逆时针旋转90度（等同于顺时针270度）
    };

    /**
     * 翻转方向
     */
    enum class FlipDirection
    {
        Horizontal,  // 左右镜像
        Vertical,    // 上下镜像
        Both         // 同时左右和上下镜像
    };

    /**
     * 将位图旋转90/180/270度
     *
     * @param source 源位图
     * @param angle 旋转角度
     * @return 新的旋转后位图
     *
     * 注意：90/270度旋转会交换宽度和高度
     */
    template<typename T, uint C>
    Bitmap<T, C> Rotate(const Bitmap<T, C>& source, RotateAngle angle)
    {
        const int src_width = source.GetWidth();
        const int src_height = source.GetHeight();

        if (src_width == 0 || src_height == 0)
            return Bitmap<T, C>();

        const T* src_data = source.GetData();
        if (!src_data)
            return Bitmap<T, C>();

        Bitmap<T, C> result;

        switch (angle)
        {
            case RotateAngle::Rotate90CW:
            {
                // 新尺寸：宽高互换
                result.Create(src_height, src_width);
                T* dst_data = result.GetData();

                // 顺时针90度：dst(y, x) = src(x, height - 1 - y)
                for (int src_y = 0; src_y < src_height; ++src_y)
                {
                    for (int src_x = 0; src_x < src_width; ++src_x)
                    {
                        int dst_x = src_y;
                        int dst_y = src_width - 1 - src_x;
                        dst_data[dst_y * src_height + dst_x] = src_data[src_y * src_width + src_x];
                    }
                }
                break;
            }

            case RotateAngle::Rotate180:
            {
                // 尺寸保持不变
                result.Create(src_width, src_height);
                T* dst_data = result.GetData();

                // 180度：dst(x, y) = src(width - 1 - x, height - 1 - y)
                for (int src_y = 0; src_y < src_height; ++src_y)
                {
                    for (int src_x = 0; src_x < src_width; ++src_x)
                    {
                        int dst_x = src_width - 1 - src_x;
                        int dst_y = src_height - 1 - src_y;
                        dst_data[dst_y * src_width + dst_x] = src_data[src_y * src_width + src_x];
                    }
                }
                break;
            }

            case RotateAngle::Rotate90CCW:
            {
                // 新尺寸：宽高互换
                result.Create(src_height, src_width);
                T* dst_data = result.GetData();

                // 逆时针90度：dst(y, x) = src(width - 1 - x, y)
                for (int src_y = 0; src_y < src_height; ++src_y)
                {
                    for (int src_x = 0; src_x < src_width; ++src_x)
                    {
                        int dst_x = src_height - 1 - src_y;
                        int dst_y = src_x;
                        dst_data[dst_y * src_height + dst_x] = src_data[src_y * src_width + src_x];
                    }
                }
                break;
            }
        }

        return result;
    }

    /**
     * 翻转位图（生成新位图）
     *
     * @param source 源位图
     * @param direction 翻转方向
     * @return 新的翻转后位图
     */
    template<typename T, uint C>
    Bitmap<T, C> Flip(const Bitmap<T, C>& source, FlipDirection direction)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();

        if (width == 0 || height == 0)
            return Bitmap<T, C>();

        const T* src_data = source.GetData();
        if (!src_data)
            return Bitmap<T, C>();

        Bitmap<T, C> result;
        result.Create(width, height);
        T* dst_data = result.GetData();

        switch (direction)
        {
            case FlipDirection::Horizontal:
            {
                // 左右镜像
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        int src_x = width - 1 - x;
                        dst_data[y * width + x] = src_data[y * width + src_x];
                    }
                }
                break;
            }

            case FlipDirection::Vertical:
            {
                // 上下镜像
                for (int y = 0; y < height; ++y)
                {
                    int src_y = height - 1 - y;
                    for (int x = 0; x < width; ++x)
                    {
                        dst_data[y * width + x] = src_data[src_y * width + x];
                    }
                }
                break;
            }

            case FlipDirection::Both:
            {
                // 同时左右和上下镜像（等同于旋转180度）
                for (int y = 0; y < height; ++y)
                {
                    int src_y = height - 1 - y;
                    for (int x = 0; x < width; ++x)
                    {
                        int src_x = width - 1 - x;
                        dst_data[y * width + x] = src_data[src_y * width + src_x];
                    }
                }
                break;
            }
        }

        return result;
    }

    /**
     * 原地翻转位图（修改原始数据，不分配新内存）
     *
     * @param bitmap 要翻转的位图
     * @param direction 翻转方向
     *
     * 注意：垂直翻转时，使用已有的Bitmap::Flip()方法
     *       Both方向时，先垂直翻转再水平翻转
     */
    template<typename T, uint C>
    void FlipInPlace(Bitmap<T, C>& bitmap, FlipDirection direction)
    {
        const int width = bitmap.GetWidth();
        const int height = bitmap.GetHeight();

        if (width == 0 || height == 0)
            return;

        T* data = bitmap.GetData();
        if (!data)
            return;

        switch (direction)
        {
            case FlipDirection::Horizontal:
            {
                // 原地左右镜像
                for (int y = 0; y < height; ++y)
                {
                    T* row = data + y * width;
                    for (int x = 0; x < width / 2; ++x)
                    {
                        int mirror_x = width - 1 - x;
                        std::swap(row[x], row[mirror_x]);
                    }
                }
                break;
            }

            case FlipDirection::Vertical:
            {
                // 使用已有的Bitmap::Flip()方法进行垂直翻转
                bitmap.Flip();
                break;
            }

            case FlipDirection::Both:
            {
                // 先垂直翻转
                bitmap.Flip();
                // 再水平翻转
                FlipInPlace(bitmap, FlipDirection::Horizontal);
                break;
            }
        }
    }

} // namespace hgl::bitmap::transform
