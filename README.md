# CM2D

传统像素级（软件渲染）2D 图形库，ULRE 引擎的 CM 系列子模块之一。提供位图容器、像素级图元绘制、图像处理、颜色空间转换、TGA 编解码与程序化地形生成，不依赖 GPU。

## 特性

- **位图容器**
  - `Bitmap<T,C>`：交错存储模板位图（灰度/RG/RGB/RGBA × 8bit/16bit/32bit float），完整值语义（深拷贝 + 移动）
  - `PlanarBitmap<T,C>`：平面存储位图（每通道独立缓冲区），支持与 `Bitmap` 互转
  - 平台位图：Windows DIB / macOS CoreGraphics / X11 / Wayland，统一 `BitmapPlatform` 别名
- **像素级绘制**（`DrawGeometry`）
  - 点/水平线/垂直线/矩形/线段（Bresenham）/线框圆（中点圆）/实心圆/扇形
  - 圆弧/二次贝塞尔/三次贝塞尔/Catmull-Rom 样条
  - 单色位图绘制，全部支持裁剪与混合
- **混合系统**（`Blend`）
  - 29 种混合模式（Normal/Add/Screen/Overlay/HardLight/SoftLight/ColorDodge/Difference/Hue/Saturation...）
  - 函数指针派发（优于虚函数），uint8 颜色自动经 float 精度混合
- **图像处理**
  - 缩放：最近邻/双线性/双三次/Lanczos2/Lanczos3/Mitchell-Netravali/自适应，支持像素中心对齐
  - 变换：90°/180°/270° 旋转、水平/垂直/双向翻转（原地与非原地）
  - 瓦片：分割/组装/无缝平铺纹理
  - Mipmap 链生成（自定义滤波器/最大层级/最小尺寸）
  - 通道：RGBA/RGB/RG 分离与合并、按索引提取通道
- **颜色转换**（`ColorConversion`）
  - RGB ↔ RGBA / 灰度 / YCbCr / YCoCg / HSV / HSL / XYZ / OKLab，8bit 与 float 双精度
  - sRGB ↔ 线性色彩空间
- **图像格式**
  - TGA 读写（内置，无第三方依赖；`SaveTga` / `LoadBitmapFromTGA`）
  - ImageMagick（可选，`-DCM2D_USE_IMAGEMAGICK=ON`，200+ 格式）
- **程序化地形**（`Terrain`）
  - Perlin / Simplex / Voronoi / FBM 噪声生成器
  - HeightMap（热力/水力侵蚀）、BiomeMap 生物群落、TerrainGenerator 高层生成器

## 目录结构

```
CM2D/
├── inc/hgl/2d/        # 全部公开头文件（模板为主）
├── src/               # 实现（TGA 编解码、颜色转换、平台位图、地形）
├── examples/          # 示例与自检测试
└── docs/              # 技术文档
```

## 依赖

- **CMUtil**（PUBLIC）：通用工具
- **CMCore / CMCoreType / CMMath**（传递）：基础类型、颜色、数学库
- 平台库：Windows `gdi32 msimg32`；macOS CoreGraphics/ApplicationServices；Linux X11（可选）/ Wayland（可选）
- ImageMagick（可选）

## 构建

作为 ULRE 引擎子模块集成（顶层 `use_cm_module(CM2D)`），也可独立构建：

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target CM2D --config Debug
```

构建产物：`build/out/Windows_64_Debug/CM2D.lib`（示例生成 TGA 输出到运行目录）。

## 快速上手

```cpp
#include<hgl/2d/Bitmap.h>
#include<hgl/2d/DrawGeometry.h>
#include<hgl/2d/TGA.h>

using namespace hgl;
using namespace hgl::bitmap;

BitmapRGB8 img;
img.Create(256, 256);
img.ClearColor(Color3ub(255, 255, 255));

DrawGeometryRGB8 dg(&img);
dg.SetDrawColor(Color3ub(255, 0, 0));
dg.DrawBar(32, 32, 64, 48);          // 红色矩形
dg.DrawSolidCircle(160, 160, 48);    // 实心圆
dg.DrawLine(0, 0, 255, 255);         // 对角线

SaveTga("output.tga", &img);
```

## 文档索引

- [BlendColor_Usage.md](docs/BlendColor_Usage.md) — 混合系统使用指南
- [BlendColor_Implementation.md](docs/BlendColor_Implementation.md) — 混合系统实现细节
- [TerrainGeneration.md](docs/TerrainGeneration.md) — 地形生成系统
- [README.md](docs/README.md) — 文档索引

## 示例列表

| 示例 | 内容 |
|---|---|
| `BlendTest` | 混合模式测试（输出 blend_modes_test.tga 等） |
| `TransformResizeTest` | 旋转/翻转/缩放（7 种滤波器） |
| `DrawGeometryTest` | 图元绘制演示 |
| `DrawCurvesTest` | 贝塞尔/Catmull-Rom 曲线 |
| `ChannelOpsTest` | 通道分离/合并/交换（含 roundtrip 断言） |
| `TerrainGenExample` | 地形生成（Perlin/Simplex/Voronoi/FBM/侵蚀/生物群落） |
| `BitmapSemanticsTest` | 位图值语义自检（拷贝/移动/自赋值） |
| `GeometryVerifyTest` | 图元绘制断言测试（DrawBar/圆/线/裁剪） |
| `TileMipMapTest` | 瓦片与 Mipmap 断言测试 |
| `BitmapWindowsTest` | Windows DIB 平台测试（BGR 转换/预乘/生命周期） |
| `DistributionChart2D` | 2D 分布图绘制（graphics 子目录） |
| `Pick2DTest` | 2D 拾取测试（pick 子目录） |
| `ImageMagickExample` | ImageMagick 多格式加载/保存（需开启选项） |

## 许可证

见 ULRE 主仓库。
