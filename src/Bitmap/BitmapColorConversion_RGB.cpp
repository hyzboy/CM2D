// BitmapColorConversion_RGB.cpp
// 实现RGB相关的颜色空间转换函数
#include "hgl/2d/ColorConversion.h"
#include <hgl/color/sRGBConvert.h>
#include "hgl/math/Clamp.h"
#include "hgl/math/Color.h"
#include "hgl/math/Vector.h"
#include "hgl/color/Color3ub.h"
#include "hgl/color/Color4ub.h"
#include "hgl/color/Color3f.h"
#include "hgl/color/Color4f.h"
#include <cmath>
#include <algorithm>

namespace hgl::bitmap::color
{
    BitmapRGBA8 ConvertRGBToRGBA(const BitmapRGB8& source, uint8 alpha)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGBA8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color3ub* src = reinterpret_cast<const Color3ub*>(source.GetData());
        Color4ub* dst = reinterpret_cast<Color4ub*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i].r;
            dst[i].g = src[i].g;
            dst[i].b = src[i].b;
            dst[i].a = alpha;
        }
        return result;
    }

    BitmapRGB8 ConvertRGBAToRGB(const BitmapRGBA8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGB8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color4ub* src = reinterpret_cast<const Color4ub*>(source.GetData());
        Color3ub* dst = reinterpret_cast<Color3ub*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i].r;
            dst[i].g = src[i].g;
            dst[i].b = src[i].b;
        }
        return result;
    }

    BitmapGrey8 ConvertRGBToGrey(const BitmapRGB8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapGrey8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color3ub* src = reinterpret_cast<const Color3ub*>(source.GetData());
        uint8* dst = result.GetData();
        for (int i = 0; i < width * height; ++i)
        {
            float grey = 0.299f * src[i].r + 0.587f * src[i].g + 0.114f * src[i].b;
            dst[i] = hgl::math::ClampU8(grey);
        }
        return result;
    }

    BitmapGrey8 ConvertRGBAToGrey(const BitmapRGBA8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapGrey8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color4ub* src = reinterpret_cast<const Color4ub*>(source.GetData());
        uint8* dst = result.GetData();
        for (int i = 0; i < width * height; ++i)
        {
            float grey = 0.299f * src[i].r + 0.587f * src[i].g + 0.114f * src[i].b;
            dst[i] = hgl::math::ClampU8(grey);
        }
        return result;
    }

    BitmapRGB8 ConvertGreyToRGB(const BitmapGrey8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGB8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const uint8* src = source.GetData();
        Color3ub* dst = reinterpret_cast<Color3ub*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i];
            dst[i].g = src[i];
            dst[i].b = src[i];
        }
        return result;
    }

    BitmapRGBA8 ConvertGreyToRGBA(const BitmapGrey8& source, uint8 alpha)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGBA8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const uint8* src = source.GetData();
        Color4ub* dst = reinterpret_cast<Color4ub*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = src[i];
            dst[i].g = src[i];
            dst[i].b = src[i];
            dst[i].a = alpha;
        }
        return result;
    }

    BitmapRGB32F ConvertSRGB8ToLinearF(const BitmapRGB8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGB32F result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color3ub* src = reinterpret_cast<const Color3ub*>(source.GetData());
        Color3f* dst = reinterpret_cast<Color3f*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].x = sRGB2Linear(src[i].r);
            dst[i].y = sRGB2Linear(src[i].g);
            dst[i].z = sRGB2Linear(src[i].b);
        }
        return result;
    }

    BitmapRGBA32F ConvertSRGBA8ToLinearF(const BitmapRGBA8& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGBA32F result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color4ub* src = reinterpret_cast<const Color4ub*>(source.GetData());
        Color4f* dst = reinterpret_cast<Color4f*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].x = sRGB2Linear(src[i].r);
            dst[i].y = sRGB2Linear(src[i].g);
            dst[i].z = sRGB2Linear(src[i].b);
            dst[i].w = src[i].a / 255.0f;
        }
        return result;
    }

    BitmapRGB8 ConvertLinearFToSRGB8(const BitmapRGB32F& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGB8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color3f* src = reinterpret_cast<const Color3f*>(source.GetData());
        Color3ub* dst = reinterpret_cast<Color3ub*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = Linear2sRGB(src[i].x);
            dst[i].g = Linear2sRGB(src[i].y);
            dst[i].b = Linear2sRGB(src[i].z);
        }
        return result;
    }

    BitmapRGBA8 ConvertLinearFToSRGBA8(const BitmapRGBA32F& source)
    {
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        BitmapRGBA8 result;
        if (width == 0 || height == 0 || !source.GetData())
            return result;
        result.Create(width, height);
        const Color4f* src = reinterpret_cast<const Color4f*>(source.GetData());
        Color4ub* dst = reinterpret_cast<Color4ub*>(result.GetData());
        for (int i = 0; i < width * height; ++i)
        {
            dst[i].r = Linear2sRGB(src[i].x);
            dst[i].g = Linear2sRGB(src[i].y);
            dst[i].b = Linear2sRGB(src[i].z);
            dst[i].a = hgl::math::ClampU8(src[i].w * 255.0f);
        }
        return result;
    }
}
