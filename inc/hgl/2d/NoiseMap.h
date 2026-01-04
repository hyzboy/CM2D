#pragma once

#include<hgl/CoreType.h>
#include<hgl/2d/Bitmap.h>
#include<vector>
#include<cmath>

namespace hgl::bitmap
{
    /**
     * Abstract base class for all noise generators
     * Provides seedable random generation for procedural content
     */
    class NoiseGenerator
    {
    protected:
        uint32 seed;

    public:
        NoiseGenerator(uint32 s = 12345) : seed(s) {}
        virtual ~NoiseGenerator() = default;

        /**
         * Generate noise value at given 2D coordinates
         * @param x X coordinate
         * @param y Y coordinate
         * @return Noise value in range [-1, 1]
         */
        virtual float Generate(float x, float y) const = 0;

        void SetSeed(uint32 s) { seed = s; }
        uint32 GetSeed() const { return seed; }
    };

    /**
     * Classic Perlin noise implementation
     * Uses permutation table and gradient-based interpolation
     */
    class PerlinNoise : public NoiseGenerator
    {
    private:
        std::vector<int> permutation;

        float Fade(float t) const;
        float Lerp(float t, float a, float b) const;
        float Grad(int hash, float x, float y) const;

    public:
        PerlinNoise(uint32 seed = 12345);
        virtual ~PerlinNoise() = default;

        float Generate(float x, float y) const override;
    };

    /**
     * Simplex noise implementation
     * Faster than Perlin with better visual quality and fewer artifacts
     */
    class SimplexNoise : public NoiseGenerator
    {
    private:
        std::vector<int> permutation;
        
        struct Grad
        {
            float x, y;
        };
        
        static const Grad gradients[8];

        int FastFloor(float x) const;
        float Dot(const Grad& g, float x, float y) const;

    public:
        SimplexNoise(uint32 seed = 12345);
        virtual ~SimplexNoise() = default;

        float Generate(float x, float y) const override;
    };

    /**
     * Voronoi/Cellular noise implementation
     * Generates region-based patterns useful for biomes and textures
     */
    class VoronoiNoise : public NoiseGenerator
    {
    private:
        float Hash2D(float x, float y) const;
        void GetCellPoint(int ix, int iy, float& px, float& py) const;

    public:
        VoronoiNoise(uint32 seed = 12345);
        virtual ~VoronoiNoise() = default;

        float Generate(float x, float y) const override;
    };

    /**
     * Fractal Brownian Motion (FBM) noise
     * Combines multiple octaves of noise for natural-looking terrain
     */
    class FractalNoise : public NoiseGenerator
    {
    private:
        NoiseGenerator* baseNoise;
        bool ownsNoise;
        int octaves;
        float lacunarity;
        float persistence;

    public:
        /**
         * @param noise Base noise generator (will be owned if ownNoise is true)
         * @param oct Number of octaves (detail levels)
         * @param lac Frequency multiplier per octave (typically 2.0)
         * @param pers Amplitude multiplier per octave (typically 0.5)
         * @param ownNoise If true, will delete noise generator in destructor
         */
        FractalNoise(NoiseGenerator* noise, int oct = 6, float lac = 2.0f, 
                     float pers = 0.5f, bool ownNoise = true);
        virtual ~FractalNoise();

        float Generate(float x, float y) const override;

        void SetOctaves(int oct) { octaves = oct; }
        void SetLacunarity(float lac) { lacunarity = lac; }
        void SetPersistence(float pers) { persistence = pers; }

        int GetOctaves() const { return octaves; }
        float GetLacunarity() const { return lacunarity; }
        float GetPersistence() const { return persistence; }
    };

}//namespace hgl::bitmap
