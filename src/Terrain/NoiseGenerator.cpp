#include<hgl/2d/NoiseMap.h>
#include<cmath>

namespace hgl::bitmap
{
    // NoiseGenerator base class implementation
    // Base class is mostly header-only, but this file exists for future extensions

    // FractalNoise implementation
    FractalNoise::FractalNoise(NoiseGenerator* noise, int oct, float lac,
                               float pers, bool ownNoise)
        : NoiseGenerator(noise ? noise->GetSeed() : 12345)
        , baseNoise(noise)
        , ownsNoise(ownNoise)
        , octaves(oct)
        , lacunarity(lac)
        , persistence(pers)
    {
    }

    FractalNoise::~FractalNoise()
    {
        if (ownsNoise && baseNoise)
        {
            delete baseNoise;
        }
    }

    float FractalNoise::Generate(float x, float y) const
    {
        if (!baseNoise)
            return 0.0f;

        float total = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; i++)
        {
            total += baseNoise->Generate(x * frequency, y * frequency) * amplitude;

            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        // Normalize to [-1, 1]
        return total / maxValue;
    }

}//namespace hgl::bitmap
