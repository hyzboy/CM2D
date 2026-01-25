#pragma once

#include<hgl/CoreType.h>

namespace hgl::bitmap
{
    /**
    * 平面存储的多通道位图，每个通道单独分配缓冲区。
    * 模板参数传入单通道类型及通道数量。
    */
    template<typename T, uint C> class PlanarBitmap
    {
    protected:

        int width, height;
        std::array<T *, C> channels;

        void FreeChannels()
        {
            for (auto &ptr : channels)
            {
                delete[] ptr;
                ptr = nullptr;
            }
        }

    public:

        PlanarBitmap()
        {
            width = height = 0;
            channels.fill(nullptr);
        }

        ~PlanarBitmap()
        {
            FreeChannels();
        }

        const uint GetChannels() const { return C; }
        const uint GetChannelBits() const { return sizeof(T) << 3; }

        const int GetWidth() const { return width; }
        const int GetHeight() const { return height; }
        const uint GetTotalPixels() const { return width * height; }
        const uint GetLineBytes() const { return width * sizeof(T); }
        const uint GetTotalBytes() const { return width * height * sizeof(T) * C; }

        T *GetChannelData(uint channel)
        {
            return channel < C ? channels[channel] : nullptr;
        }

        const T *GetChannelData(uint channel) const
        {
            return channel < C ? channels[channel] : nullptr;
        }

        T *GetData(uint channel, int x, int y)
        {
            if (channel >= C || x < 0 || y < 0 || x >= width || y >= height)
                return nullptr;

            return channels[channel] ? channels[channel] + (y * width + x) : nullptr;
        }

        const T *GetData(uint channel, int x, int y) const
        {
            if (channel >= C || x < 0 || y < 0 || x >= width || y >= height)
                return nullptr;

            return channels[channel] ? channels[channel] + (y * width + x) : nullptr;
        }

        bool Create(uint w, uint h)
        {
            if (!w || !h)
                return false;

            if (channels[0] && width == (int)w && height == (int)h)
                return true;

            FreeChannels();

            width = w;
            height = h;

            for (auto &ptr : channels)
            {
                ptr = new T[width * height];
                if (!ptr)
                {
                    Clear();
                    return false;
                }
            }

            return true;
        }

        void Clear()
        {
            FreeChannels();
            width = height = 0;
        }

        void ClearChannel(uint channel, const T &value)
        {
            if (channel >= C || !channels[channel])
                return;

            FillPixels<T>(channels[channel], value, width * height);
        }

        void ClearColor(const std::array<T, C> &color)
        {
            for (uint i = 0; i < C; ++i)
            {
                if (channels[i])
                    FillPixels<T>(channels[i], color[i], width * height);
            }
        }

        bool SetPixel(int x, int y, const std::array<T, C> &color)
        {
            if (x < 0 || y < 0 || x >= width || y >= height)
                return false;

            const int idx = y * width + x;
            for (uint i = 0; i < C; ++i)
            {
                if (!channels[i])
                    return false;

                channels[i][idx] = color[i];
            }

            return true;
        }

        bool GetPixel(int x, int y, std::array<T, C> &outColor) const
        {
            if (x < 0 || y < 0 || x >= width || y >= height)
                return false;

            const int idx = y * width + x;
            for (uint i = 0; i < C; ++i)
            {
                if (!channels[i])
                    return false;

                outColor[i] = channels[i][idx];
            }

            return true;
        }
    };//template<typename T> class PlanarBitmap

    using PlanarBitmapGrey8=PlanarBitmap<uint8,1>;
    using PlanarBitmapRG8=PlanarBitmap<uint8,2>;
    using PlanarBitmapRGB8=PlanarBitmap<uint8,3>;
    using PlanarBitmapRGBA8=PlanarBitmap<uint8,4>;

    using PlanarBitmap32F=PlanarBitmap<float,1>;
    using PlanarBitmapRG32F=PlanarBitmap<float,2>;
    using PlanarBitmapRGB32F=PlanarBitmap<float,3>;
    using PlanarBitmapRGBA32F=PlanarBitmap<float,4>;
}//namespace hgl::bitmap
