// BitmapColorConversion_OKLab.cpp
// 实现OKLab颜色空间转换函数
#include "hgl/2d/ColorConversion.h"
#include <hgl/color/OKLab.h>

namespace hgl::bitmap::color
{
    // 8-bit RGB <-> OKLab
    BitmapOKLab8 ConvertRGBToOKLab(const BitmapRGB8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapOKLab8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3ub* s = reinterpret_cast<const Color3ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 l, a, b;
            hgl::RGB2OKLab(l, a, b, s[i].r, s[i].g, s[i].b);
            d[i * 3 + 0] = l;
            d[i * 3 + 1] = a;
            d[i * 3 + 2] = b;
        }
        return result;
    }

    BitmapRGB8 ConvertOKLabToRGB(const BitmapOKLab8& src)
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
            hgl::OKLab2RGB(r, g, b, s[i * 3 + 0], s[i * 3 + 1], s[i * 3 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
        }
        return result;
    }

    // 8-bit RGBA <-> OKLaba8 (OKLab + alpha)
    BitmapOKLaba8 ConvertRGBA8ToOKLaba8(const BitmapRGBA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapOKLaba8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4ub* s = reinterpret_cast<const Color4ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 l, a, b;
            hgl::RGB2OKLab(l, a, b, s[i].r, s[i].g, s[i].b);
            d[i * 4 + 0] = l;
            d[i * 4 + 1] = a;
            d[i * 4 + 2] = b;
            d[i * 4 + 3] = s[i].a;
        }
        return result;
    }

    BitmapRGBA8 ConvertOKLaba8ToRGBA8(const BitmapOKLaba8& src)
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
            hgl::OKLab2RGB(r, g, b, s[i * 4 + 0], s[i * 4 + 1], s[i * 4 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
            d[i].a = s[i * 4 + 3];
        }
        return result;
    }

    // float RGB32F <-> OKLabF
    BitmapOKLabF ConvertRGB32FToOKLabF(const BitmapRGB32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapOKLabF result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3f* s = reinterpret_cast<const Color3f*>(src.GetData());
        math::Vector3f* d = reinterpret_cast<math::Vector3f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float l, a, b;
            hgl::RGB2OKLab(l, a, b, s[i].x, s[i].y, s[i].z);
            d[i].x = l;
            d[i].y = a;
            d[i].z = b;
        }
        return result;
    }

    BitmapRGB32F ConvertOKLabFToRGB32F(const BitmapOKLabF& src)
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
            hgl::OKLab2RGB(r, g, b, s[i].x, s[i].y, s[i].z);
            d[i].x = hgl::math::Clamp(r, 0.0f, 1.0f);
            d[i].y = hgl::math::Clamp(g, 0.0f, 1.0f);
            d[i].z = hgl::math::Clamp(b, 0.0f, 1.0f);
        }
        return result;
    }

    // float RGBA32F <-> OKLabAf
    BitmapOKLabAf ConvertRGBA32FToOKLabAf(const BitmapRGBA32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapOKLabAf result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4f* s = reinterpret_cast<const Color4f*>(src.GetData());
        math::Vector4f* d = reinterpret_cast<math::Vector4f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float l, a, b;
            hgl::RGB2OKLab(l, a, b, s[i].x, s[i].y, s[i].z);
            d[i].x = l;
            d[i].y = a;
            d[i].z = b;
            d[i].w = s[i].w;
        }
        return result;
    }

    BitmapRGBA32F ConvertOKLabAfToRGBA32F(const BitmapOKLabAf& src)
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
            hgl::OKLab2RGB(r, g, b, s[i].x, s[i].y, s[i].z);
            d[i].x = hgl::math::Clamp(r, 0.0f, 1.0f);
            d[i].y = hgl::math::Clamp(g, 0.0f, 1.0f);
            d[i].z = hgl::math::Clamp(b, 0.0f, 1.0f);
            d[i].w = s[i].w;
        }
        return result;
    }
}
