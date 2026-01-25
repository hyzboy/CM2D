#pragma once

#include<hgl/CoreType.h>
#include<hgl/math/Vector.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<hgl/color/Color3f.h>
#include<hgl/color/Color4f.h>
#include<hgl/type/String.h>
#include<hgl/math/HalfFloat.h>
#include<iterator>

namespace hgl::bitmap
{
    template<typename T>
    static void FillPixels(T *p,const T &color,const int length)
    {
        std::fill_n(p,length,color);
    }

    /**
    * 简单的2D象素处理
    */
    template<typename T,uint C> class Bitmap
    {
    protected:

        int width,height;

        T *data;

    public:

        Bitmap()
        {
            data=nullptr;
            width=height=0;
        }

        ~Bitmap()
        {
            delete[] data;
        }

        const uint GetChannels      ()const{return C;}
        const uint GetChannelBits   ()const{return (sizeof(T)/C)<<3;}

        const int  GetWidth         ()const{return width;}
        const int  GetHeight        ()const{return height;}
        const uint GetTotalPixels   ()const{return width*height;}
        const uint GetLineBytes     ()const{return width*sizeof(T);}
        const uint GetTotalBytes    ()const{return width*height*sizeof(T);}

        T *GetData(){return data;}
        T *GetData(int x,int y)
        {
            return (x<0||x>=width||y<0||y>=height)?nullptr:data+(y*width+x);
        }

        const T *GetData()const{return data;}
        const T *GetData(int x,int y)const
        {
            return (x<0||x>=width||y<0||y>=height)?nullptr:data+(y*width+x);
        }

        bool Create(uint w,uint h)
        {
            if(!w||!h)return(false);

            if(data)
            {
                if(width==w&&height==h)return(true);
            }

            width=w;
            height=h;

            delete[] data;

            data=new T[width*height];

            return(true);
        }

        void Clear()
        {
            if(data)
            {
                delete[] data;
                data=nullptr;
            }

            width=height=0;
        }

        void ClearColor(const T &color)
        {
            if(!data)return;

            FillPixels<T>(data,color,width*height);
        }

        void Flip()
        {
            if(!data||height<=1)return;

            const uint line_bytes=width*sizeof(T);

            T *temp=new T[width];

            T *top=data;
            T *bottom=data+(width*(height-1));

            while(top<bottom)
            {
                memcpy(temp,top,line_bytes);
                memcpy(top,bottom,line_bytes);
                memcpy(bottom,temp,line_bytes);

                top+=width;
                bottom-=width;
            }

            delete[] temp;
        }

        /**
         * 从噪声函数生成位图数据
         * @param noise 噪声生成器
         * @param scale 缩放因子
         * @param offsetX X坐标偏移
         * @param offsetY Y坐标偏移
         */
        template<typename NoiseGenerator>
        void GenerateFromNoise(const NoiseGenerator& noise, float scale, float offsetX = 0.0f, float offsetY = 0.0f)
        {
            if (!data || width <= 0 || height <= 0)
                return;

            T* ptr = data;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    float nx = (x + offsetX) * scale;
                    float ny = (y + offsetY) * scale;
                    *ptr = (T)noise.Generate(nx, ny);
                    ptr++;
                }
            }
        }

        /**
         * 规范化像素数据到指定范围
         * @param minValue 目标最小值
         * @param maxValue 目标最大值
         */
        void Normalize(float minValue, float maxValue)
        {
            if (!data || width <= 0 || height <= 0)
                return;

            // Find current min and max
            float currentMin = (float)data[0];
            float currentMax = (float)data[0];

            int totalPixels = width * height;
            for (int i = 0; i < totalPixels; i++)
            {
                float val = (float)data[i];
                if (val < currentMin) currentMin = val;
                if (val > currentMax) currentMax = val;
            }

            // Avoid division by zero
            const float MIN_RANGE = 0.0001f;
            float range = currentMax - currentMin;
            if (range < MIN_RANGE)
                range = 1.0f;

            // Normalize to [minValue, maxValue]
            float targetRange = maxValue - minValue;
            for (int i = 0; i < totalPixels; i++)
            {
                data[i] = (T)(minValue + ((float)data[i] - currentMin) / range * targetRange);
            }
        }
    };//template<typename T> class Bitmap

    using BitmapGrey8=Bitmap<uint8,1>;
    using BitmapRG8=Bitmap<math::Vector2u8,2>;
    using BitmapRGB8=Bitmap<Color3ub,3>;
    using BitmapRGBA8=Bitmap<Color4ub,4>;

    using BitmapU16=Bitmap<uint16,1>;
    using BitmapU32=Bitmap<uint32,1>;

    using Bitmap32F=Bitmap<float,1>;
    using BitmapRG32F=Bitmap<math::Vector2f,2>;
    using BitmapRGB32F=Bitmap<Color3f,3>;
    using BitmapRGBA32F=Bitmap<Color4f,4>;

    using BitmapYCbCr8=Bitmap<math::Vector3u8,3>;
    using BitmapYCbCrF=Bitmap<math::Vector3f,3>;
    using BitmapYCbCrA8=Bitmap<math::Vector4u8,4>;
    using BitmapYCbCrAF=Bitmap<math::Vector4f,4>;

    using BitmapYCoCg8=Bitmap<math::Vector3u8,3>;
    using BitmapYCoCgF=Bitmap<math::Vector3f,3>;
    using BitmapYCoCgA8=Bitmap<math::Vector4u8,4>;
    using BitmapYCoCgAF=Bitmap<math::Vector4f,4>;

    // HSV
    using BitmapHSV8=Bitmap<math::Vector3u8,3>;
    using BitmapHSVF=Bitmap<math::Vector3f,3>;
    using BitmapHSVA8=Bitmap<math::Vector4u8,4>;
    using BitmapHSVAF=Bitmap<math::Vector4f,4>;

    // HSL
    using BitmapHSL8=Bitmap<math::Vector3u8,3>;
    using BitmapHSLF=Bitmap<math::Vector3f,3>;
    using BitmapHSLA8=Bitmap<math::Vector4u8,4>;
    using BitmapHSLAf=Bitmap<math::Vector4f,4>;

    // XYZ
    using BitmapXYZ8=Bitmap<math::Vector3u8,3>;
    using BitmapXYZF=Bitmap<math::Vector3f,3>;
    using BitmapXYZA8=Bitmap<math::Vector4u8,4>;
    using BitmapXYZAF=Bitmap<math::Vector4f,4>;

    // OKLab
    using BitmapOKLab8=Bitmap<math::Vector3u8,3>;
    using BitmapOKLabF=Bitmap<math::Vector3f,3>;
    using BitmapOKLaba8=Bitmap<math::Vector4u8,4>;
    using BitmapOKLabAf=Bitmap<math::Vector4f,4>;
}//namespace hgl::bitmap
