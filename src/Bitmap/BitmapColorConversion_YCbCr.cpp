// BitmapColorConversion_YCbCr.cpp
// 实现YCbCr相关的颜色空间转换函数
#include <hgl/2d/ColorConversion.h>
#include <hgl/color/YCbCr.h>
#include <hgl/math/Vector.h>
#include <hgl/math/Clamp.h>

namespace hgl::bitmap::color
{
    BitmapYCbCr8 ConvertRGBToYCbCr(const BitmapRGB8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapYCbCr8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3ub* s = reinterpret_cast<const Color3ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 y, cb, cr;
            hgl::RGB2YCbCr(y, cb, cr, s[i].r, s[i].g, s[i].b);
            d[i * 3 + 0] = y;
            d[i * 3 + 1] = cb;
            d[i * 3 + 2] = cr;
        }
        return result;
    }

    BitmapRGB8 ConvertYCbCrToRGB(const BitmapYCbCr8& src)
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
            hgl::YCbCr2RGB(r, g, b, s[i * 3 + 0], s[i * 3 + 1], s[i * 3 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
        }
        return result;
    }

    BitmapYCbCrA8 ConvertRGBAToYCbCrA(const BitmapRGBA8& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapYCbCrA8 result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color4ub* s = reinterpret_cast<const Color4ub*>(src.GetData());
        uint8* d = reinterpret_cast<uint8*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            uint8 y, cb, cr;
            hgl::RGB2YCbCr(y, cb, cr, s[i].r, s[i].g, s[i].b);
            d[i * 4 + 0] = y;
            d[i * 4 + 1] = cb;
            d[i * 4 + 2] = cr;
            d[i * 4 + 3] = s[i].a;
        }
        return result;
    }

    BitmapRGBA8 ConvertYCbCrAToRGBA(const BitmapYCbCrA8& src)
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
            hgl::YCbCr2RGB(r, g, b, s[i * 4 + 0], s[i * 4 + 1], s[i * 4 + 2]);
            d[i].r = r;
            d[i].g = g;
            d[i].b = b;
            d[i].a = s[i * 4 + 3];
        }
        return result;
    }

    // float 版本：BitmapRGB32F <-> BitmapYCbCrF
    BitmapYCbCrF ConvertRGB32FToYCbCrF(const BitmapRGB32F& src)
    {
        const int w = src.GetWidth();
        const int h = src.GetHeight();
        BitmapYCbCrF result;
        if (w == 0 || h == 0 || !src.GetData())
            return result;
        result.Create(w, h);
        const Color3f* s = reinterpret_cast<const Color3f*>(src.GetData());
        math::Vector3f* d = reinterpret_cast<math::Vector3f*>(result.GetData());
        for (int i = 0; i < w * h; ++i)
        {
            float y, cb, cr;
            hgl::RGB2YCbCr(y, cb, cr, s[i].x, s[i].y, s[i].z);
            d[i].x = y;
            d[i].y = cb;
            d[i].z = cr;
        }
        return result;
    }

    BitmapRGB32F ConvertYCbCrFToRGB32F(const BitmapYCbCrF& src)
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
            hgl::YCbCr2RGB(r, g, b, s[i].x, s[i].y, s[i].z);
            d[i].x = hgl::math::Clamp(r, 0.0f, 1.0f);
            d[i].y = hgl::math::Clamp(g, 0.0f, 1.0f);
            d[i].z = hgl::math::Clamp(b, 0.0f, 1.0f);
        }
        return result;
    }
}
