// BitmapColorConversion_XYZ.cpp
// 实现XYZ相关的颜色空间转换函数
#include "hgl/2d/ColorConversion.h"
#include <hgl/color/XYZ.h>

namespace hgl::bitmap::color
{
    BitmapXYZ8 ConvertRGBToXYZ(const BitmapRGB8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapXYZ8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3ub* s = reinterpret_cast<const Color3ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 x_, y_, z_;
            hgl::RGB2XYZ(x_, y_, z_, s[i].r, s[i].g, s[i].b);
            d[i * 3 + 0] = x_;
            d[i * 3 + 1] = y_;
            d[i * 3 + 2] = z_;
        }
        return result;
    }

    BitmapRGB8 ConvertXYZToRGB(const BitmapXYZ8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapRGB8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const uint8* s = reinterpret_cast<const uint8*>(src.GetData());
        Color3ub* d = reinterpret_cast<Color3ub*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 r, g, b;
            hgl::XYZ2RGB(r, g, b, s[i * 3 + 0], s[i * 3 + 1], s[i * 3 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
        }
        return result;
    }

    BitmapXYZA8 ConvertRGBA8ToXYZA8(const BitmapRGBA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapXYZA8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4ub* s = reinterpret_cast<const Color4ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 x_, y_, z_;
            hgl::RGB2XYZ(x_, y_, z_, s[i].r, s[i].g, s[i].b);
            d[i * 4 + 0] = x_;
            d[i * 4 + 1] = y_;
            d[i * 4 + 2] = z_;
            d[i * 4 + 3] = s[i].a;
        }
        return result;
    }

    BitmapRGBA8 ConvertXYZA8ToRGBA8(const BitmapXYZA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapRGBA8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const uint8* s = reinterpret_cast<const uint8*>(src.GetData());
        Color4ub* d = reinterpret_cast<Color4ub*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 r, g, b;
            hgl::XYZ2RGB(r, g, b, s[i * 4 + 0], s[i * 4 + 1], s[i * 4 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
            d[i].a = s[i * 4 + 3];
        }
        return result;
    }

    // float 版本：BitmapRGB32F <-> BitmapXYZF
    BitmapXYZF ConvertRGB32FToXYZF(const BitmapRGB32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapXYZF result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3f* s = reinterpret_cast<const Color3f*>(src.GetData());
        math::Vector3f* d = reinterpret_cast<math::Vector3f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float x, y, z;
            hgl::RGB2XYZ(x, y, z, s[i].x, s[i].y, s[i].z);
            d[i].x = x;
            d[i].y = y;
            d[i].z = z;
        }
        return result;
    }

    BitmapRGB32F ConvertXYZFToRGB32F(const BitmapXYZF& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapRGB32F result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const math::Vector3f* s = reinterpret_cast<const math::Vector3f*>(src.GetData());
        Color3f* d = reinterpret_cast<Color3f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float r, g, b;
            hgl::XYZ2RGB(r, g, b, s[i].x, s[i].y, s[i].z);
            d[i].x = hgl::math::Clamp(r, 0.0f, 1.0f);
            d[i].y = hgl::math::Clamp(g, 0.0f, 1.0f);
            d[i].z = hgl::math::Clamp(b, 0.0f, 1.0f);
        }
        return result;
    }

    BitmapXYZAF ConvertRGBA32FToXYZAF(const BitmapRGBA32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapXYZAF result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4f* s = reinterpret_cast<const Color4f*>(src.GetData());
        math::Vector4f* d = reinterpret_cast<math::Vector4f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float x, y, z;
            hgl::RGB2XYZ(x, y, z, s[i].x, s[i].y, s[i].z);
            d[i].x = x;
            d[i].y = y;
            d[i].z = z;
            d[i].w = s[i].w;
        }
        return result;
    }

    BitmapRGBA32F ConvertXYZAFToRGBA32F(const BitmapXYZAF& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapRGBA32F result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const math::Vector4f* s = reinterpret_cast<const math::Vector4f*>(src.GetData());
        Color4f* d = reinterpret_cast<Color4f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float r, g, b;
            hgl::XYZ2RGB(r, g, b, s[i].x, s[i].y, s[i].z);
            d[i].x = hgl::math::Clamp(r, 0.0f, 1.0f);
            d[i].y = hgl::math::Clamp(g, 0.0f, 1.0f);
            d[i].z = hgl::math::Clamp(b, 0.0f, 1.0f);
            d[i].w = s[i].w;
        }
        return result;
    }
}
