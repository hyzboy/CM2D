# ImageMagick集成架构图

本文档提供ImageMagick与CM2D Bitmap集成的可视化架构说明。

## 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│                  (用户应用程序代码)                           │
└────────────┬────────────────────────────────┬───────────────┘
             │                                │
             ├────────────────┐               │
             ▼                ▼               ▼
    ┌────────────────┐  ┌──────────────┐  ┌─────────────────┐
    │  Bitmap Core   │  │  TGA Loader  │  │ ImageMagick     │
    │   Template     │  │   (Native)   │  │   Wrapper       │
    │   <T, C>       │  │              │  │  (Optional)     │
    └────────────────┘  └──────────────┘  └─────────────────┘
             │                  │                   │
             │                  │                   │
             ▼                  ▼                   ▼
    ┌────────────────────────────────────────────────────────┐
    │                 Memory Buffer (T* data)                │
    │          width × height × sizeof(T) bytes              │
    └────────────────────────────────────────────────────────┘
                              │
                              ▼
                    ┌─────────────────┐
                    │   File System   │
                    └─────────────────┘
```

## 数据流向

### 加载流程 (Load)

```
┌──────────────┐     ┌──────────────────┐     ┌─────────────┐
│  Image File  │────▶│  ImageMagick     │────▶│  Bitmap<T>  │
│  (PNG/JPEG)  │     │  Decoder         │     │   Object    │
└──────────────┘     └──────────────────┘     └─────────────┘
                              │
                              │ 1. Read file
                              │ 2. Decode format
                              │ 3. Convert pixels
                              ▼
                     ┌──────────────────┐
                     │  Pixel Buffer    │
                     │  (temp storage)  │
                     └──────────────────┘
                              │
                              │ 4. Copy to Bitmap
                              ▼
                     ┌──────────────────┐
                     │  Bitmap->data    │
                     │  (final storage) │
                     └──────────────────┘
```

### 保存流程 (Save)

```
┌─────────────┐     ┌──────────────────┐     ┌──────────────┐
│  Bitmap<T>  │────▶│  ImageMagick     │────▶│  Image File  │
│   Object    │     │  Encoder         │     │  (PNG/JPEG)  │
└─────────────┘     └──────────────────┘     └──────────────┘
      │                      │
      │                      │ 1. Get pixel data
      │                      │ 2. Convert format
      │                      │ 3. Compress
      │                      │ 4. Write file
      ▼                      ▼
┌──────────────┐    ┌──────────────────┐
│ Bitmap->data │    │  Pixel Buffer    │
│              │    │  (temp storage)  │
└──────────────┘    └──────────────────┘
```

## 类层次结构

```
┌────────────────────────────────────────────────────────────┐
│                    Bitmap<T, C>                            │
│  - Template class for pixel data storage                  │
│  - T: pixel type (Vector3u8, Vector4u8, etc.)             │
│  - C: channel count (1, 3, 4)                             │
├────────────────────────────────────────────────────────────┤
│  + Create(width, height)                                   │
│  + GetData() : T*                                          │
│  + GetWidth() : int                                        │
│  + GetHeight() : int                                       │
│  + GetChannels() : uint                                    │
│  + GetChannelBits() : uint                                 │
└────────────────────────────────────────────────────────────┘
                          ▲
                          │ uses
                          │
        ┌─────────────────┴─────────────────┐
        │                                   │
┌───────┴────────┐              ┌───────────┴──────────┐
│  TGA Loader    │              │ ImageMagick Loader   │
├────────────────┤              ├──────────────────────┤
│ BitmapLoader   │              │ ImageMagickLoader    │
│  (interface)   │              │    (interface)       │
└────────────────┘              └──────────────────────┘
        │                                   │
        ▼                                   ▼
┌────────────────┐              ┌──────────────────────┐
│ LoadFromTGA()  │              │ LoadFromImageMagick()│
│ SaveToTGA()    │              │ SaveToImageMagick()  │
└────────────────┘              └──────────────────────┘
```

## 类型映射关系

```
CM2D Bitmap Types          ImageMagick Types
┌──────────────────┐      ┌──────────────────────┐
│  BitmapGrey8     │◀────▶│  Grayscale (8-bit)   │
│  (uint8, 1)      │      │  StorageType: Char   │
│                  │      │  Map: "I"            │
└──────────────────┘      └──────────────────────┘

┌──────────────────┐      ┌──────────────────────┐
│  BitmapRGB8      │◀────▶│  RGB (24-bit)        │
│  (Vector3u8, 3)  │      │  StorageType: Char   │
│                  │      │  Map: "RGB"          │
└──────────────────┘      └──────────────────────┘

