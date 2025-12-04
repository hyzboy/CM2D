#pragma once

#include<hgl/CoreType.h>
#include<hgl/math/Vector.h>
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
    };//template<typename T> class Bitmap

    using BitmapGrey8=Bitmap<uint8,1>;
    using BitmapRG8=Bitmap<Vector2u8,2>;
    using BitmapRGB8=Bitmap<Vector3u8,3>;
    using BitmapRGBA8=Bitmap<Vector4u8,4>;

    using BitmapU16=Bitmap<uint16,1>;
    using BitmapU32=Bitmap<uint32,1>;

    using Bitmap32F=Bitmap<float,1>;
    using BitmapRG32F=Bitmap<Vector2f,2>;
    using BitmapRGB32F=Bitmap<Vector3f,3>;
    using BitmapRGBA32F=Bitmap<Vector4f,4>;
}//namespace hgl::bitmap
