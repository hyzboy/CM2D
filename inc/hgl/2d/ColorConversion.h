#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/math/Vector.h>
#include<hgl/math/Color.h>
#include<hgl/math/Clamp.h>
#include<hgl/color/Color3f.h>
#include<hgl/color/Color4f.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<hgl/color/YCbCr.h>
#include<hgl/color/YCoCg.h>
#include<cmath>
#include<algorithm>

/**
 * 颜色转换模块
 *
 * 提供用于位图的各种颜色空间和格式转换。
 * 支持 RGB/RGBA/灰度转换以及 sRGB/线性色彩空间转换。
 *
 * 示例用法：
 * ```cpp
 * // RGB 转 RGBA，使用不透明的 alpha 值
 * BitmapRGB8 rgb_image;
 * auto rgba_image = hgl::bitmap::color::ConvertRGBToRGBA(rgb_image, 255);
 *
 * // 使用 ITU-R BT.601 标准将 RGB 转换为灰度
 * auto grey_image = hgl::bitmap::color::ConvertRGBToGrey(rgb_image);
 *
 * // 提取红色通道
 * auto red_channel = hgl::bitmap::color::ExtractChannel(rgb_image, 0);
 *
 * // sRGB 转线性色彩空间
 * auto linear = hgl::bitmap::color::ConvertSRGB8ToLinearF(rgb_image);
 * ```
 */

namespace hgl::bitmap::color
{
    // ===================== RGB ↔ RGBA 转换 =====================
    BitmapRGBA8 ConvertRGBToRGBA(const BitmapRGB8& source, uint8 alpha = 255);
    BitmapRGB8 ConvertRGBAToRGB(const BitmapRGBA8& source);

    // ===================== RGB ↔ 灰度转换 =====================
    BitmapGrey8 ConvertRGBToGrey(const BitmapRGB8& source);
    BitmapGrey8 ConvertRGBAToGrey(const BitmapRGBA8& source);
    BitmapRGB8 ConvertGreyToRGB(const BitmapGrey8& source);
    BitmapRGBA8 ConvertGreyToRGBA(const BitmapGrey8& source, uint8 alpha = 255);

    // ===================== 通道提取 =====================
    template<typename T, uint C>
    Bitmap<uint8, 1> ExtractChannel(const Bitmap<T, C>& source, uint channel_index);

    // ===================== sRGB ↔ 线性色彩空间 =====================
    BitmapRGB32F ConvertSRGB8ToLinearF(const BitmapRGB8& source);
    BitmapRGBA32F ConvertSRGBA8ToLinearF(const BitmapRGBA8& source);
    BitmapRGB8 ConvertLinearFToSRGB8(const BitmapRGB32F& source);
    BitmapRGBA8 ConvertLinearFToSRGBA8(const BitmapRGBA32F& source);

    // ===================== RGB ↔ YCbCr =====================
    BitmapYCbCr8 ConvertRGBToYCbCr(const BitmapRGB8& source);
    BitmapRGB8 ConvertYCbCrToRGB(const BitmapYCbCr8& source);
    // Float 扩展
    BitmapYCbCrF ConvertRGB32FToYCbCrF(const BitmapRGB32F& source);
    BitmapRGB32F ConvertYCbCrFToRGB32F(const BitmapYCbCrF& source);

    // ===================== RGB ↔ YCoCg =====================
    BitmapYCoCg8 ConvertRGBToYCoCg(const BitmapRGB8& source);
    BitmapRGB8 ConvertYCoCgToRGB(const BitmapYCoCg8& source);
    // Float 扩展
    BitmapYCoCgF ConvertRGB32FToYCoCgF(const BitmapRGB32F& source);
    BitmapRGB32F ConvertYCoCgFToRGB32F(const BitmapYCoCgF& source);

    // ===================== RGB ↔ HSV =====================
    BitmapHSV8 ConvertRGBToHSV(const BitmapRGB8& source);
    BitmapRGB8 ConvertHSVToRGB(const BitmapHSV8& source);
    BitmapHSVA8 ConvertRGBA8ToHSVA8(const BitmapRGBA8& source);
    BitmapRGBA8 ConvertHSVA8ToRGBA8(const BitmapHSVA8& source);

    // ===================== RGB ↔ HSL =====================
    BitmapHSL8 ConvertRGBToHSL(const BitmapRGB8& source);
    BitmapRGB8 ConvertHSLToRGB(const BitmapHSL8& source);
    BitmapHSLA8 ConvertRGBA8ToHSLA8(const BitmapRGBA8& source);
    BitmapRGBA8 ConvertHSLA8ToRGBA8(const BitmapHSLA8& source);

    // ===================== RGB ↔ XYZ =====================
    BitmapXYZ8 ConvertRGBToXYZ(const BitmapRGB8& source);
    BitmapRGB8 ConvertXYZToRGB(const BitmapXYZ8& source);
    BitmapXYZA8 ConvertRGBA8ToXYZA8(const BitmapRGBA8& source);
    BitmapRGBA8 ConvertXYZA8ToRGBA8(const BitmapXYZA8& source);

    // ===================== Float 扩展（RGB32F ↔ HSVF/HSLF/XYZF） =====================
    BitmapHSVF  ConvertRGB32FToHSVF(const BitmapRGB32F& source);
    BitmapRGB32F ConvertHSVFToRGB32F(const BitmapHSVF& source);
    BitmapHSVAF ConvertRGBA32FToHSVAF(const BitmapRGBA32F& source);
    BitmapRGBA32F ConvertHSVAFToRGBA32F(const BitmapHSVAF& source);
    BitmapHSLF  ConvertRGB32FToHSLF(const BitmapRGB32F& source);
    BitmapRGB32F ConvertHSLFToRGB32F(const BitmapHSLF& source);
    BitmapHSLAf ConvertRGBA32FToHSLAf(const BitmapRGBA32F& source);
    BitmapRGBA32F ConvertHSLAfToRGBA32F(const BitmapHSLAf& source);
    BitmapXYZF  ConvertRGB32FToXYZF(const BitmapRGB32F& source);
    BitmapRGB32F ConvertXYZFToRGB32F(const BitmapXYZF& source);
    BitmapXYZAF ConvertRGBA32FToXYZAF(const BitmapRGBA32F& source);
    BitmapRGBA32F ConvertXYZAFToRGBA32F(const BitmapXYZAF& source);

    // ===================== RGB ↔ OKLab =====================
    BitmapOKLab8  ConvertRGBToOKLab(const BitmapRGB8& source);
    BitmapRGB8    ConvertOKLabToRGB(const BitmapOKLab8& source);
    BitmapOKLaba8 ConvertRGBA8ToOKLaba8(const BitmapRGBA8& source);
    BitmapRGBA8   ConvertOKLaba8ToRGBA8(const BitmapOKLaba8& source);

    // Float 扩展（RGB32F ↔ OKLabF/OKLabAf）
    BitmapOKLabF  ConvertRGB32FToOKLabF(const BitmapRGB32F& source);
    BitmapRGB32F  ConvertOKLabFToRGB32F(const BitmapOKLabF& source);
    BitmapOKLabAf ConvertRGBA32FToOKLabAf(const BitmapRGBA32F& source);
    BitmapRGBA32F ConvertOKLabAfToRGBA32F(const BitmapOKLabAf& source);
}