┌──────────────────┐      ┌──────────────────────┐
│  BitmapRGBA8     │◀────▶│  RGBA (32-bit)       │
│  (Vector4u8, 4)  │      │  StorageType: Char   │
│                  │      │  Map: "RGBA"         │
└──────────────────┘      └──────────────────────┘

┌──────────────────┐      ┌──────────────────────┐
│  BitmapU16       │◀────▶│  Grayscale (16-bit)  │
│  (uint16, 1)     │      │  StorageType: Short  │
│                  │      │  Map: "I"            │
└──────────────────┘      └──────────────────────┘

┌──────────────────┐      ┌──────────────────────┐
│  Bitmap32F       │◀────▶│  Float (32-bit)      │
│  (float, 1)      │      │  StorageType: Float  │
│                  │      │  Map: "I"            │
└──────────────────┘      └──────────────────────┘
```

## 内存布局

### CM2D Bitmap内存布局

```
BitmapRGBA8 (256x256 像素)

┌────────────────────────────────────────┐
│ Pixel[0,0]   = [R0, G0, B0, A0]        │  ← Line 0
│ Pixel[1,0]   = [R1, G1, B1, A1]        │
│ ...                                     │
│ Pixel[255,0] = [R255, G255, B255, A255]│
├────────────────────────────────────────┤
│ Pixel[0,1]   = [R0, G0, B0, A0]        │  ← Line 1
│ Pixel[1,1]   = [R1, G1, B1, A1]        │
│ ...                                     │
├────────────────────────────────────────┤
│ ...                                     │
├────────────────────────────────────────┤
│ Pixel[0,255]   = [R0, G0, B0, A0]      │  ← Line 255
│ Pixel[1,255]   = [R1, G1, B1, A1]      │
│ ...                                     │
│ Pixel[255,255] = [R255,G255,B255,A255] │
└────────────────────────────────────────┘

总大小: 256 × 256 × 4 bytes = 262,144 bytes
连续存储，无padding，行优先(row-major)
```

### ImageMagick内存布局

```
Magick::Image (256x256)

类似结构，但可能包含：
- 额外的元数据
- 色彩空间信息
- 压缩信息
- 可能的padding
```

## 编译配置流程

```
┌─────────────────────┐
│  CMakeLists.txt     │
│                     │
│  option(            │
│    CM2D_USE_        │
│    IMAGEMAGICK)     │
└──────────┬──────────┘
           │
           ├─────────── OFF (default) ──────────┐
           │                                    │
           └─────────── ON ─────────┐           │
                                    ▼           ▼
                          ┌──────────────┐  ┌────────────┐
                          │ find_package │  │ Skip IM    │
                          │ (ImageMagick)│  │ Support    │
                          └──────┬───────┘  └────────────┘
                                 │
                    ┌────────────┴────────────┐
                    │                         │
                 Found                    Not Found
                    │                         │
                    ▼                         ▼
         ┌────────────────────┐     ┌─────────────────┐
         │ Add source files:  │     │ Warning message │
         │ BitmapImageMagick  │     │ Skip IM support │
         │                    │     └─────────────────┘
         │ Set definitions:   │
         │ HGL_IMAGEMAGICK_   │
         │ SUPPORT            │
         │                    │
         │ Link libraries:    │
         │ ${ImageMagick_     │
         │  LIBRARIES}        │
         └────────────────────┘
                    │
                    ▼
         ┌────────────────────┐
         │  Build with        │
         │  ImageMagick       │
         │  support enabled   │
         └────────────────────┘
```

## 格式支持对比

```
┌──────────────────────────────────────────────────────────────┐
│                      Format Support                          │
├────────────┬─────────────────────┬────────────────────────────┤
│  Format    │    Native (TGA)     │    ImageMagick             │
├────────────┼─────────────────────┼────────────────────────────┤
│  TGA       │  ✅ Fast            │  ✅ Available              │
│  PNG       │  ❌                 │  ✅ Lossless, Alpha        │
│  JPEG      │  ❌                 │  ✅ Lossy, No Alpha        │
│  BMP       │  ❌                 │  ✅ Windows Standard       │
│  GIF       │  ❌                 │  ✅ Animation Support      │
│  TIFF      │  ❌                 │  ✅ Professional           │
│  WebP      │  ❌                 │  ✅ Modern Web             │
│  EXR       │  ❌                 │  ✅ HDR, Float             │
│  HDR       │  ❌                 │  ✅ Radiance HDR           │
│  ICO       │  ❌                 │  ✅ Windows Icons          │
│  SVG       │  ❌                 │  ✅ Vector (rasterized)    │
│  ...       │  ❌                 │  ✅ 200+ formats           │
└────────────┴─────────────────────┴────────────────────────────┘
```

## 性能特征

```
               Load Speed         File Size       Quality
               (相对)              (相对)          (相对)
