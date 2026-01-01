# BlendColor Usage Examples

This document provides examples of how to use the refactored `BlendColor` system in CM2D.

## Overview

The `BlendColor` class has been refactored to support flexible blending modes using function pointers instead of virtual functions. This provides better performance and supports all blend modes from CMMath.

## Basic Usage

### Using Predefined Blend Modes

```cpp
#include <hgl/2d/Blend.h>

using namespace hgl;
using namespace hgl::bitmap;

// Create a blend color instance
BlendColorRGBA8 blend;

// Set blend mode
blend.SetBlendMode(AlphaBlendMode::Add);

// Perform blending
math::Vector4u8 src(255, 128, 64, 255);  // Source color
math::Vector4u8 dst(128, 64, 32, 255);   // Destination color
math::Vector4u8 result = blend(src, dst, 0.5f);  // Blend with 50% alpha
```

### Supported Blend Modes

The following blend modes from `AlphaBlendMode` enum are supported:

- `Normal` - Standard alpha blending
- `Add` - Additive blending
- `Subtract` - Subtractive blending
- `Multiply` - Multiplicative blending (darken)
- `Screen` - Screen blending (lighten)
- `Overlay` - Overlay blending
- `HardLight` - Hard light blending
- `SoftLight` - Soft light blending
- `ColorDodge` - Color dodge (brighten)
- `ColorBurn` - Color burn (darken)
- `LinearDodge` - Linear dodge (same as Add)
- `LinearBurn` - Linear burn
- `Darken` - Keep darker color
- `Lighten` - Keep lighter color
- `Difference` - Absolute difference
- `Exclusion` - Exclusion blending
- `PremultipliedAlpha` - Premultiplied alpha blending

### Using Custom Blend Functions

```cpp
// Define a custom blend function
math::Vector4u8 CustomBlend(const math::Vector4u8 &src, 
                            const math::Vector4u8 &dst, 
                            float alpha)
{
    math::Vector4u8 result;
    result.r = static_cast<uint8>((src.r + dst.r) / 2);
    result.g = static_cast<uint8>((src.g + dst.g) / 2);
    result.b = static_cast<uint8>((src.b + dst.b) / 2);
    result.a = static_cast<uint8>((src.a + dst.a) / 2);
    return result;
}

// Use custom blend function
BlendColorRGBA8 blend;
blend.SetBlendFunc(CustomBlend);
```

### Working with Different Color Types

```cpp
// RGB8 (24-bit RGB)
BlendColorRGB8 blend_rgb8;
blend_rgb8.SetBlendMode(AlphaBlendMode::Multiply);

math::Vector3u8 src_rgb(255, 128, 64);
math::Vector3u8 dst_rgb(128, 64, 32);
math::Vector3u8 result_rgb = blend_rgb8(src_rgb, dst_rgb, 1.0f);

// RGB32F (floating-point RGB)
BlendColorRGB32F blend_rgb32f;
blend_rgb32f.SetBlendMode(AlphaBlendMode::Screen);

math::Vector3f src_f(1.0f, 0.5f, 0.25f);
math::Vector3f dst_f(0.5f, 0.25f, 0.125f);
math::Vector3f result_f = blend_rgb32f(src_f, dst_f, 0.5f);

// RGBA32F (floating-point RGBA)
BlendColorRGBA32F blend_rgba32f;
blend_rgba32f.SetBlendMode(AlphaBlendMode::Overlay);
```

## Bitmap Blending

### Using BlendBitmap for Full Image Blending

```cpp
#include <hgl/2d/Blend.h>
#include <hgl/2d/Bitmap.h>

using namespace hgl;
using namespace hgl::bitmap;

// Create bitmaps
BitmapRGBA8 src_bitmap;
src_bitmap.Create(800, 600);

BitmapRGB8 dst_bitmap;
dst_bitmap.Create(800, 600);

// Perform bitmap blending
BlendBitmapRGBA8toRGB8 bitmap_blender;
bitmap_blender(&src_bitmap, &dst_bitmap, 0.75f);  // Blend with 75% opacity
```

## Type Conversion

The system automatically handles conversions between uint8 [0,255] and float [0,1] color spaces:

- **uint8 types** (Vector3u8, Vector4u8): Colors in range [0, 255]
- **float types** (Vector3f, Vector4f): Colors in range [0.0, 1.0]

When using uint8 blend modes, the values are:
1. Converted from uint8 to float
2. Blended in float space using CMMath functions
3. Converted back to uint8 with clamping

## Performance Considerations

- **Function Pointers**: The new implementation uses function pointers instead of virtual functions for better performance
- **Static Wrappers**: Type conversion wrappers are instantiated as template functions at compile time
- **Zero Overhead**: For float types, there's no wrapper overhead - CMMath functions are called directly

## Backward Compatibility

The legacy `BlendColorU32Additive` class is still available for backward compatibility:

```cpp
BlendColorU32Additive additive_blend;
uint32 src = 0xFF8040FF;
uint32 dst = 0x80402010;
uint32 result = additive_blend(src, dst, 0.5f);
```

## Migration from Old API

If you were using virtual function overrides:

**Old Code:**
```cpp
struct MyBlend : public BlendColor<Vector4u8> {
    const Vector4u8 operator()(const Vector4u8 &src, const Vector4u8 &dst) const override {
        // Custom blend logic
    }
};
```

**New Code:**
```cpp
// Option 1: Use predefined blend mode
BlendColorRGBA8 blend;
blend.SetBlendMode(AlphaBlendMode::Add);  // Or any other mode

// Option 2: Use custom function
Vector4u8 MyBlendFunc(const Vector4u8 &src, const Vector4u8 &dst, float alpha) {
    // Custom blend logic
}

BlendColorRGBA8 blend;
blend.SetBlendFunc(MyBlendFunc);
```
