#include<hgl/2d/BitmapLoad.h>
#include<hgl/2d/TGA.h>
#include<hgl/io/InputStream.h>
#include<hgl/io/OutputStream.h>

namespace hgl
{
    using namespace io;
    using namespace imgfmt;

    namespace bitmap
    {
        bool LoadBitmapFromTGAStream(io::InputStream *is,BitmapLoader *bl)
        {
            if(!is||!bl)return(false);

            TGAHeader tga_header;
            TGAImageDesc tga_desc;

            if(is->Read(&tga_header,TGAHeaderSize)!=TGAHeaderSize)
                return(false);

            if(tga_header.image_type!=TGA_IMAGE_TYPE_TRUE_COLOR)
                return(false);

            if(tga_header.bit!=bl->OnPixelBits())
                return(false);

            tga_desc.image_desc=tga_header.image_desc;

            void *bmp=bl->OnRecvBitmap(tga_header.width,tga_header.height);

            const uint total_bytes=(tga_header.width*tga_header.height*tga_header.bit)>>3;

            if(is->Read(bmp,total_bytes)!=total_bytes)
            {
                bl->OnLoadFailed();
                return(false);
            }

            if(tga_desc.direction==TGA_DIRECTION_LOWER_LEFT)
               bl->OnFlip();

            return(true);
        }
    
        /**
        * 以TGA格式保存Bitmap数据到流
        */
        bool SaveBitmapToTGA(io::OutputStream *os,void *data,uint width,uint height,uint channels,uint single_channel_bits)
        {
            if(!os||!data||width<=0||height<=0||channels<=0||single_channel_bits<=0)
                return(false);

            TGAHeader tga_header;

            const uint total_bytes=(width*height*channels*single_channel_bits)>>3;

            FillTGAHeader(&tga_header,width,height,channels,single_channel_bits);

            if(os->Write(&tga_header,TGAHeaderSize)!=TGAHeaderSize)
                return(false);

            if(os->Write(data,total_bytes)!=total_bytes)
                return(false);

            return(true);
        }
    }//namespace bitmap
}//namespace hgl
