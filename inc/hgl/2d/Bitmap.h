#ifndef HGL_2D_BITMAP_INCLUDE
#define HGL_2D_BITMAP_INCLUDE

#include<hgl/type/DataType.h>
#include<hgl/math/HalfFloat.h>
namespace hgl
{
    /**
     * 简单的2D象素处理
     */
    template<typename T,uint CHANNELS> class Bitmap
    {
        uint width,height;

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

        const uint GetWidth()const{return width;}
        const uint GetHeight()const{return height;}
        const uint GetTotalPixels()const{return width*height;}
        const uint GetLineBytea()const{return width*CHANNELS*sizeof(T);}
        const uint GetTotalBytes()const{return width*height*CHANNELS*sizeof(T);}

        T *GetData(){return data;}

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

            data=new T[width*height*CHANNELS];

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

        void ClearColor(T *color)
        {
            if(!data||!color)return;
    
            uint total=width*height;

            T *p=data;

            for(size_t i=0; i<length; i++)
            {
                for(uint j=0;j<CHANNELS;j++)
                {
                    *p=*color;
                    ++p;
                }
            }
        }

        bool SplitChannels(Bitmap<T,1> *outputs)
        {
            if(!data||!outputs)            
                return(false);

            T *sp=data;
            AutoDeleteArray<T *> op=new T *[CHANNELS];

            for(uint i=0;i<CHANNELS;i++)
            {
                outputs[i].Create(width,height);
                op[i]=outputs[i].data;
            }

            const uint total=width*height;

            for(uint i=0;i<total;i++)
            {
                for(uint j=0;j<CHANNELS;j++)
                {
                    op[j]=*p;
                    ++op[h];
                    ++p;
                }
            }

            return(true);
        }

        bool MerageChannels(Bitmap<T,1> *inputs)
        {
            if(!data||!inputs)            
                return(false);

            T *dp=data;
            AutoDeleteArray<T *> ip=new T *[CHANNELS];

            for(uint i=0;i<CHANNELS;i++)
            {
                if(!inputs[i].data)
                    return(false);

                ip[i]=inputs[i].data;
            }

            const uint total=width*height;

            for(uint i=0;i<total;i++)
            {
                for(uint j=0;j<CHANNELS;j++)
                {
                    *dp=*ip[j];
                    ++ip[j];
                    ++dp;
                }
            }

            return(true);
        }

        void Flip()
        {
            if(!data||height<=1)return;

            const uint line_pixels=width*CHANNELS;
            const uint line_bytes=width*CHANNELS*sizeof(T);

            T *temp=new T[line_pixels];

            T *top=data;
            T *bottom=data+(line_pixels*(height-1));

            while(top<bottom)
            {
                memcpy(temp,top,line_bytes);
                memcpy(top,bottom,line_bytes);
                memcpy(bottom,temp,line_bytes);

                top+=line_pixels;
                bottom-=line_pixels;
            }

            delete[] temp;
        }

        void PutPixel(int x,int y,const T *color)
        {
            if(!data||!color)return;

            if(x<0||y<0||x>=width||y>=height)return;

            T *p=data+(y*width+x)*CHANNELS;

            for(uint i=0;i<CHANNELS;i++)
            {
                *p=*color;
                ++p;
                ++color;
            }
        }

        bool GetPixel(int x,int y,T *color)
        {
            if(!data||!color)return(false);

            if(x<0||y<0||x>=width||y>=height)return(false);

            T *p=data+(y*width+x)*CHANNELS;

            for(uint i=0;i<CHANNELS;i++)
            {
                *color=*p;
                ++p;
                ++color;
            }

            return(true);
        }
    };//template<typename T,uint CHANNELS> class Bitmap

    using BitmapR8      =Bitmap<uint8,  1>;
    using BitmapR16     =Bitmap<uint16, 1>;
    using BitmapR32U    =Bitmap<uint32, 1>;
    using BitmapR32F    =Bitmap<float,  1>;

    using BitmapRG8     =Bitmap<uint8,  2>;
    using BitmapRG16    =Bitmap<uint16, 2>;
    using BitmapRG32U   =Bitmap<uint32, 2>;
    using BitmapRG32F   =Bitmap<float,  2>;

    using BitmapRGBA8   =Bitmap<uint8,  4>;
    using BitmapRGBA16  =Bitmap<uint16, 4>;
    using BitmapRGBA32U =Bitmap<uint32, 4>;
    using BitmapRGBA32F =Bitmap<float,  4>;
}//namespace hgl
#endif//HGL_2D_BITMAP_INCLUDE
