# Terrain Generation System

The CM2D Terrain Generation System provides a comprehensive set of tools for procedural generation of heightmaps, biomes, and terrain features for game development and simulation applications.

## Features

### Noise Generators

- **PerlinNoise**: Classic Perlin noise with permutation table-based implementation
- **SimplexNoise**: Faster simplex noise with fewer artifacts and better visual quality
- **VoronoiNoise**: Cellular/Voronoi noise for region-based generation (useful for biomes)
- **FractalNoise**: Fractal Brownian Motion (FBM) combining multiple octaves for natural-looking terrain

### Terrain Maps

- **HeightMap**: Float-based elevation data with generation and erosion capabilities
- **BiomeMap**: Classification of terrain into biomes (Ocean, Beach, Plains, Forest, Desert, Tundra, Snow, Mountain, River)
- **TerrainGenerator**: High-level orchestration for complete terrain generation

### Erosion Simulation

- **Thermal Erosion**: Simulates slope-based material movement
- **Hydraulic Erosion**: Simulates water-based sediment transport

## Usage

### Basic Heightmap Generation

```cpp
#include<hgl/2d/NoiseMap.h>
#include<hgl/2d/TerrainMap.h>

using namespace hgl::bitmap;

// Create a heightmap
HeightMap heightMap;
heightMap.Create(512, 512);

// Generate using Perlin noise
PerlinNoise noise(12345); // seed
heightMap.GenerateFromNoise(noise, 0.005f); // scale
heightMap.Normalize(0.0f, 1.0f);
```

### Multi-Octave Terrain (FBM)

```cpp
// Create fractal noise with multiple octaves
PerlinNoise* perlin = new PerlinNoise(12345);
FractalNoise fbm(perlin, 
    6,      // octaves (detail levels)
    2.0f,   // lacunarity (frequency multiplier)
    0.5f,   // persistence (amplitude multiplier)
    true    // owns the base noise generator
);

heightMap.GenerateFromNoise(fbm, 0.005f);
heightMap.Normalize(0.0f, 1.0f);
```

### Terrain with Erosion

```cpp
TerrainGenerator generator(512, 512, 12345);
HeightMap heightMap;

// Generate detailed terrain with erosion
generator.GenerateDetailed(
    heightMap,
    1.0f,   // scale
    6,      // octaves
    50      // erosion iterations
);
```

### Biome Generation

```cpp
TerrainGenerator generator(512, 512, 12345);
HeightMap heightMap;
BiomeMap biomeMap;

// Generate heightmap
generator.GenerateQuick(heightMap, 1.0f, 6);

// Generate biomes from height, temperature, and moisture
generator.GenerateBiomes(biomeMap, heightMap);

// Query biome at specific location
BiomeType biome = biomeMap.GetBiome(x, y);
```

### Slope Calculation

```cpp
HeightMap heightMap;
// ... generate heightmap ...

Bitmap32F slopeMap;
heightMap.CalculateSlopeMap(slopeMap);
```

## API Reference

### NoiseGenerator

Abstract base class for all noise generators.

**Methods:**
- `float Generate(float x, float y) const` - Generate noise value at coordinates (returns [-1, 1])
- `void SetSeed(uint32 seed)` - Set random seed
- `uint32 GetSeed() const` - Get current seed

### PerlinNoise

Classic Perlin noise implementation.

**Constructor:**
- `PerlinNoise(uint32 seed = 12345)`

### SimplexNoise

Faster simplex noise with better quality.

**Constructor:**
- `SimplexNoise(uint32 seed = 12345)`

### VoronoiNoise

Cellular/Voronoi noise for region-based patterns.

**Constructor:**
- `VoronoiNoise(uint32 seed = 12345)`

### FractalNoise

Fractal Brownian Motion combining multiple octaves.

**Constructor:**
- `FractalNoise(NoiseGenerator* noise, int octaves = 6, float lacunarity = 2.0f, float persistence = 0.5f, bool ownNoise = true)`