TGA:           ████████████       ████████        ████████
               (快，无压缩)        (大)            (无损)

PNG:           ████████           ████            ████████
               (中，需解压)        (小-中)         (无损)

JPEG:          ██████             ██              ██████
               (中，需解码)        (很小)          (有损)

BMP:           ██████████         ██████          ████████
               (快，简单)          (中-大)         (无损)

WebP:          ██████             ██              ███████
               (中，现代)          (很小)          (有损/无损)

EXR:           ████               ██████          ██████████
               (慢，复杂)          (大)            (HDR)
```

## 使用场景推荐

```
┌────────────────────────────────────────────────────────────┐
│                      Use Cases                             │
├──────────────────┬────────────────┬────────────────────────┤
│  Scenario        │  Recommended   │  Reason                │
├──────────────────┼────────────────┼────────────────────────┤
│ Runtime Loading  │  TGA           │  Fastest               │
│ Asset Exchange   │  PNG           │  Universal, Lossless   │
│ Photos           │  JPEG          │  Small size            │
│ Web Export       │  WebP/PNG      │  Web optimized         │
│ HDR/Float        │  EXR           │  Professional          │
│ Development      │  PNG           │  Easy to preview       │
│ Distribution     │  TGA           │  Fast loading          │
│ Archival         │  PNG/TIFF      │  Long-term stable      │
└──────────────────┴────────────────┴────────────────────────┘
```

## 错误处理流程

```
LoadBitmapFromImageMagick(filename)
           │
           ▼
    ┌─────────────┐
    │ Initialize  │
    │ ImageMagick │
    └──────┬──────┘
           │
           ▼
    ┌─────────────┐
    │  Read File  │───────── File Error ──────┐
    └──────┬──────┘                           │
           │                                  │
           ▼                                  │
    ┌─────────────┐                          │
    │ Decode      │───────── Format Error ────┤
    │ Format      │                           │
    └──────┬──────┘                           │
           │                                  │
           ▼                                  │
    ┌─────────────┐                          │
    │ Allocate    │───────── Memory Error ────┤
    │ Bitmap      │                           │
    └──────┬──────┘                           │
           │                                  │
           ▼                                  │
    ┌─────────────┐                          │
    │ Copy Pixels │───────── Conversion ──────┤
    │             │          Error            │
    └──────┬──────┘                           │
           │                                  │
           ▼                                  │
    ┌─────────────┐                          │
    │ Return      │                           │
    │ Bitmap*     │                           │
    └─────────────┘                           │
                                              │
           ┌──────────────────────────────────┘
           ▼
    ┌─────────────┐
    │ OnLoadFailed│
    │ Cleanup     │
    │ Return NULL │
    └─────────────┘
```

## 依赖关系

```
┌────────────────────────────────────────┐
│         CM2D Application               │
└─────────────────┬──────────────────────┘
                  │ depends on
                  ▼
┌────────────────────────────────────────┐
│         CM2D Library                   │
│  ┌──────────────────────────────────┐  │
│  │  Core (always)                   │  │
│  │  - Bitmap<T,C>                   │  │
│  │  - Vector types                  │  │
│  └──────────────────────────────────┘  │
│  ┌──────────────────────────────────┐  │
│  │  TGA (always)                    │  │
│  │  - LoadFromTGA                   │  │
│  │  - SaveToTGA                     │  │
│  └──────────────────────────────────┘  │
│  ┌──────────────────────────────────┐  │
│  │  ImageMagick (optional)          │  │
│  │  - LoadFromImageMagick           │  │
│  │  - SaveToImageMagick             │  │
│  └──────────────────────────────────┘  │
└─────────────────┬──────────────────────┘
                  │ optionally depends on
                  ▼
┌────────────────────────────────────────┐
│    ImageMagick (External Library)      │
│  - Magick++ (C++ API)                  │
│  - libMagickCore                       │
│  - libMagickWand                       │
│  - Various format libraries            │
│    (libpng, libjpeg, etc.)             │
└────────────────────────────────────────┘
```

---

**提示**: 这些图表展示了ImageMagick与CM2D Bitmap集成的各个方面，帮助理解整体架构和数据流。

**版本**: 1.0  
**日期**: 2026-01-05
