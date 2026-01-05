# ImageMagick集成 - 快速参考

## 一分钟上手

### 安装与编译
```bash
# Ubuntu/Debian
sudo apt-get install libmagick++-dev

# macOS
brew install imagemagick

# 编译CM2D
cmake -DCM2D_USE_IMAGEMAGICK=ON ..
make
```

### 基本使用
```cpp
#include <hgl/2d/BitmapImageMagick.h>
using namespace hgl::bitmap;

// 加载
auto *bmp = LoadBitmapRGBA8FromImageMagick("image.png");

// 保存
SaveBitmapToImageMagick("output.jpg", bmp, "JPEG");

// 清理
delete bmp;
```

## API速查

### 加载函数

```cpp
// 通用加载（模板）
template<typename T>
T* LoadBitmapFromImageMagick(const OSString &filename);

// 便捷函数
BitmapGrey8*  LoadBitmapGrey8FromImageMagick(filename);
BitmapRGB8*   LoadBitmapRGB8FromImageMagick(filename);
BitmapRGBA8*  LoadBitmapRGBA8FromImageMagick(filename);
```

### 保存函数

```cpp
// 通用保存（模板）
template<typename T>
bool SaveBitmapToImageMagick(const OSString &filename,
                             const T *bmp,
                             const std::string &format = "");

// 示例
SaveBitmapToImageMagick("out.png", bmp);        // 从扩展名推断
SaveBitmapToImageMagick("out.dat", bmp, "PNG"); // 明确指定格式
```

### 工具函数

```cpp
// 获取支持的格式列表
std::string formats = imagemagick::GetSupportedFormats();

// 获取版本
std::string version = imagemagick::GetVersion();

// 检查格式支持
bool supported = imagemagick::IsSupportedFormat("PNG");
```

## 常用格式

### 无损格式
| 格式 | 扩展名 | 适用场景 | Alpha支持 |
|-----|--------|---------|----------|
| PNG | .png | 网络、通用 | ✅ |
| BMP | .bmp | Windows | ✅ |
| TGA | .tga | 游戏纹理 | ✅ |
| TIFF | .tif/.tiff | 印刷、专业 | ✅ |

### 有损格式
| 格式 | 扩展名 | 适用场景 | Alpha支持 |
|-----|--------|---------|----------|
| JPEG | .jpg/.jpeg | 照片 | ❌ |
| WebP | .webp | 网络 | ✅ |

### 高级格式
| 格式 | 扩展名 | 适用场景 | 特点 |
|-----|--------|---------|------|
| EXR | .exr | HDR | 浮点，高动态范围 |
| HDR | .hdr | HDR | Radiance格式 |

## 常见模式

### 模式1：简单加载保存
```cpp
auto *img = LoadBitmapRGBA8FromImageMagick("input.png");
if (img) {
    // 处理...
    SaveBitmapToImageMagick("output.png", img);
    delete img;
}
```

### 模式2：格式转换
```cpp
// PNG -> JPEG
auto *img = LoadBitmapRGBA8FromImageMagick("input.png");
if (img) {
    // JPEG不支持alpha，转换为RGB8
    BitmapRGB8 *rgb = ConvertToRGB8(img);
    SaveBitmapToImageMagick("output.jpg", rgb, "JPEG");
    delete rgb;
    delete img;
}
```

### 模式3：批量处理
```cpp
for (auto &file : files) {
    auto *img = LoadBitmapRGBA8FromImageMagick(file);
    if (img) {
        ProcessImage(img);
        SaveBitmapToImageMagick(GetOutputName(file), img);
        delete img;
    }
}
```

### 模式4：混合使用TGA和IM
```cpp
BitmapRGBA8* SmartLoad(const std::string &filename) {
    if (EndsWith(filename, ".tga"))
        return LoadBitmapRGBA8FromTGA(filename);  // 快
    else
        return LoadBitmapRGBA8FromImageMagick(filename);  // 灵活
}
```

## 错误处理

### 检查加载失败
```cpp
auto *bmp = LoadBitmapRGBA8FromImageMagick("image.png");
if (!bmp) {
    std::cerr << "Failed to load image" << std::endl;
    return;
}
// 使用bmp...
delete bmp;
```

### 检查保存失败
```cpp
bool ok = SaveBitmapToImageMagick("output.png", bmp);
if (!ok) {
    std::cerr << "Failed to save image" << std::endl;
}
```

