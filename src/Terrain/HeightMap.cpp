#include<hgl/2d/TerrainMap.h>
#include<cmath>
#include<algorithm>

namespace hgl::bitmap
{
    void HeightMap::GenerateFromNoise(const NoiseGenerator& noise, float scale,
                                     float offsetX, float offsetY)
    {
        if (!data || width <= 0 || height <= 0)
            return;

        float* ptr = data;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float nx = (x + offsetX) * scale;
                float ny = (y + offsetY) * scale;
                *ptr = noise.Generate(nx, ny);
                ptr++;
            }
        }
    }

    void HeightMap::Normalize(float minHeight, float maxHeight)
    {
        if (!data || width <= 0 || height <= 0)
            return;

        // Find current min and max
        float currentMin = data[0];
        float currentMax = data[0];
        
        int totalPixels = width * height;
        for (int i = 0; i < totalPixels; i++)
        {
            currentMin = std::min(currentMin, data[i]);
            currentMax = std::max(currentMax, data[i]);
        }

        // Avoid division by zero
        const float MIN_RANGE = 0.0001f; // Small epsilon to prevent division by zero
        float range = currentMax - currentMin;
        if (range < MIN_RANGE)
            range = 1.0f;

        // Normalize to [minHeight, maxHeight]
        float targetRange = maxHeight - minHeight;
        for (int i = 0; i < totalPixels; i++)
        {
            data[i] = minHeight + (data[i] - currentMin) / range * targetRange;
        }
    }

    void HeightMap::CalculateSlopeMap(Bitmap32F& slopeMap) const
    {
        if (!data || width <= 0 || height <= 0)
            return;

        slopeMap.Create(width, height);
        float* slopeData = slopeMap.GetData();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                // Get height at current position
                float h = data[y * width + x];

                // Calculate gradients using neighboring pixels
                float dx = 0.0f, dy = 0.0f;

                if (x > 0 && x < width - 1)
                    dx = (data[y * width + (x + 1)] - data[y * width + (x - 1)]) * 0.5f;
                if (y > 0 && y < height - 1)
                    dy = (data[(y + 1) * width + x] - data[(y - 1) * width + x]) * 0.5f;

                // Calculate slope magnitude
                float slope = std::sqrt(dx * dx + dy * dy);
                slopeData[y * width + x] = slope;
            }
        }

        // Normalize slope values to [0, 1]
        slopeMap.Normalize(0.0f, 1.0f);
    }

    void HeightMap::ApplyThermalErosion(int iterations, float talusAngle)
    {
        if (!data || width <= 0 || height <= 0)
            return;

        for (int iter = 0; iter < iterations; iter++)
        {
            for (int y = 1; y < height - 1; y++)
            {
                for (int x = 1; x < width - 1; x++)
                {
                    int idx = y * width + x;
                    float h = data[idx];

                    // Check 4-connected neighbors
                    float maxDiff = 0.0f;
                    int maxIdx = idx;

                    // Check each neighbor
                    const int offsets[4] = { -width, width, -1, 1 };
                    for (int i = 0; i < 4; i++)
                    {
                        int nIdx = idx + offsets[i];
                        float diff = h - data[nIdx];
                        if (diff > maxDiff)
                        {
                            maxDiff = diff;
                            maxIdx = nIdx;
                        }
                    }

                    // If slope exceeds talus angle, move material
                    if (maxDiff > talusAngle)
                    {
                        float amount = 0.5f * (maxDiff - talusAngle);
                        data[idx] -= amount;
                        data[maxIdx] += amount;
                    }
                }
            }
        }
    }

    void HeightMap::ApplyHydraulicErosion(int iterations, float strength)
    {
        if (!data || width <= 0 || height <= 0)
            return;

        for (int iter = 0; iter < iterations; iter++)
        {
            for (int y = 1; y < height - 1; y++)
            {
                for (int x = 1; x < width - 1; x++)
                {
                    int idx = y * width + x;
                    float h = data[idx];

                    // Find lowest neighbor
                    float minHeight = h;
                    int minIdx = idx;

                    // Check 8-connected neighbors
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        for (int dx = -1; dx <= 1; dx++)
                        {
                            if (dx == 0 && dy == 0)
                                continue;

                            int nIdx = (y + dy) * width + (x + dx);
                            if (data[nIdx] < minHeight)
                            {
                                minHeight = data[nIdx];
                                minIdx = nIdx;
                            }
                        }
                    }

                    // Move sediment downhill
                    if (minIdx != idx)
                    {
                        float diff = h - minHeight;
                        float amount = diff * strength * 0.5f;
                        data[idx] -= amount;
                        data[minIdx] += amount;
                    }
                }
            }
        }
    }

}//namespace hgl::bitmap
