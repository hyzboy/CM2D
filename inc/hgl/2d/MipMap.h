#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/Resize.h>
#include<vector>
#include<algorithm>

/**
 * Mipmap 生成模块
 *
 * 为纹理生成 mipmap 链（自动生成各级别）。
 * 适用于GPU纹理加载与LOD（细节层级）系统。
 *
 * 示例用法：
 * ```cpp
 * BitmapRGB8 base_texture;
 * base_texture.Create(512, 512);
 *
 * // 生成完整的mipmap链
 * auto mipmaps = hgl::bitmap::mipmap::GenerateMipMaps(base_texture);
 *
 * // 访问特定级别
 * const auto& level0 = mipmaps.GetLevel(0); // 原始 512x512
 * const auto& level1 = mipmaps.GetLevel(1); // 256x256
 * const auto& level2 = mipmaps.GetLevel(2); // 128x128
 *
 * // 自定义配置
 * hgl::bitmap::mipmap::MipMapConfig config;
 * config.filter = resize::FilterType::NearestNeighbor;
 * config.min_size = 16; // 在16x16处停止
 * auto custom_mipmaps = hgl::bitmap::mipmap::GenerateMipMaps(base_texture, config);
 * ```
 */

namespace hgl::bitmap::mipmap
{
    /**
     * mipmap 生成配置
     */
    struct MipMapConfig
    {
        resize::FilterType filter = resize::FilterType::Bilinear;  // Downsampling filter
        int max_levels = -1;        // Maximum number of levels (-1 = generate until 1x1)
        int min_size = 1;           // Minimum dimension size (stops when width or height reaches this)
    };

    /**
     * Mipmap 链容器
     *
     * 存储纹理的多个级别，每一级通常是前一等级的一半尺寸。
     * 级别0为基础（原始分辨率）纹理。
     */
    template<typename T, uint C>
    class MipMapChain
    {
    private:
        std::vector<Bitmap<T, C>> levels_;

    public:
        MipMapChain() = default;

        /**
         * 从基础纹理生成mipmap级别
         *
         * @param base 基础纹理（级别0）
         * @param filter 降采样使用的滤波器
         *
         * 生成级别直到最小维度达到1像素为止。
         */
        void Generate(const Bitmap<T, C>& base,
                     resize::FilterType filter = resize::FilterType::Bilinear)
        {
            MipMapConfig config;
            config.filter = filter;
            GenerateWithConfig(base, config);
        }

        /**
         * 使用自定义配置生成mipmap级别
         *
         * @param base 基础纹理（级别0）
         * @param config mipmap 生成配置
         */
        void GenerateWithConfig(const Bitmap<T, C>& base, const MipMapConfig& config)
        {
            levels_.clear();

            const int base_width = base.GetWidth();
            const int base_height = base.GetHeight();

            if (base_width == 0 || base_height == 0 || !base.GetData())
                return;

            // 级别0为原始图像（复制）
            Bitmap<T, C> level0;
            level0.Create(base_width, base_height);
            // Using memcpy for POD types (consistent with Bitmap::Flip implementation)
            memcpy(level0.GetData(), base.GetData(), base.GetTotalBytes());
            levels_.push_back(std::move(level0));

            // 生成后续级别
            int current_width = base_width;
            int current_height = base_height;
            int level_count = 1;

            while (true)
            {
                // 计算下一级的尺寸（减半，最小为1）
                int next_width = std::max(1, current_width / 2);
                int next_height = std::max(1, current_height / 2);

                // 检查停止条件
                if (config.max_levels > 0 && level_count >= config.max_levels)
                    break;

                if (std::min(next_width, next_height) < config.min_size)
                    break;

                // 如果已达到1x1则停止
                if (current_width == 1 && current_height == 1)
                    break;

                // 通过降采样生成下一级
                auto next_level = resize::Resize(levels_.back(), next_width, next_height, config.filter);
                levels_.push_back(std::move(next_level));

                current_width = next_width;
                current_height = next_height;
                level_count++;
            }
        }

        /**
         * 获取mipmap级别数量
         */
        size_t GetLevelCount() const
        {
            return levels_.size();
        }

        /**
         * 获取指定的mipmap级别
         *
         * @param index 级别索引（0 = 基础级别）
         * @return 指定级别的位图引用
         */
        Bitmap<T, C>& GetLevel(size_t index)
        {
            return levels_[index];
        }

        /**
         * 获取指定的mipmap级别（只读版本）
         *
         * @param index 级别索引（0 = 基础级别）
         * @return 指定级别的位图常量引用
         */
        const Bitmap<T, C>& GetLevel(size_t index) const
        {
            return levels_[index];
        }

        /**
         * 检查链是否为空
         */
        bool IsEmpty() const
        {
            return levels_.empty();
        }

        /**
         * 清除所有级别
         */
        void Clear()
        {
            levels_.clear();
        }

        /**
         * 获取基础级别（级别0）
         */
        Bitmap<T, C>& GetBaseLevel()
        {
            return levels_[0];
        }

        /**
         * 获取基础级别（级别0，只读版本）
         */
        const Bitmap<T, C>& GetBaseLevel() const
        {
            return levels_[0];
        }
    };

    /**
     * 从源位图生成完整的mipmap链
     *
     * @param source 源位图（将作为级别0）
     * @param config mipmap 生成配置
     * @return 包含所有生成级别的MipMapChain
     *
     * 这是一个便捷函数，创建并返回一个 MipMapChain。
     */
    template<typename T, uint C>
    MipMapChain<T, C> GenerateMipMaps(const Bitmap<T, C>& source,
                                       const MipMapConfig& config = MipMapConfig())
    {
        MipMapChain<T, C> chain;
        chain.GenerateWithConfig(source, config);
        return chain;
    }

    /**
     * 计算给定尺寸下的mipmap级别数
     *
     * @param width 图像宽度
     * @param height 图像高度
     * @param min_size 最小维度大小
     * @return mipmap级别数（包括基础级别）
     */
    inline int CalculateMipMapLevels(int width, int height, int min_size = 1)
    {
        if (width <= 0 || height <= 0)
            return 0;

        int levels = 1;
        int current_width = width;
        int current_height = height;

        while (true)
        {
            if (current_width == 1 && current_height == 1)
                break;

            int next_width = std::max(1, current_width / 2);
            int next_height = std::max(1, current_height / 2);

            if (std::min(next_width, next_height) < min_size)
                break;

            current_width = next_width;
            current_height = next_height;
            levels++;
        }

        return levels;
    }

} // namespace hgl::bitmap::mipmap
