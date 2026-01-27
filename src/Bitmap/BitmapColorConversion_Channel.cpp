#include <hgl/2d/ColorConversion.h>

namespace hgl::bitmap::color
{
    // 通道提取
    template<typename T, uint C>
    Bitmap<uint8, 1> ExtractChannel(const Bitmap<T, C>& source, uint channel_index)
    {
        static_assert(C > 1, "Source must be multi-channel bitmap");
        const int width = source.GetWidth();
        const int height = source.GetHeight();
        Bitmap<uint8, 1> result;
        if (width == 0 || height == 0 || !source.GetData() || channel_index >= C)
            return result;
        result.Create(width, height);
        const T* src = source.GetData();
        uint8* dst = result.GetData();
        for (int i = 0; i < width * height; ++i)
        {
            if constexpr (C == 2)
            {
                if (channel_index == 0)
                    dst[i] = static_cast<uint8>(src[i].x);
                else
                    dst[i] = static_cast<uint8>(src[i].y);
            }
            else if constexpr (C == 3)
            {
                if (channel_index == 0)
                    dst[i] = static_cast<uint8>(src[i].r);
                else if (channel_index == 1)
                    dst[i] = static_cast<uint8>(src[i].g);
                else
                    dst[i] = static_cast<uint8>(src[i].b);
            }
            else if constexpr (C == 4)
            {
                if (channel_index == 0)
                    dst[i] = static_cast<uint8>(src[i].r);
                else if (channel_index == 1)
                    dst[i] = static_cast<uint8>(src[i].g);
                else if (channel_index == 2)
                    dst[i] = static_cast<uint8>(src[i].b);
                else
                    dst[i] = static_cast<uint8>(src[i].a);
            }
        }
        return result;
    }
}
