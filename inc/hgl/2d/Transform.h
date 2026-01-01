#pragma once

#include<hgl/2d/Bitmap.h>
#include<algorithm>

/**
 * Image Transformation Module
 * 
 * Provides rotation and flip operations for bitmaps.
 * All operations support any pixel format (RGB8, RGBA8, Grey8, RGB32F, etc.)
 * 
 * Example usage:
 * ```cpp
 * BitmapRGB8 source;
 * source.Create(100, 100);
 * 
 * // Rotate 90 degrees clockwise
 * auto rotated = hgl::bitmap::transform::Rotate(source, RotateAngle::Rotate90CW);
 * 
 * // Flip horizontally
 * auto flipped = hgl::bitmap::transform::Flip(source, FlipDirection::Horizontal);
 * 
 * // Flip in place (no new allocation)
 * hgl::bitmap::transform::FlipInPlace(source, FlipDirection::Vertical);
 * ```
 */

namespace hgl::bitmap::transform
{
    /**
     * Rotation angles
     */
    enum class RotateAngle 
    { 
        Rotate90CW,   // 90 degrees clockwise
        Rotate180,    // 180 degrees
        Rotate90CCW   // 90 degrees counter-clockwise (same as 270 CW)
    };

    /**
     * Flip directions
     */
    enum class FlipDirection 
    { 
        Horizontal,  // Left-right mirror
        Vertical,    // Top-bottom mirror
        Both         // Both horizontal and vertical
    };

    /**
     * Rotate a bitmap by 90/180/270 degrees
     * 
     * @param source Source bitmap
     * @param angle Rotation angle
     * @return New rotated bitmap
     * 
     * Note: 90/270 degree rotations swap width and height
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
                // New dimensions: swap width and height
                result.Create(src_height, src_width);
                T* dst_data = result.GetData();
                
                // For 90 CW: dst(y, x) = src(x, height - 1 - y)
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
                // Dimensions stay the same
                result.Create(src_width, src_height);
                T* dst_data = result.GetData();
                
                // For 180: dst(x, y) = src(width - 1 - x, height - 1 - y)
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
                // New dimensions: swap width and height
                result.Create(src_height, src_width);
                T* dst_data = result.GetData();
                
                // For 90 CCW: dst(y, x) = src(width - 1 - x, y)
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
     * Flip a bitmap (creates new bitmap)
     * 
     * @param source Source bitmap
     * @param direction Flip direction
     * @return New flipped bitmap
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
                // Mirror left-right
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
                // Mirror top-bottom
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
                // Mirror both directions (equivalent to 180 degree rotation)
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
     * Flip a bitmap in place (modifies original, no new allocation)
     * 
     * @param bitmap Bitmap to flip
     * @param direction Flip direction
     * 
     * Note: Only supports Horizontal and Vertical flips, not Both
     *       For vertical flip, uses the existing Bitmap::Flip() method
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
                // Mirror left-right in place
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
                // Use existing Bitmap::Flip() method for vertical flip
                bitmap.Flip();
                break;
            }
            
            case FlipDirection::Both:
            {
                // First flip vertically
                bitmap.Flip();
                // Then flip horizontally
                FlipInPlace(bitmap, FlipDirection::Horizontal);
                break;
            }
        }
    }

} // namespace hgl::bitmap::transform
