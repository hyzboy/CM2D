#include<hgl/2d/NoiseMap.h>
#include<cmath>
#include<algorithm>

namespace hgl::bitmap
{
    VoronoiNoise::VoronoiNoise(uint32 seed) : NoiseGenerator(seed)
    {
    }

    float VoronoiNoise::Hash2D(float x, float y) const
    {
        // Hash function for 2D coordinates
        float n = std::sin(x * 12.9898f + y * 78.233f + seed * 0.001f) * 43758.5453f;
        return n - std::floor(n);
    }

    void VoronoiNoise::GetCellPoint(int ix, int iy, float& px, float& py) const
    {
        // Generate pseudo-random point within cell
        px = ix + Hash2D(ix, iy);
        py = iy + Hash2D(ix + 1.0f, iy + 1.0f);
    }

    float VoronoiNoise::Generate(float x, float y) const
    {
        // Get integer cell coordinates
        int ix = static_cast<int>(std::floor(x));
        int iy = static_cast<int>(std::floor(y));

        // Start with large distance value (maximum possible distance for 3x3 grid search)
        // Maximum distance in a 3x3 unit cell grid is ~sqrt(8) ≈ 2.83
        const float MAX_SEARCH_DISTANCE = 3.0f;
        float minDist = MAX_SEARCH_DISTANCE;

        // Check 3x3 grid of cells around point
        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                float px, py;
                GetCellPoint(ix + dx, iy + dy, px, py);

                // Calculate distance to cell point
                float dx_val = x - px;
                float dy_val = y - py;
                float dist = std::sqrt(dx_val * dx_val + dy_val * dy_val);

                minDist = std::min(minDist, dist);
            }
        }

        // Normalize to approximately [-1, 1]
        // Typical min distance is ~0, max is ~sqrt(2) for unit cells
        // Scale down by sqrt(2) to get [0, 1], then convert to [-1, 1]
        const float INV_SQRT_TWO = 0.70710678f; // 1/sqrt(2)
        return (minDist * INV_SQRT_TWO) * 2.0f - 1.0f;
    }

}//namespace hgl::bitmap
