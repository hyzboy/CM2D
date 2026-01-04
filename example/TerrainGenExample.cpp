/**
 * Terrain Generation Example
 * Demonstrates the CM2D terrain generation system
 */

#include<hgl/2d/NoiseMap.h>
#include<hgl/2d/TerrainMap.h>
#include<hgl/2d/BitmapSave.h>
#include<iostream>

using namespace hgl;
using namespace hgl::bitmap;

// Convert float heightmap to grayscale for visualization
void ConvertHeightMapToGrayscale(const HeightMap& heightMap, BitmapGrey8& output)
{
    int w = heightMap.GetWidth();
    int h = heightMap.GetHeight();
    
    output.Create(w, h);
    
    const float* heightData = heightMap.GetData();
    uint8* outData = output.GetData();
    
    for (int i = 0; i < w * h; i++)
    {
        // Convert [0, 1] float to [0, 255] byte
        outData[i] = static_cast<uint8>(heightData[i] * 255.0f);
    }
}

// Color code biomes for visualization
void ConvertBiomeMapToColor(const BiomeMap& biomeMap, BitmapRGB8& output)
{
    int w = biomeMap.GetWidth();
    int h = biomeMap.GetHeight();
    
    output.Create(w, h);
    
    // Define colors for each biome
    math::Vector3u8 colors[] = {
        {0, 0, 139},      // Ocean - Dark Blue
        {238, 214, 175},  // Beach - Sandy
        {124, 252, 0},    // Plains - Light Green
        {34, 139, 34},    // Forest - Dark Green
        {210, 180, 140},  // Desert - Tan
        {112, 128, 144},  // Tundra - Gray-Blue
        {255, 250, 250},  // Snow - White
        {139, 137, 137},  // Mountain - Gray
        {30, 144, 255}    // River - Blue
    };
    
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            BiomeType biome = biomeMap.GetBiome(x, y);
            int biomeIdx = static_cast<int>(biome);
            if (biomeIdx < 0 || biomeIdx >= 9)
                biomeIdx = 0;
            
            math::Vector3u8* pixel = output.GetData(x, y);
            if (pixel)
                *pixel = colors[biomeIdx];
        }
    }
}

int main(int argc, char** argv)
{
    std::cout << "=== CM2D Terrain Generation Example ===" << std::endl;
    std::cout << std::endl;

    // Configuration
    const int width = 512;
    const int height = 512;
    const uint32 seed = 12345;
    const float scale = 0.005f;
    
    std::cout << "Map size: " << width << "x" << height << std::endl;
    std::cout << "Seed: " << seed << std::endl;
    std::cout << std::endl;

    // Example 1: Basic heightmap with Perlin noise
    std::cout << "1. Generating basic Perlin noise heightmap..." << std::endl;
    {
        HeightMap heightMap;
        heightMap.Create(width, height);
        
        PerlinNoise noise(seed);
        heightMap.GenerateFromNoise(noise, scale);
        heightMap.Normalize(0.0f, 1.0f);
        
        BitmapGrey8 grayscale;
        ConvertHeightMapToGrayscale(heightMap, grayscale);
        SaveBitmapToTGA(U8_TEXT("terrain_perlin.tga"), &grayscale);
        
        std::cout << "   Saved: terrain_perlin.tga" << std::endl;
    }

    // Example 2: Simplex noise
    std::cout << "2. Generating Simplex noise heightmap..." << std::endl;
    {
        HeightMap heightMap;
        heightMap.Create(width, height);
        
        SimplexNoise noise(seed);
        heightMap.GenerateFromNoise(noise, scale);
        heightMap.Normalize(0.0f, 1.0f);
        
        BitmapGrey8 grayscale;
        ConvertHeightMapToGrayscale(heightMap, grayscale);
        SaveBitmapToTGA(U8_TEXT("terrain_simplex.tga"), &grayscale);
        
        std::cout << "   Saved: terrain_simplex.tga" << std::endl;
    }

    // Example 3: Voronoi/Cellular noise
    std::cout << "3. Generating Voronoi noise pattern..." << std::endl;
    {
        HeightMap heightMap;
        heightMap.Create(width, height);
        
        VoronoiNoise noise(seed);
        heightMap.GenerateFromNoise(noise, scale * 10.0f); // Larger cells
        heightMap.Normalize(0.0f, 1.0f);
        
        BitmapGrey8 grayscale;
        ConvertHeightMapToGrayscale(heightMap, grayscale);
        SaveBitmapToTGA(U8_TEXT("terrain_voronoi.tga"), &grayscale);
        
        std::cout << "   Saved: terrain_voronoi.tga" << std::endl;
    }

    // Example 4: Fractal Brownian Motion (multi-octave terrain)
    std::cout << "4. Generating FBM multi-octave terrain..." << std::endl;
    {
        HeightMap heightMap;
        heightMap.Create(width, height);
        
        PerlinNoise* perlin = new PerlinNoise(seed);
        FractalNoise fbm(perlin, 6, 2.0f, 0.5f, true);
        heightMap.GenerateFromNoise(fbm, scale);
        heightMap.Normalize(0.0f, 1.0f);
        
        BitmapGrey8 grayscale;
        ConvertHeightMapToGrayscale(heightMap, grayscale);
        SaveBitmapToTGA(U8_TEXT("terrain_fbm.tga"), &grayscale);
        
        std::cout << "   Saved: terrain_fbm.tga" << std::endl;
    }

    // Example 5: Terrain with erosion
    std::cout << "5. Generating detailed terrain with erosion..." << std::endl;
    {
        TerrainGenerator generator(width, height, seed);
        HeightMap heightMap;
        
        generator.GenerateDetailed(heightMap, 1.0f, 6, 50);
        
        BitmapGrey8 grayscale;
        ConvertHeightMapToGrayscale(heightMap, grayscale);
        SaveBitmapToTGA(U8_TEXT("terrain_eroded.tga"), &grayscale);
        
        std::cout << "   Saved: terrain_eroded.tga" << std::endl;
    }

    // Example 6: Biome generation
    std::cout << "6. Generating biome map..." << std::endl;
    {
        TerrainGenerator generator(width, height, seed);
        HeightMap heightMap;
        BiomeMap biomeMap;
        
        generator.GenerateQuick(heightMap, 1.0f, 6);
        generator.GenerateBiomes(biomeMap, heightMap);
        
        BitmapRGB8 colorMap;
        ConvertBiomeMapToColor(biomeMap, colorMap);
        SaveBitmapToTGA(U8_TEXT("terrain_biomes.tga"), &colorMap);
        
        std::cout << "   Saved: terrain_biomes.tga" << std::endl;
    }

    // Example 7: Slope map calculation
    std::cout << "7. Generating slope map..." << std::endl;
    {
        HeightMap heightMap;
        heightMap.Create(width, height);
        
        PerlinNoise* perlin = new PerlinNoise(seed);
        FractalNoise fbm(perlin, 6, 2.0f, 0.5f, true);
        heightMap.GenerateFromNoise(fbm, scale);
        heightMap.Normalize(0.0f, 1.0f);
        
        Bitmap32F slopeMap;
        heightMap.CalculateSlopeMap(slopeMap);
        
        BitmapGrey8 grayscale;
        ConvertHeightMapToGrayscale(slopeMap, grayscale);
        SaveBitmapToTGA(U8_TEXT("terrain_slope.tga"), &grayscale);
        
        std::cout << "   Saved: terrain_slope.tga" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== Generation Complete ===" << std::endl;
    std::cout << "All terrain images saved as .tga files in the current directory." << std::endl;
    
    return 0;
}
