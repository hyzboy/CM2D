#pragma once

#include<hgl/CoreType.h>
#include<hgl/2d/Bitmap.h>
#include<hgl/2d/NoiseMap.h>

namespace hgl::bitmap
{
    /**
     * HeightMap class for terrain elevation data
     * Extends Bitmap32F (float bitmap) for height storage
     */
    class HeightMap : public Bitmap32F
    {
    public:
        HeightMap() : Bitmap32F() {}
        virtual ~HeightMap() = default;

        /**
         * Generate heightmap from noise generator
         * @param noise Noise generator to use
         * @param scale Scale factor for noise coordinates
         * @param offsetX X offset for noise sampling
         * @param offsetY Y offset for noise sampling
         */
        void GenerateFromNoise(const NoiseGenerator& noise, float scale = 1.0f, 
                              float offsetX = 0.0f, float offsetY = 0.0f);

        /**
         * Normalize height values to specified range
         * @param minHeight Minimum height value
         * @param maxHeight Maximum height value
         */
        void Normalize(float minHeight = 0.0f, float maxHeight = 1.0f);

        /**
         * Calculate slope/gradient map
         * @param slopeMap Output slope map (values 0-1, where 1 is steepest)
         */
        void CalculateSlopeMap(Bitmap32F& slopeMap) const;

        /**
         * Apply thermal erosion simulation
         * @param iterations Number of erosion iterations
         * @param talusAngle Angle of repose (typical: 0.5-0.9)
         */
        void ApplyThermalErosion(int iterations, float talusAngle = 0.7f);

        /**
         * Apply simplified hydraulic erosion
         * @param iterations Number of erosion iterations
         * @param strength Erosion strength (0-1)
         */
        void ApplyHydraulicErosion(int iterations, float strength = 0.1f);
    };

    /**
     * Biome types for terrain classification
     */
    enum class BiomeType : uint8
    {
        Ocean = 0,
        Beach = 1,
        Plains = 2,
        Forest = 3,
        Desert = 4,
        Tundra = 5,
        Snow = 6,
        Mountain = 7,
        River = 8
    };

    /**
     * BiomeMap class for biome distribution
     * Extends BitmapGrey8 (8-bit grayscale) where each value represents a biome type
     */
    class BiomeMap : public BitmapGrey8
    {
    public:
        BiomeMap() : BitmapGrey8() {}
        virtual ~BiomeMap() = default;

        /**
         * Generate biome map from height, temperature, and moisture maps
         * @param heightMap Height/elevation data
         * @param temperatureMap Temperature data (optional, can be nullptr)
         * @param moistureMap Moisture data (optional, can be nullptr)
         */
        void GenerateFromMaps(const HeightMap& heightMap, 
                             const Bitmap32F* temperatureMap = nullptr,
                             const Bitmap32F* moistureMap = nullptr);

        /**
         * Get biome type at specific coordinates
         * @param x X coordinate
         * @param y Y coordinate
         * @return BiomeType at given position
         */
        BiomeType GetBiome(int x, int y) const;

        /**
         * Set biome type at specific coordinates
         * @param x X coordinate
         * @param y Y coordinate
         * @param biome BiomeType to set
         */
        void SetBiome(int x, int y, BiomeType biome);
    };

    /**
     * High-level terrain generator orchestrating the generation process
     */
    class TerrainGenerator
    {
    private:
        uint32 seed;
        int width;
        int height;

    public:
        TerrainGenerator(int w, int h, uint32 s = 12345) 
            : width(w), height(h), seed(s) {}

        /**
         * Generate quick terrain without erosion
         * @param heightMap Output heightmap
         * @param scale Noise scale factor
         * @param octaves Number of FBM octaves
         */
        void GenerateQuick(HeightMap& heightMap, float scale = 1.0f, int octaves = 6);

        /**
         * Generate detailed terrain with erosion simulation
         * @param heightMap Output heightmap
         * @param scale Noise scale factor
         * @param octaves Number of FBM octaves
         * @param erosionIterations Number of erosion passes
         */
        void GenerateDetailed(HeightMap& heightMap, float scale = 1.0f, 
                            int octaves = 6, int erosionIterations = 100);

        /**
         * Generate biome distribution map
         * @param biomeMap Output biome map
         * @param heightMap Input heightmap
         * @param tempScale Temperature noise scale
         * @param moistScale Moisture noise scale
         */
        void GenerateBiomes(BiomeMap& biomeMap, const HeightMap& heightMap,
                           float tempScale = 0.5f, float moistScale = 0.5f);

        void SetSeed(uint32 s) { seed = s; }
        uint32 GetSeed() const { return seed; }
    };

}//namespace hgl::bitmap
