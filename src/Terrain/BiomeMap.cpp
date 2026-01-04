#include<hgl/2d/TerrainMap.h>

namespace hgl::bitmap
{
    void BiomeMap::GenerateFromMaps(const HeightMap& heightMap,
                                   const Bitmap32F* temperatureMap,
                                   const Bitmap32F* moistureMap)
    {
        int w = heightMap.GetWidth();
        int h = heightMap.GetHeight();
        
        if (!Create(w, h))
            return;

        const float* heightData = heightMap.GetData();
        const float* tempData = temperatureMap ? temperatureMap->GetData() : nullptr;
        const float* moistData = moistureMap ? moistureMap->GetData() : nullptr;

        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                int idx = y * w + x;
                float height = heightData[idx];
                float temp = tempData ? tempData[idx] : 0.5f;
                float moist = moistData ? moistData[idx] : 0.5f;

                BiomeType biome;

                // Height-based classification
                if (height < 0.3f)
                {
                    biome = BiomeType::Ocean;
                }
                else if (height < 0.35f)
                {
                    biome = BiomeType::Beach;
                }
                else if (height > 0.75f)
                {
                    biome = BiomeType::Mountain;
                }
                else if (height > 0.7f)
                {
                    // High elevation: Snow or Tundra
                    biome = temp > 0.5f ? BiomeType::Tundra : BiomeType::Snow;
                }
                else
                {
                    // Mid-range elevation: use temperature and moisture
                    if (temp < 0.3f)
                    {
                        // Cold
                        biome = BiomeType::Tundra;
                    }
                    else if (temp > 0.7f)
                    {
                        // Hot
                        biome = moist > 0.5f ? BiomeType::Forest : BiomeType::Desert;
                    }
                    else
                    {
                        // Temperate
                        if (moist < 0.3f)
                            biome = BiomeType::Plains;
                        else if (moist > 0.6f)
                            biome = BiomeType::Forest;
                        else
                            biome = BiomeType::Plains;
                    }
                }

                SetBiome(x, y, biome);
            }
        }
    }

    BiomeType BiomeMap::GetBiome(int x, int y) const
    {
        const uint8* pixel = GetData(x, y);
        if (!pixel)
            return BiomeType::Ocean;
        
        return static_cast<BiomeType>(*pixel);
    }

    void BiomeMap::SetBiome(int x, int y, BiomeType biome)
    {
        uint8* pixel = GetData(x, y);
        if (pixel)
            *pixel = static_cast<uint8>(biome);
    }

}//namespace hgl::bitmap
