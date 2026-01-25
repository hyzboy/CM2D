#pragma once

/**
 * 通道操作模块
 *
 * 提供位图通道的分离与合并的综合功能。
 * 本头文件为方便起见同时包含 ChannelSplit.h 和 ChannelMerge.h。
 *
 * 分离操作（ChannelSplit.h）：
 * - SplitRGBA()：将 RGBA 分离为 4 个独立通道（R、G、B、A）
 * - SplitRGB()：将 RGB 分离为 3 个独立通道（R、G、B）
 * - SplitRG()：将 RG 分离为 2 个独立通道（R、G）
 * - SplitRGBA_To_RGB_A()：将 RGBA 分离为 RGB + A
 * - ExtractChannel()：按索引提取单个通道
 * - ExtractR/G/B/A()：便捷提取指定通道的封装
 *
 * 合并操作（ChannelMerge.h）：
 * - MergeRGBA()：将 4 个通道合并为 RGBA
 * - MergeRGB()：将 3 个通道合并为 RGB
 * - MergeRG()：将 2 个通道合并为 RG
 * - MergeRGB_A_To_RGBA()：将 RGB 位图与 Alpha 通道合并为 RGBA
 * - MergeRG_B_To_RGB()：将 RG 位图与 B 通道合并为 RGB
 *
 * 示例用法：
 * ```cpp
 * #include <hgl/2d/ChannelOps.h>
 *
 * // 将 RGBA 分离为各个通道
 * BitmapRGBA8 source;
 * source.Create(100, 100);
 * auto [r, g, b, a] = hgl::bitmap::channel::SplitRGBA(source);
 *
 * // 处理各个通道...
 *
 * // 合并回 RGBA
 * auto result = hgl::bitmap::channel::MergeRGBA(*r, *g, *b, *a);
 *
 * // 清理内存
 * delete r; delete g; delete b; delete a;
 * delete result;
 *
 * // 或分离为 RGB + A 进行处理
 * auto [rgb, alpha] = hgl::bitmap::channel::SplitRGBA_To_RGB_A(source);
 * // ... 处理 ...
 * auto merged = hgl::bitmap::channel::MergeRGB_A_To_RGBA(*rgb, *alpha);
 * delete rgb; delete alpha; delete merged;
 * ```
 */

#include<hgl/2d/ChannelSplit.h>
#include<hgl/2d/ChannelMerge.h>