**Methods:**
- `void SetOctaves(int octaves)`
- `void SetLacunarity(float lacunarity)`
- `void SetPersistence(float persistence)`

### HeightMap

Extends `Bitmap32F` for terrain elevation.

**Methods:**
- `void GenerateFromNoise(const NoiseGenerator& noise, float scale = 1.0f, float offsetX = 0.0f, float offsetY = 0.0f)`
- `void Normalize(float minHeight = 0.0f, float maxHeight = 1.0f)`
- `void CalculateSlopeMap(Bitmap32F& slopeMap) const`
- `void ApplyThermalErosion(int iterations, float talusAngle = 0.7f)`
- `void ApplyHydraulicErosion(int iterations, float strength = 0.1f)`

### BiomeMap

Extends `BitmapGrey8` for biome classification.

**Biome Types:**
- Ocean, Beach, Plains, Forest, Desert, Tundra, Snow, Mountain, River

**Methods:**
- `void GenerateFromMaps(const HeightMap& heightMap, const Bitmap32F* temperatureMap = nullptr, const Bitmap32F* moistureMap = nullptr)`
- `BiomeType GetBiome(int x, int y) const`
- `void SetBiome(int x, int y, BiomeType biome)`

### TerrainGenerator

High-level terrain generation orchestration.

**Constructor:**
- `TerrainGenerator(int width, int height, uint32 seed = 12345)`

**Methods:**
- `void GenerateQuick(HeightMap& heightMap, float scale = 1.0f, int octaves = 6)`
- `void GenerateDetailed(HeightMap& heightMap, float scale = 1.0f, int octaves = 6, int erosionIterations = 100)`
- `void GenerateBiomes(BiomeMap& biomeMap, const HeightMap& heightMap, float tempScale = 0.5f, float moistScale = 0.5f)`

## Examples

See `example/TerrainGenExample.cpp` for a comprehensive demonstration including:
- Basic noise generation
- Multi-octave terrain
- Erosion simulation
- Biome generation
- Slope calculation
- TGA export for visualization

To build the example:
```bash
cmake -DCM2D_BUILD_EXAMPLES=ON ..
make
./TerrainGenExample
```

## Performance Considerations

- **Large Maps**: The system handles maps up to 2048x2048+ efficiently
- **Erosion**: Each erosion iteration processes the entire map; adjust iteration count based on desired quality vs. performance
- **Memory**: Height data stored as 32-bit floats (4 bytes per pixel)
- **Multi-threading**: Current implementation is single-threaded but structured for OpenMP parallelization

## Integration with CM2D

The terrain system integrates seamlessly with CM2D's `Bitmap<T, C>` template system:
- HeightMap extends `Bitmap32F` (single-channel float)
- BiomeMap extends `BitmapGrey8` (single-channel byte)
- All bitmap operations (flip, clear, etc.) work on terrain maps
- Export to TGA using `SaveBitmapToTGA()` for visualization

## Algorithm Details

### Perlin Noise
- 256-element permutation table for consistent pseudo-randomness
- 16-direction gradient function
- Smoothstep fade: `t³(t(6t - 15) + 10)`
- Bilinear interpolation

### Simplex Noise
- Skewed grid traversal for efficient sampling
- 3 gradient contributions per sample
- ~40% faster than Perlin with better visual quality

### Erosion
- **Thermal**: Moves material when slope exceeds angle of repose
- **Hydraulic**: Simplified model moving sediment from high to low areas
- Both use iterative relaxation

### Biome Classification
- Height-based for extremes (ocean, mountain)
- Temperature + moisture matrix for mid-range elevations
- Thresholds: Ocean < 0.3, Beach < 0.35, Mountain > 0.75

## Future Enhancements

Potential additions to the system:
- GPU acceleration for large-scale generation
- Advanced hydraulic erosion with water simulation
- River network generation
- Cave system generation
- Vegetation placement
- Road/path network generation
