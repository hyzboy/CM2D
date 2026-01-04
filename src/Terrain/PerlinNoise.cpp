#include<hgl/2d/NoiseMap.h>
#include<cmath>
#include<algorithm>

namespace hgl::bitmap
{
    PerlinNoise::PerlinNoise(uint32 seed) : NoiseGenerator(seed)
    {
        // Initialize permutation table
        permutation.resize(512);
        
        // Fill with values 0-255
        std::vector<int> p(256);
        for (int i = 0; i < 256; i++)
            p[i] = i;
        
        // Shuffle using seed
        uint32 s = seed;
        for (int i = 255; i > 0; i--)
        {
            s = s * 1103515245 + 12345; // Linear congruential generator
            int j = (s / 65536) % (i + 1);
            std::swap(p[i], p[j]);
        }
        
        // Duplicate for overflow handling
        for (int i = 0; i < 256; i++)
        {
            permutation[i] = p[i];
            permutation[256 + i] = p[i];
        }
    }

    float PerlinNoise::Fade(float t) const
    {
        // Smoothstep function: 6t^5 - 15t^4 + 10t^3
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    float PerlinNoise::Lerp(float t, float a, float b) const
    {
        return a + t * (b - a);
    }

    float PerlinNoise::Grad(int hash, float x, float y) const
    {
        // Convert hash to gradient direction (16 directions)
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    float PerlinNoise::Generate(float x, float y) const
    {
        // Find unit grid cell containing point
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        
        // Get relative position within cell
        x -= std::floor(x);
        y -= std::floor(y);
        
        // Compute fade curves
        float u = Fade(x);
        float v = Fade(y);
        
        // Hash coordinates of the 4 cube corners
        int A = permutation[X] + Y;
        int AA = permutation[A];
        int AB = permutation[A + 1];
        int B = permutation[X + 1] + Y;
        int BA = permutation[B];
        int BB = permutation[B + 1];
        
        // Blend results from 4 corners
        float res = Lerp(v,
            Lerp(u, Grad(permutation[AA], x, y),
                    Grad(permutation[BA], x - 1, y)),
            Lerp(u, Grad(permutation[AB], x, y - 1),
                    Grad(permutation[BB], x - 1, y - 1)));
        
        // Normalize to [-1, 1]
        return res;
    }

}//namespace hgl::bitmap
