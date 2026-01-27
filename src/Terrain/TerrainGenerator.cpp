#include<hgl/2d/TerrainMap.h>

namespace hgl::bitmap
{
    void TerrainGenerator::GenerateQuick(HeightMap& heightMap, float scale, int octaves)
    {
        if (!heightMap.Create(width, height))
            return;

        // Create Perlin noise with FBM (using unique_ptr for exception safety)
        PerlinNoise perlin(seed);
        FractalNoise fbm(&perlin, octaves, 2.0f, 0.5f, false);

        // Generate heightmap
        heightMap.GenerateFromNoise(fbm, scale / width);

        // Normalize to [0, 1] range
        heightMap.Normalize(0.0f, 1.0f);
    }

    void TerrainGenerator::GenerateDetailed(HeightMap& heightMap, float scale,
                                           int octaves, int erosionIterations)
    {
        // First generate basic terrain
        GenerateQuick(heightMap, scale, octaves);

        // Apply erosion if requested
        if (erosionIterations > 0)
        {
            // Apply thermal erosion (for steep slopes)
            heightMap.ApplyThermalErosion(erosionIterations / 2, 0.7f);

            // Apply hydraulic erosion (for water flow)
            heightMap.ApplyHydraulicErosion(erosionIterations / 2, 0.1f);

            // Re-normalize after erosion
            heightMap.Normalize(0.0f, 1.0f);
        }
    }

    void TerrainGenerator::GenerateBiomes(BiomeMap& biomeMap, const HeightMap& heightMap,
                                         float tempScale, float moistScale)
    {
        if (heightMap.GetWidth() != width || heightMap.GetHeight() != height)
            return;

        // Generate temperature map with well-separated seed
        Bitmap32F temperatureMap;
        temperatureMap.Create(width, height);
        PerlinNoise tempNoise(seed ^ 0x9E3779B9); // Use XOR with golden ratio for seed independence
        FractalNoise tempFBM(&tempNoise, 4, 2.0f, 0.5f, false);
        temperatureMap.GenerateFromNoise(tempFBM, tempScale / width);
        temperatureMap.Normalize(0.0f, 1.0f);

        // Generate moisture map with different seed offset
        Bitmap32F moistureMap;
        moistureMap.Create(width, height);
        PerlinNoise moistNoise(seed ^ 0x517CC1B7); // Use XOR with different constant
        FractalNoise moistFBM(&moistNoise, 4, 2.0f, 0.5f, false);
        moistureMap.GenerateFromNoise(moistFBM, moistScale / width);
        moistureMap.Normalize(0.0f, 1.0f);

        // Generate biome map from height, temperature, and moisture
        biomeMap.GenerateFromMaps(heightMap, &temperatureMap, &moistureMap);
    }

}//namespace hgl::bitmap
