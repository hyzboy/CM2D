#pragma once

#include<hgl/2d/Bitmap.h>
#include<vector>
#include<algorithm>

/**
 * 瓦片处理模块
 *
 * 提供将图像分割为瓦片并重新组装的功能。
 * 还包括用于生成无缝平铺纹理的功能。
 *
 * 示例用法：
 * ```cpp
 * BitmapRGB8 source;
 * source.Create(256, 256);
 *
 * // 分割为64x64的瓦片
 * auto tileset = hgl::bitmap::tile::SplitIntoTiles(source, 64, 64);
 *
 * // 处理每个瓦片...
 * for (size_t i = 0; i < tileset.GetTileCount(); ++i) {
 *     auto& tile = tileset.GetTile(i);
 *     // 修改瓦片...
 * }
 *
 * // 将瓦片重新组装为图像
 * auto assembled = hgl::bitmap::tile::AssembleTiles(tileset, 256, 256);
 *
 * // 生成无缝平铺纹理
 * auto seamless = hgl::bitmap::tile::MakeSeamless(source, 16);
 * ```
 */

namespace hgl::bitmap::tile
{
    /**
     * 瓦片在原始图像中的位置信息
     */
    struct TileInfo
    {
        int x, y;            // Tile position in original image (top-left corner)
        int width, height;   // Tile dimensions (may be less than requested at edges)
        int index;           // Tile index in the tileset
    };

    /**
     * 瓦片集合的容器
     */
    template<typename T, uint C>
    class TileSet
    {
    private:
        std::vector<Bitmap<T, C>> tiles_;
        std::vector<TileInfo> infos_;

    public:
        TileSet() = default;

        /**
         * 获取集合中的瓦片数量
         */
        size_t GetTileCount() const
        {
            return tiles_.size();
        }

        /**
         * 获取指定的瓦片（可修改）
         */
        Bitmap<T, C>& GetTile(size_t index)
        {
            return tiles_[index];
        }

        /**
         * 获取指定的瓦片（只读）
         */
        const Bitmap<T, C>& GetTile(size_t index) const
        {
            return tiles_[index];
        }

        /**
         * 获取指定瓦片的信息
         */
        const TileInfo& GetTileInfo(size_t index) const
        {
            return infos_[index];
        }

        /**
         * 向集合中添加一个瓦片
         */
        void AddTile(Bitmap<T, C>&& tile, const TileInfo& info)
        {
            tiles_.push_back(std::move(tile));
            infos_.push_back(info);
        }

        /**
         * 检查瓦片集合是否为空
         */
        bool IsEmpty() const
        {
            return tiles_.empty();
        }

        /**
         * 清空所有瓦片
         */
        void Clear()
        {
            tiles_.clear();
            infos_.clear();
        }
    };

    /**
     * 将位图分割为固定大小的瓦片
     *
     * @param source 要分割的源位图
     * @param tile_width 每个瓦片的宽度
     * @param tile_height 每个瓦片的高度
     * @param allow_partial 是否允许边缘生成部分瓦片（true为允许，false只生成完整瓦片）
     * @return 包含所有瓦片的TileSet
     *
     * 瓦片按从左到右、从上到下的顺序生成。
     * 如果allow_partial为true，边缘瓦片可能小于tile_width x tile_height。
     */
    template<typename T, uint C>
    TileSet<T, C> SplitIntoTiles(const Bitmap<T, C>& source,
                                  int tile_width, int tile_height,
                                  bool allow_partial = true)
    {
        TileSet<T, C> result;

        const int src_width = source.GetWidth();
        const int src_height = source.GetHeight();

        if (src_width == 0 || src_height == 0 || !source.GetData())
            return result;

        if (tile_width <= 0 || tile_height <= 0)
            return result;

        const T* src_data = source.GetData();
        int tile_index = 0;

        // Iterate through tiles
        for (int tile_y = 0; tile_y < src_height; tile_y += tile_height)
        {
            for (int tile_x = 0; tile_x < src_width; tile_x += tile_width)
            {
                // Calculate actual tile dimensions (may be smaller at edges)
                int actual_width = std::min(tile_width, src_width - tile_x);
                int actual_height = std::min(tile_height, src_height - tile_y);

                // Skip partial tiles if not allowed
                if (!allow_partial && (actual_width < tile_width || actual_height < tile_height))
                    continue;

                // Create tile
                Bitmap<T, C> tile;
                tile.Create(actual_width, actual_height);
                T* tile_data = tile.GetData();

                // Copy pixels from source to tile
                for (int y = 0; y < actual_height; ++y)
                {
                    const T* src_row = src_data + (tile_y + y) * src_width + tile_x;
                    T* tile_row = tile_data + y * actual_width;
                    // Using memcpy for POD types (consistent with Bitmap::Flip implementation)
                    memcpy(tile_row, src_row, actual_width * sizeof(T));
                }

                // Create tile info
                TileInfo info;
                info.x = tile_x;
                info.y = tile_y;
                info.width = actual_width;
                info.height = actual_height;
                info.index = tile_index++;

                result.AddTile(std::move(tile), info);
            }
        }

        return result;
    }

