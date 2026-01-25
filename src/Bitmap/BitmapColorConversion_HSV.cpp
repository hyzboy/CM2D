// BitmapColorConversion_HSV.cpp
// 实现HSV相关的颜色空间转换函数
#include "hgl/2d/ColorConversion.h"
#include <hgl/color/HSV.h>

namespace hgl::bitmap::color
{
    BitmapHSV8 ConvertRGBToHSV(const BitmapRGB8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapHSV8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3ub* s = reinterpret_cast<const Color3ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 h_, s_, v_;
            hgl::RGB2HSV(h_, s_, v_, s[i].r, s[i].g, s[i].b);
            d[i * 3 + 0] = h_;
            d[i * 3 + 1] = s_;
            d[i * 3 + 2] = v_;
        }
        return result;
    }
    BitmapRGB8 ConvertHSVToRGB(const BitmapHSV8& src)
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
            hgl::HSV2RGB(r, g, b, s[i * 3 + 0], s[i * 3 + 1], s[i * 3 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
        }
        return result;
    }

    BitmapHSVA8 ConvertRGBA8ToHSVA8(const BitmapRGBA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapHSVA8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4ub* s = reinterpret_cast<const Color4ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 h_, s_, v_;
            hgl::RGB2HSV(h_, s_, v_, s[i].r, s[i].g, s[i].b);
            d[i * 4 + 0] = h_;
            d[i * 4 + 1] = s_;
            d[i * 4 + 2] = v_;
            d[i * 4 + 3] = s[i].a;
        }
        return result;
    }

    BitmapRGBA8 ConvertHSVA8ToRGBA8(const BitmapHSVA8& src)
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
            hgl::HSV2RGB(r, g, b, s[i * 4 + 0], s[i * 4 + 1], s[i * 4 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
            d[i].a = s[i * 4 + 3];
        }
        return result;
    }
    // float 版本：BitmapRGB32F <-> BitmapHSVF
    BitmapHSVF ConvertRGB32FToHSVF(const BitmapRGB32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapHSVF result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3f* s = reinterpret_cast<const Color3f*>(src.GetData());
        math::Vector3f* d = reinterpret_cast<math::Vector3f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float hf, sf, vf;
            hgl::RGB2HSV(hf, sf, vf, s[i].x, s[i].y, s[i].z);
            d[i].x = hf / 360.0f; // 归一化到[0,1]
            d[i].y = sf;
            d[i].z = vf;
        }
        return result;
    }

    BitmapRGB32F ConvertHSVFToRGB32F(const BitmapHSVF& src)
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
            hgl::HSV2RGB(r, g, b, s[i].x * 360.0f, s[i].y, s[i].z);
            d[i].x = r;
            d[i].y = g;
            d[i].z = b;
        }
        return result;
    }

    BitmapHSVAF ConvertRGBA32FToHSVAF(const BitmapRGBA32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapHSVAF result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4f* s = reinterpret_cast<const Color4f*>(src.GetData());
        math::Vector4f* d = reinterpret_cast<math::Vector4f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float hf, sf, vf;
            hgl::RGB2HSV(hf, sf, vf, s[i].x, s[i].y, s[i].z);
            d[i].x = hf / 360.0f;
            d[i].y = sf;
            d[i].z = vf;
            d[i].w = s[i].w;
        }
        return result;
    }

    BitmapRGBA32F ConvertHSVAFToRGBA32F(const BitmapHSVAF& src)
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
            hgl::HSV2RGB(r, g, b, s[i].x * 360.0f, s[i].y, s[i].z);
            d[i].x = r;
            d[i].y = g;
            d[i].z = b;
            d[i].w = s[i].w;
        }
        return result;
    }
}
