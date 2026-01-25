// BitmapColorConversion_YCoCg.cpp
// 实现YCoCg相关的颜色空间转换函数
#include "hgl/2d/ColorConversion.h"
#include "hgl/color/YCoCg.h"

namespace hgl::bitmap::color
{
    BitmapYCoCg8 ConvertRGBToYCoCg(const BitmapRGB8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapYCoCg8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3ub* s = reinterpret_cast<const Color3ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 y, co, cg;
            hgl::RGB2YCoCg(y, co, cg, s[i].r, s[i].g, s[i].b);
            d[i * 3 + 0] = y;
            d[i * 3 + 1] = co;
            d[i * 3 + 2] = cg;
        }
        return result;
    }

    BitmapRGB8 ConvertYCoCgToRGB(const BitmapYCoCg8& src)
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
            hgl::YCoCg2RGB(r, g, b, s[i * 3 + 0], s[i * 3 + 1], s[i * 3 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
        }
        return result;
    }

    BitmapYCoCgA8 ConvertRGBAToYCoCgA(const BitmapRGBA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapYCoCgA8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4ub* s = reinterpret_cast<const Color4ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 y, co, cg;
            hgl::RGB2YCoCg(y, co, cg, s[i].r, s[i].g, s[i].b);
            d[i * 4 + 0] = y;
            d[i * 4 + 1] = co;
            d[i * 4 + 2] = cg;
            d[i * 4 + 3] = s[i].a;
        }
        return result;
    }

    BitmapRGBA8 ConvertYCoCgAToRGBA(const BitmapYCoCgA8& src)
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
            hgl::YCoCg2RGB(r, g, b, s[i * 4 + 0], s[i * 4 + 1], s[i * 4 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
            d[i].a = s[i * 4 + 3];
        }
        return result;
    }

    // float 版本：BitmapRGB32F <-> BitmapYCoCgF
    BitmapYCoCgF ConvertRGB32FToYCoCgF(const BitmapRGB32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapYCoCgF result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3f* s = reinterpret_cast<const Color3f*>(src.GetData());
        math::Vector3f* d = reinterpret_cast<math::Vector3f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float y, co, cg;
            hgl::RGB2YCoCg(y, co, cg, s[i].x, s[i].y, s[i].z);
            d[i].x = y;
            d[i].y = co;
            d[i].z = cg;
        }
        return result;
    }

    BitmapRGB32F ConvertYCoCgFToRGB32F(const BitmapYCoCgF& src)
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
            hgl::YCoCg2RGB(r, g, b, s[i].x, s[i].y, s[i].z);
            d[i].x = hgl::math::Clamp(r, 0.0f, 1.0f);
            d[i].y = hgl::math::Clamp(g, 0.0f, 1.0f);
            d[i].z = hgl::math::Clamp(b, 0.0f, 1.0f);
        }
        return result;
    }
}