### RAII风格（推荐）
```cpp
std::unique_ptr<BitmapRGBA8> bmp(
    LoadBitmapRGBA8FromImageMagick("image.png")
);

if (bmp) {
    ProcessImage(bmp.get());
    SaveBitmapToImageMagick("output.png", bmp.get());
}
// 自动删除
```

## 性能提示

### ✅ 推荐做法
```cpp
// 1. 高频操作用TGA
for (int i = 0; i < 1000; i++) {
    auto *bmp = LoadBitmapFromTGA("texture.tga");  // 快
    Render(bmp);
}

// 2. 低频操作用ImageMagick
auto *asset = LoadBitmapFromImageMagick("user_image.jpg");  // 灵活

// 3. 预处理资源
// 开发时：使用PNG/JPEG等方便编辑
// 发布时：转换为TGA以提升加载速度
```

### ❌ 避免做法
```cpp
// 不要在循环中频繁使用ImageMagick加载
for (int i = 0; i < 1000; i++) {
    auto *bmp = LoadBitmapFromImageMagick("frame.png");  // 慢
}
```

## 像素格式对照

| Bitmap类型 | 通道 | 位数 | ImageMagick | 用途 |
|-----------|-----|------|-------------|------|
| BitmapGrey8 | 1 | 8 | I | 灰度图 |
| BitmapRGB8 | 3 | 8 | RGB | 彩色图 |
| BitmapRGBA8 | 4 | 8 | RGBA | 带透明度 |
| BitmapU16 | 1 | 16 | Short | 高精度灰度 |
| Bitmap32F | 1 | 32 | Float | HDR/浮点 |

## 常见问题

### Q: 编译时找不到ImageMagick
```bash
# 安装开发包
sudo apt-get install libmagick++-dev

# 验证安装
pkg-config --modversion Magick++
```

### Q: JPEG保存失败
```cpp
// JPEG不支持alpha通道
// 错误：
SaveBitmapToImageMagick("out.jpg", rgba_bmp);  // ❌

// 正确：先转换为RGB8
BitmapRGB8 *rgb = ConvertToRGB8(rgba_bmp);
SaveBitmapToImageMagick("out.jpg", rgb);  // ✅
```

### Q: 如何禁用ImageMagick
```bash
# 重新编译，不加ImageMagick选项
cmake ..  # 不添加 -DCM2D_USE_IMAGEMAGICK=ON
make
```

### Q: 运行时库找不到
```bash
# Linux
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# macOS  
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

## CMake配置

### 启用ImageMagick
```cmake
# CMakeLists.txt
option(CM2D_USE_IMAGEMAGICK "Enable ImageMagick" ON)
```

### 命令行
```bash
# 启用
cmake -DCM2D_USE_IMAGEMAGICK=ON ..

# 禁用（默认）
cmake -DCM2D_USE_IMAGEMAGICK=OFF ..
# 或简单地
cmake ..
```

### 检查是否启用
```bash
# 查看编译时是否定义了宏
grep HGL_IMAGEMAGICK_SUPPORT src/Bitmap/*.cpp
```

## 示例程序

### 编译示例
```bash
cmake -DCM2D_USE_IMAGEMAGICK=ON -DCM2D_BUILD_EXAMPLES=ON ..
make
```

### 运行示例
```bash
./example/ImageMagickExample
```

### 示例源码
```cpp
// 位置：example/ImageMagickExample.cpp
// 包含：
// - 版本信息查询
// - 格式支持测试
// - 图像加载保存
// - 格式转换演示
```

## 相关文档

| 文档 | 内容 | 适合 |
|-----|------|------|
| [评估报告](ImageMagick_Integration_Assessment.md) | 可行性、架构 | 了解技术细节 |
| [使用指南](ImageMagick_Usage_Guide.md) | 详细教程 | 深入学习 |
| [实施总结](ImageMagick_Implementation_Summary.md) | 完整总结 | 全面了解 |
| **本文档** | 快速参考 | 日常开发 |

## 联系与反馈

- **GitHub**: https://github.com/hyzboy/CM2D
- **Issues**: 报告问题或建议
- **Discussions**: 技术讨论

---

**版本**: 1.0  
**更新**: 2026-01-05  
**提示**: 收藏本页面以便快速查阅！
