# ImageMagick集成使用指南

本文档介绍如何在CM2D中使用ImageMagick集成功能。

## 目录

1. [快速开始](#快速开始)
2. [编译配置](#编译配置)
3. [基本用法](#基本用法)
4. [高级功能](#高级功能)
5. [常见问题](#常见问题)
6. [性能建议](#性能建议)

## 快速开始

### 安装ImageMagick

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libmagick++-dev
```

#### macOS
```bash
brew install imagemagick
```

#### Windows
下载并安装ImageMagick开发包：
https://imagemagick.org/script/download.php

### 启用ImageMagick支持

```bash
cd CM2D
mkdir build
cd build
cmake -DCM2D_USE_IMAGEMAGICK=ON -DCM2D_BUILD_EXAMPLES=ON ..
make
```

### 运行示例程序

```bash
./example/ImageMagickExample
```

## 编译配置

### CMake选项

| 选项 | 默认值 | 说明 |
|-----|-------|------|
| CM2D_USE_IMAGEMAGICK | OFF | 启用ImageMagick支持 |
| CM2D_BUILD_EXAMPLES | OFF | 编译示例程序 |

### 检查ImageMagick是否正确安装

```bash
# 检查ImageMagick版本
convert --version

# 检查Magick++头文件
pkg-config --cflags Magick++

# 检查Magick++库文件
pkg-config --libs Magick++
```

## 基本用法

### 加载图像

```cpp
#include<hgl/2d/BitmapImageMagick.h>

using namespace hgl::bitmap;

// 方法1：使用通用加载函数
auto *bitmap = LoadBitmapFromImageMagick<BitmapRGBA8>("image.png");
if(bitmap)
{
    // 使用bitmap...
    delete bitmap;
}

// 方法2：使用便捷函数
auto *rgb_bitmap = LoadBitmapRGB8FromImageMagick("photo.jpg");
auto *rgba_bitmap = LoadBitmapRGBA8FromImageMagick("texture.png");
auto *grey_bitmap = LoadBitmapGrey8FromImageMagick("grayscale.bmp");
```

### 保存图像

```cpp
#include<hgl/2d/BitmapImageMagick.h>

using namespace hgl::bitmap;

// 创建或加载bitmap
BitmapRGBA8 *bitmap = new BitmapRGBA8();
bitmap->Create(512, 512);

// 保存为PNG（自动从扩展名推断格式）
SaveBitmapToImageMagick("output.png", bitmap);

// 保存为JPEG（显式指定格式）
SaveBitmapToImageMagick("output.jpg", bitmap, "JPEG");

// 保存为BMP
SaveBitmapToImageMagick("output.bmp", bitmap, "BMP");

delete bitmap;
```

### 支持的图像格式

通过ImageMagick，CM2D可以支持200+种图像格式，包括但不限于：

**常用格式：**
- PNG - 便携式网络图形
- JPEG/JPG - 联合图像专家组
- BMP - Windows位图
- GIF - 图形交换格式
- TIFF - 标签图像文件格式
- WebP - 网络图像格式

**高级格式：**
- EXR - OpenEXR（HDR）
- HDR - Radiance HDR
- TGA - Targa（CM2D原生也支持）
- ICO - Windows图标
- SVG - 可缩放矢量图形（栅格化）

**检查格式支持：**

```cpp
#include<hgl/2d/BitmapImageMagick.h>

using namespace hgl::bitmap;

// 获取所有支持的格式
std::string formats = imagemagick::GetSupportedFormats();
std::cout << "Supported formats: " << formats << std::endl;

// 检查特定格式是否支持
if(imagemagick::IsSupportedFormat("PNG"))
{
    std::cout << "PNG is supported" << std::endl;
}
```

## 高级功能

### 格式转换

```cpp
// 加载PNG并保存为JPEG
auto *bitmap = LoadBitmapRGBA8FromImageMagick("input.png");
if(bitmap)
{
    // 注意：JPEG不支持透明通道，需要转换为RGB8
    BitmapRGB8 *rgb_bitmap = new BitmapRGB8();
    rgb_bitmap->Create(bitmap->GetWidth(), bitmap->GetHeight());
    
    // 转换RGBA到RGB（简单版本）
    auto *src = bitmap->GetData();
    auto *dst = rgb_bitmap->GetData();
    int total = bitmap->GetWidth() * bitmap->GetHeight();
    
    for(int i = 0; i < total; i++)
    {
        dst[i].r = src[i].r;
        dst[i].g = src[i].g;
        dst[i].b = src[i].b;
    }
    
    SaveBitmapToImageMagick("output.jpg", rgb_bitmap, "JPEG");
    
    delete rgb_bitmap;
    delete bitmap;
}
```

### 批量处理

```cpp
#include<filesystem>
#include<hgl/2d/BitmapImageMagick.h>

namespace fs = std::filesystem;
using namespace hgl::bitmap;

void ConvertAllPNGToJPEG(const std::string &directory)
{
    for(const auto &entry : fs::directory_iterator(directory))
    {
        if(entry.path().extension() == ".png")
        {
            auto *bitmap = LoadBitmapRGBA8FromImageMagick(entry.path().string());
            if(bitmap)
            {
                // 转换为RGB（JPEG不支持alpha）
                BitmapRGB8 *rgb = ConvertToRGB8(bitmap);
                
                // 更改扩展名
                auto output_path = entry.path();
                output_path.replace_extension(".jpg");
                
                SaveBitmapToImageMagick(output_path.string(), rgb, "JPEG");
                
                delete rgb;
                delete bitmap;
            }
        }
    }
}
```

### 与现有TGA功能配合使用

```cpp
#include<hgl/2d/BitmapLoad.h>     // TGA加载
#include<hgl/2d/BitmapSave.h>     // TGA保存
#include<hgl/2d/BitmapImageMagick.h>  // ImageMagick

using namespace hgl::bitmap;

// 策略1：加载时自动选择
BitmapRGBA8* LoadBitmapAuto(const OSString &filename)
{
    // 获取文件扩展名
    std::string ext = GetFileExtension(filename);
    
    if(ext == ".tga")
    {
        // 使用快速的TGA加载器
        return LoadBitmapRGBA8FromTGA(filename);
    }
    else
    {
        // 使用ImageMagick加载其他格式
        return LoadBitmapRGBA8FromImageMagick(filename);
    }
}

// 策略2：保存时优化格式选择
bool SaveBitmapOptimal(const OSString &filename, BitmapRGBA8 *bitmap)
{
    std::string ext = GetFileExtension(filename);
    
    if(ext == ".tga")
    {
        // TGA保存速度更快
        return SaveBitmapToTGA(filename, bitmap);
    }
    else
    {
        // 其他格式使用ImageMagick
        return SaveBitmapToImageMagick(filename, bitmap);
    }
}
```

## 常见问题

### Q1: 编译时找不到ImageMagick

**A:** 确保已安装ImageMagick开发包：

```bash
# Ubuntu/Debian
sudo apt-get install libmagick++-dev

# 检查安装
pkg-config --modversion Magick++
```

如果pkg-config找不到，可能需要设置PKG_CONFIG_PATH：

```bash
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```

### Q2: 运行时报错"undefined symbol"

**A:** 确保运行时库路径正确：

```bash
# Linux
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# macOS
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

### Q3: 加载图像失败但文件存在

**A:** 检查几个方面：
1. 文件格式是否真的受支持
2. 文件是否损坏
3. 权限是否正确
4. 使用绝对路径而不是相对路径

```cpp
// 添加错误检查
auto *bitmap = LoadBitmapRGBA8FromImageMagick("test.png");
if(!bitmap)
{
    std::cerr << "Failed to load image" << std::endl;
    // 检查文件是否存在
    // 检查格式是否支持
}
```

### Q4: JPEG保存失败或出现奇怪的颜色

**A:** JPEG不支持alpha通道，需要先转换为RGB8：

```cpp
// 错误：RGBA8不能直接保存为JPEG
SaveBitmapToImageMagick("output.jpg", rgba_bitmap);  // ❌

// 正确：先转换为RGB8
BitmapRGB8 *rgb = ConvertToRGB8(rgba_bitmap);
SaveBitmapToImageMagick("output.jpg", rgb);  // ✅
```

### Q5: 如何禁用ImageMagick支持

**A:** 重新编译时不设置CM2D_USE_IMAGEMAGICK选项：

```bash
cmake ..  # 不添加 -DCM2D_USE_IMAGEMAGICK=ON
make
```

ImageMagick是可选依赖，不会影响核心功能。

## 性能建议

### 1. 格式选择

| 场景 | 推荐格式 | 原因 |
|-----|---------|------|
| 运行时加载 | TGA | 最快，无压缩开销 |
| 资源交换 | PNG | 无损压缩，广泛支持 |
| 照片 | JPEG | 高压缩比 |
| HDR/浮点 | EXR | 专业格式 |

### 2. 加载优化

```cpp
// 高频操作：使用TGA
for(int i = 0; i < 1000; i++)
{
    auto *bmp = LoadBitmapFromTGA("texture.tga");  // 快
    ProcessBitmap(bmp);
}

// 低频操作：使用ImageMagick
auto *asset = LoadBitmapFromImageMagick("asset.png");  // 慢但灵活
```

### 3. 批量转换优化

```cpp
// 将外部资源预转换为TGA以提升运行时性能
void PreprocessAssets()
{
    for(auto &png_file : asset_files)
    {
        auto *bmp = LoadBitmapFromImageMagick(png_file);
        std::string tga_file = ReplaceExtension(png_file, ".tga");
        SaveBitmapToTGA(tga_file, bmp);
        delete bmp;
    }
}

// 运行时只加载TGA
void RuntimeLoad()
{
    auto *bmp = LoadBitmapFromTGA("preprocessed.tga");  // 快速加载
}
```

### 4. 内存管理

```cpp
// 及时释放不再使用的bitmap
{
    auto *temp = LoadBitmapFromImageMagick("large_image.png");
    ProcessImage(temp);
    delete temp;  // 立即释放
}

// 避免不必要的复制
auto *original = LoadBitmapFromImageMagick("image.png");
// 直接使用original，避免创建副本
ProcessInPlace(original);
```

## 最佳实践

### 1. 错误处理

```cpp
auto *bitmap = LoadBitmapFromImageMagick("image.png");
if(!bitmap)
{
    // 处理加载失败
    std::cerr << "Failed to load image.png" << std::endl;
    return;
}

// 使用bitmap...

delete bitmap;
```

### 2. RAII模式

```cpp
#include<memory>

std::unique_ptr<BitmapRGBA8> LoadImageSafe(const std::string &filename)
{
    auto *raw = LoadBitmapRGBA8FromImageMagick(filename);
    return std::unique_ptr<BitmapRGBA8>(raw);
}

// 使用
auto bitmap = LoadImageSafe("image.png");
if(bitmap)
{
    // 自动管理内存
    ProcessBitmap(bitmap.get());
}
// 自动释放
```

### 3. 格式无关的接口

```cpp
class ImageLoader
{
public:
    static BitmapRGBA8* Load(const std::string &filename)
    {
        std::string ext = GetExtension(filename);
        
        // 根据扩展名选择加载器
        if(ext == ".tga")
            return LoadBitmapRGBA8FromTGA(filename);
        else
            return LoadBitmapRGBA8FromImageMagick(filename);
    }
};
```

## 参考资料

- [ImageMagick官方文档](https://imagemagick.org/)
- [Magick++ API参考](https://imagemagick.org/Magick++/)
- [CM2D Bitmap类文档](../inc/hgl/2d/Bitmap.h)
- [可行性评估文档](ImageMagick_Integration_Assessment.md)

## 更新日志

- **2026-01-05**: 初始版本，添加基础功能和文档
