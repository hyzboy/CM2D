#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/Resize.h>
#include<vector>
#include<algorithm>

/**
 * Mipmap Generation Module
 * 
 * Generates mipmap chains for textures with automatic level generation.
 * Useful for GPU texture loading and LOD (Level of Detail) systems.
 * 
 * Example usage:
 * ```cpp
 * BitmapRGB8 base_texture;
 * base_texture.Create(512, 512);
 * 
 * // Generate complete mipmap chain
 * auto mipmaps = hgl::bitmap::mipmap::GenerateMipMaps(base_texture);
 * 
 * // Access specific levels
 * const auto& level0 = mipmaps.GetLevel(0); // Original 512x512
 * const auto& level1 = mipmaps.GetLevel(1); // 256x256
 * const auto& level2 = mipmaps.GetLevel(2); // 128x128
 * 
 * // Custom configuration
 * hgl::bitmap::mipmap::MipMapConfig config;
 * config.filter = resize::FilterType::NearestNeighbor;
 * config.min_size = 16; // Stop at 16x16
 * auto custom_mipmaps = hgl::bitmap::mipmap::GenerateMipMaps(base_texture, config);
 * ```
 */

namespace hgl::bitmap::mipmap
{
    /**
     * Configuration for mipmap generation
     */
    struct MipMapConfig
    {
        resize::FilterType filter = resize::FilterType::Bilinear;  // Downsampling filter
        int max_levels = -1;        // Maximum number of levels (-1 = generate until 1x1)
        int min_size = 1;           // Minimum dimension size (stops when width or height reaches this)
    };

    /**
     * Mipmap chain container
     * 
     * Stores multiple levels of a texture, where each level is half the size of the previous.
     * Level 0 is the base (full resolution) texture.
     */
    template<typename T, uint C>
    class MipMapChain
    {
    private:
        std::vector<Bitmap<T, C>> levels_;

    public:
        MipMapChain() = default;

        /**
         * Generate mipmap levels from a base texture
         * 
         * @param base Base texture (level 0)
         * @param filter Downsampling filter
         * 
         * Generates levels until the smallest dimension reaches 1 pixel.
         */
        void Generate(const Bitmap<T, C>& base, 
                     resize::FilterType filter = resize::FilterType::Bilinear)
        {
            MipMapConfig config;
            config.filter = filter;
            GenerateWithConfig(base, config);
        }

        /**
         * Generate mipmap levels with custom configuration
         * 
         * @param base Base texture (level 0)
         * @param config Mipmap generation configuration
         */
        void GenerateWithConfig(const Bitmap<T, C>& base, const MipMapConfig& config)
        {
            levels_.clear();

            const int base_width = base.GetWidth();
            const int base_height = base.GetHeight();

            if (base_width == 0 || base_height == 0 || !base.GetData())
                return;

            // Level 0 is the original image (copy it)
            Bitmap<T, C> level0;
            level0.Create(base_width, base_height);
            // Using memcpy for POD types (consistent with Bitmap::Flip implementation)
            memcpy(level0.GetData(), base.GetData(), base.GetTotalBytes());
            levels_.push_back(std::move(level0));

            // Generate subsequent levels
            int current_width = base_width;
            int current_height = base_height;
            int level_count = 1;

            while (true)
            {
                // Calculate next level dimensions (half size, minimum 1)
                int next_width = std::max(1, current_width / 2);
                int next_height = std::max(1, current_height / 2);

                // Check stopping conditions
                if (config.max_levels > 0 && level_count >= config.max_levels)
                    break;

                if (std::min(next_width, next_height) < config.min_size)
                    break;

                // Stop if we've reached 1x1
                if (current_width == 1 && current_height == 1)
                    break;

                // Generate next level by downsampling
                auto next_level = resize::Resize(levels_.back(), next_width, next_height, config.filter);
                levels_.push_back(std::move(next_level));

                current_width = next_width;
                current_height = next_height;
                level_count++;
            }
        }

        /**
         * Get number of mipmap levels
         */
        size_t GetLevelCount() const
        {
            return levels_.size();
        }

        /**
         * Get a specific mipmap level
         * 
         * @param index Level index (0 = base level)
         * @return Reference to the bitmap at the specified level
         */
        Bitmap<T, C>& GetLevel(size_t index)
        {
            return levels_[index];
        }

        /**
         * Get a specific mipmap level (const version)
         * 
         * @param index Level index (0 = base level)
         * @return Const reference to the bitmap at the specified level
         */
        const Bitmap<T, C>& GetLevel(size_t index) const
        {
            return levels_[index];
        }

        /**
         * Check if chain is empty
         */
        bool IsEmpty() const
        {
            return levels_.empty();
        }

        /**
         * Clear all levels
         */
        void Clear()
        {
            levels_.clear();
        }

        /**
         * Get the base level (level 0)
         */
        Bitmap<T, C>& GetBaseLevel()
        {
            return levels_[0];
        }

        /**
         * Get the base level (level 0, const version)
         */
        const Bitmap<T, C>& GetBaseLevel() const
        {
            return levels_[0];
        }
    };

    /**
     * Generate complete mipmap chain from a source bitmap
     * 
     * @param source Source bitmap (will become level 0)
     * @param config Mipmap generation configuration
     * @return MipMapChain containing all generated levels
     * 
     * This is a convenience function that creates and returns a MipMapChain.
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
     * Calculate the number of mipmap levels for given dimensions
     * 
     * @param width Image width
     * @param height Image height
     * @param min_size Minimum dimension size
     * @return Number of mipmap levels (including base level)
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
