# CM2D Documentation

## BlendColor System

The BlendColor system provides flexible alpha blending for bitmap operations in CM2D.

### Documentation Files

- **[BlendColor_Usage.md](BlendColor_Usage.md)** - Usage examples and API reference
  - Basic usage examples
  - All supported blend modes
  - Custom blend functions
  - Working with different color types
  - Bitmap blending
  - Migration guide

- **[BlendColor_Implementation.md](BlendColor_Implementation.md)** - Technical implementation details
  - Architecture and design decisions
  - Template specializations
  - Type conversion system
  - Performance characteristics
  - Thread safety
  - Dependencies

### Quick Start

```cpp
#include <hgl/2d/Blend.h>

using namespace hgl;
using namespace hgl::bitmap;

// Create a blend color instance
BlendColorRGBA8 blend;

// Set blend mode
blend.SetBlendMode(AlphaBlendMode::Add);

// Perform blending
math::Vector4u8 src(255, 128, 64, 255);
math::Vector4u8 dst(128, 64, 32, 255);
math::Vector4u8 result = blend(src, dst, 0.5f);
```

### Key Features

- ✅ 17 blend modes from CMMath (Normal, Add, Multiply, Screen, etc.)
- ✅ Custom blend function support
- ✅ Automatic uint8 ↔ float type conversion
- ✅ Function pointer-based (better performance than virtual functions)
- ✅ Thread-safe
- ✅ Type-safe templates
- ✅ Backward compatible

### Supported Blend Modes

Normal • Add • Subtract • Multiply • Screen • Overlay • HardLight • SoftLight  
ColorDodge • ColorBurn • LinearDodge • LinearBurn • Darken • Lighten  
Difference • Exclusion • PremultipliedAlpha

### Type Aliases

```cpp
using BlendColorRGB8 = BlendColor<math::Vector3u8>;      // 24-bit RGB
using BlendColorRGBA8 = BlendColor<math::Vector4u8>;     // 32-bit RGBA
using BlendColorRGB32F = BlendColor<math::Vector3f>;     // Float RGB
using BlendColorRGBA32F = BlendColor<math::Vector4f>;    // Float RGBA
```

### Dependencies

- **CMCoreType**: Provides AlphaBlendMode enum and basic types
- **CMMath**: Provides blend functions and Vector types

### Further Reading

For more detailed information, see:
- [Usage Examples](BlendColor_Usage.md)
- [Implementation Details](BlendColor_Implementation.md)