    /**
     * 将瓦片重新组装为完整图像
     *
     * @param tileset 要组装的瓦片集合
     * @param target_width 目标图像的宽度
     * @param target_height 目标图像的高度
     * @return 组装后的位图
     *
     * 瓦片会放置在其原始位置（根据TileInfo）。
     * 没有被瓦片覆盖的区域将保持未初始化状态。
     */
    template<typename T, uint C>
    Bitmap<T, C> AssembleTiles(const TileSet<T, C>& tileset,
                               int target_width, int target_height)
    {
        Bitmap<T, C> result;

        if (target_width <= 0 || target_height <= 0 || tileset.IsEmpty())
            return result;

        result.Create(target_width, target_height);
        T* dst_data = result.GetData();

        // Place each tile
        for (size_t i = 0; i < tileset.GetTileCount(); ++i)
        {
            const auto& tile = tileset.GetTile(i);
            const auto& info = tileset.GetTileInfo(i);

            const T* tile_data = tile.GetData();
            const int tile_width = info.width;
            const int tile_height = info.height;

            // Copy tile pixels to result
            for (int y = 0; y < tile_height; ++y)
            {
                int dst_y = info.y + y;
                if (dst_y < 0 || dst_y >= target_height)
                    continue;

                for (int x = 0; x < tile_width; ++x)
                {
                    int dst_x = info.x + x;
                    if (dst_x < 0 || dst_x >= target_width)
                        continue;

                    dst_data[dst_y * target_width + dst_x] = tile_data[y * tile_width + x];
                }
            }
        }

        return result;
    }

    /**
     * 通过混合边缘使纹理无缝
     *
     * @param source 源位图
     * @param blend_width 边缘混合区域宽度（0表示自动计算为最小边长的10%）
     * @return 可无限平铺的无缝位图
     *
     * 此函数会混合相对的边缘，以消除平铺时可见的接缝。
     * 混合区域是指对边像素进行混合的区域。
     */
    template<typename T, uint C>
    Bitmap<T, C> MakeSeamless(const Bitmap<T, C>& source, int blend_width = 0)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();

        if (width == 0 || height == 0 || !source.GetData())
            return Bitmap<T, C>();

        // Auto-calculate blend width if not specified
        if (blend_width <= 0)
        {
            blend_width = std::max(1, std::min(width, height) / 10);
        }

        // Clamp blend width to reasonable values
        blend_width = std::min(blend_width, std::min(width, height) / 2);

        // Create result as a copy of source
        Bitmap<T, C> result;
        result.Create(width, height);
        // Using memcpy for POD types (consistent with Bitmap::Flip implementation)
        memcpy(result.GetData(), source.GetData(), source.GetTotalBytes());

        T* data = result.GetData();

