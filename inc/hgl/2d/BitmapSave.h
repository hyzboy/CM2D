#pragma once
#include<hgl/2d/Bitmap.h>
#include<hgl/io/FileOutputStream.h>

namespace hgl
{
    namespace bitmap
    {
        bool SaveBitmapToTGA(io::OutputStream *os,void *data,uint width,uint height,uint channels,uint single_channel_bits);

        template<typename T>
        inline bool SaveBitmapToTGA(io::OutputStream *os,const T *bmp)
        {
            if(!os||!bmp)return(false);

            return SaveBitmapToTGA(os,(void *)(bmp->GetData()),bmp->GetWidth(),bmp->GetHeight(),bmp->GetChannels(),bmp->GetChannelBits());
        }

        template<typename T>
        inline bool SaveBitmapToTGA(const OSString &filename,T *bmp)
        {
            if(filename.IsEmpty()||!bmp)
                return(false);

            io::OpenFileOutputStream fos(filename,io::FileOpenMode::CreateTrunc);

            if(!fos)
                return(false);
        
            return SaveBitmapToTGA(fos,bmp);
        }
    }//namespace bitmap
}//namespace hgl