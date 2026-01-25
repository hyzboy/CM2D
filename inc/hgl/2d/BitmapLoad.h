#pragma once
#include<hgl/2d/Bitmap.h>
#include<hgl/io/FileInputStream.h>

namespace hgl
{
    namespace bitmap
    {
        struct BitmapLoader
        {
            virtual const uint OnChannels()const=0;
            virtual const uint OnChannelBits()const=0;

            const uint OnPixelBits()const
            {
                return OnChannelBits()*OnChannels();
            }

            virtual void *OnRecvBitmap(uint w,uint h)=0;

            virtual void OnLoadFailed()=0;
            virtual void OnFlip()=0;
        };

        template<typename T> struct BitmapLoaderImpl:public BitmapLoader
        {
            T *bmp;

        public:

            BitmapLoaderImpl()
            {
                bmp=nullptr;
            }

            const uint OnChannels()const override{return bmp->GetChannels();}
            const uint OnChannelBits()const override{return bmp->GetChannelBits();}

            void *OnRecvBitmap(uint w,uint h) override
            {
                if(!bmp)
                    bmp=new T;

                bmp->Create(w,h);
                return bmp->GetData();
            }

            void OnLoadFailed() override
            {
                SAFE_CLEAR(bmp);
            }

            void OnFlip() override
            {
                if(bmp)
                    bmp->Flip();
            }
        };

        bool LoadBitmapFromTGAStream(io::InputStream *,BitmapLoader *);

        template<typename T>
        inline T *LoadBitmapFromTGA(io::InputStream *is)
        {
            BitmapLoaderImpl<T> bli;

            if(LoadBitmapFromTGAStream(is,&bli))
                return bli.bmp;

            return(nullptr);
        }

        inline BitmapRGB8 *LoadBitmapRGB8FromTGA(io::InputStream *is){return LoadBitmapFromTGA<BitmapRGB8>(is);}
        inline BitmapRGBA8 *LoadBitmapRGBA8FromTGA(io::InputStream *is){return LoadBitmapFromTGA<BitmapRGBA8>(is);}

        template<typename T>
        inline T *LoadBitmapFromTGA(const OSString &filename)
        {
            io::OpenFileInputStream fis(filename);

            if(!fis)
                return(nullptr);

            return LoadBitmapFromTGA<T>(&fis);
        }

        inline BitmapRGB8 *LoadBitmapRGB8FromTGA(const OSString &filename){return LoadBitmapFromTGA<BitmapRGB8>(filename);}
        inline BitmapRGBA8 *LoadBitmapRGBA8FromTGA(const OSString &filename){return LoadBitmapFromTGA<BitmapRGBA8>(filename);}
    }//namespace bitmap
}//namespace hgl