        // Blend horizontal edges (left-right)
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < blend_width; ++x)
            {
                float t = static_cast<float>(x) / blend_width; // 0 at left, 1 at blend_width

                // Left edge: blend with right edge
                int left_x = x;
                int right_x = width - blend_width + x;

                const T& left_pixel = data[y * width + left_x];
                const T& right_pixel = data[y * width + right_x];

                // Blend and write to both sides
                if constexpr (C == 1)
                {
                    // Single channel (grayscale)
                    if constexpr (std::is_same_v<T, uint8>)
                    {
                        uint8 blended = static_cast<uint8>(left_pixel * (1.0f - t) + right_pixel * t);
                        data[y * width + left_x] = blended;
                        data[y * width + right_x] = blended;
                    }
                    else
                    {
                        T blended = left_pixel * (1.0f - t) + right_pixel * t;
                        data[y * width + left_x] = blended;
                        data[y * width + right_x] = blended;
                    }
                }
                else
                {
                    // Multi-channel (RGB, RGBA, etc.)
                    T blended;

                    if constexpr (C == 2)
                    {
                        blended.x = static_cast<decltype(blended.x)>(left_pixel.x * (1.0f - t) + right_pixel.x * t);
                        blended.y = static_cast<decltype(blended.y)>(left_pixel.y * (1.0f - t) + right_pixel.y * t);
                    }
                    else if constexpr (C == 3)
                    {
                        blended.r = static_cast<decltype(blended.r)>(left_pixel.r * (1.0f - t) + right_pixel.r * t);
                        blended.g = static_cast<decltype(blended.g)>(left_pixel.g * (1.0f - t) + right_pixel.g * t);
                        blended.b = static_cast<decltype(blended.b)>(left_pixel.b * (1.0f - t) + right_pixel.b * t);
                    }
                    else if constexpr (C == 4)
                    {
                        blended.r = static_cast<decltype(blended.r)>(left_pixel.r * (1.0f - t) + right_pixel.r * t);
                        blended.g = static_cast<decltype(blended.g)>(left_pixel.g * (1.0f - t) + right_pixel.g * t);
                        blended.b = static_cast<decltype(blended.b)>(left_pixel.b * (1.0f - t) + right_pixel.b * t);
                        blended.a = static_cast<decltype(blended.a)>(left_pixel.a * (1.0f - t) + right_pixel.a * t);
                    }

                    data[y * width + left_x] = blended;
                    data[y * width + right_x] = blended;
                }
            }
        }

        // Blend vertical edges (top-bottom)
        for (int y = 0; y < blend_width; ++y)
        {
            float t = static_cast<float>(y) / blend_width; // 0 at top, 1 at blend_width

            for (int x = 0; x < width; ++x)
            {
                // Top edge: blend with bottom edge
                int top_y = y;
                int bottom_y = height - blend_width + y;

                const T& top_pixel = data[top_y * width + x];
                const T& bottom_pixel = data[bottom_y * width + x];

                // Blend and write to both sides
                if constexpr (C == 1)
                {
                    // Single channel (grayscale)
                    if constexpr (std::is_same_v<T, uint8>)
                    {
                        uint8 blended = static_cast<uint8>(top_pixel * (1.0f - t) + bottom_pixel * t);
                        data[top_y * width + x] = blended;
                        data[bottom_y * width + x] = blended;
                    }
                    else
                    {
                        T blended = top_pixel * (1.0f - t) + bottom_pixel * t;
                        data[top_y * width + x] = blended;
                        data[bottom_y * width + x] = blended;
                    }
                }
                else
                {
                    // Multi-channel (RGB, RGBA, etc.)
                    T blended;

                    if constexpr (C == 2)
                    {
                        blended.x = static_cast<decltype(blended.x)>(top_pixel.x * (1.0f - t) + bottom_pixel.x * t);
                        blended.y = static_cast<decltype(blended.y)>(top_pixel.y * (1.0f - t) + bottom_pixel.y * t);
                    }
                    else if constexpr (C == 3)
                    {
                        blended.r = static_cast<decltype(blended.r)>(top_pixel.r * (1.0f - t) + bottom_pixel.r * t);
                        blended.g = static_cast<decltype(blended.g)>(top_pixel.g * (1.0f - t) + bottom_pixel.g * t);
                        blended.b = static_cast<decltype(blended.b)>(top_pixel.b * (1.0f - t) + bottom_pixel.b * t);
                    }
                    else if constexpr (C == 4)
                    {
                        blended.r = static_cast<decltype(blended.r)>(top_pixel.r * (1.0f - t) + bottom_pixel.r * t);
                        blended.g = static_cast<decltype(blended.g)>(top_pixel.g * (1.0f - t) + bottom_pixel.g * t);
                        blended.b = static_cast<decltype(blended.b)>(top_pixel.b * (1.0f - t) + bottom_pixel.b * t);
                        blended.a = static_cast<decltype(blended.a)>(top_pixel.a * (1.0f - t) + bottom_pixel.a * t);
                    }

                    data[top_y * width + x] = blended;
                    data[bottom_y * width + x] = blended;
                }
            }
        }

        return result;
    }

} // namespace hgl::bitmap::tile
