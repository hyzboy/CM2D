#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/color/Color3ub.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/utf.h>
#include<vector>
#include<hgl/2d/Bitmap.h>
#include<hgl/2d/BitmapSave.h>
namespace hgl
{
    namespace imgfmt
    {
        constexpr const uint TGA_IMAGE_TYPE_COLOR_MAP   =1;
        constexpr const uint TGA_IMAGE_TYPE_TRUE_COLOR  =2;
        constexpr const uint TGA_IMAGE_TYPE_GRAYSCALE   =3;

        constexpr const uint TGA_DIRECTION_LOWER_LEFT   =0;
        constexpr const uint TGA_DIRECTION_UPPER_LEFT   =1;

#pragma pack(push,1)
        struct TGAHeader
        {
            uint8 id;
            uint8 color_map_type;
            uint8 image_type;               // 1 colormap image ,2 true-color,3 grayscale

            uint16 color_map_first;
            uint16 color_map_length;
            uint8 color_map_size;

            uint16 x_origin;
            uint16 y_origin;

            uint16 width;
            uint16 height;
            uint8 bit;
            uint8 image_desc;
        };

        union TGAImageDesc
        {
            uint8 image_desc;
            struct
            {
                uint alpha_depth:4;
                uint reserved:1;
                uint direction:1;       //0 lower-left,1 upper left
            };
        };
#pragma pack(pop)

        constexpr size_t TGAHeaderSize=sizeof(TGAHeader);       ///<TGA文件头，必须等于18

        bool FillTGAHeader(TGAHeader *header,const uint16 width,const uint16 height,const uint8 channels,const uint8 single_channel_bits=8);
    }//namespace imgfmt

    namespace bitmap
    {
        /**
         * Save bitmap to TGA file (generic version)
         * @param filename Output TGA filename
         * @param bmp Bitmap pointer (any bitmap type with GetChannels, GetChannelBits, GetWidth, GetHeight, GetData methods)
         * @return true if save successful, false otherwise
         */
        template<typename T>
        bool SaveTga(const char *filename, T *bmp)
        {
            if(!filename || !bmp)
                return false;

            hgl::io::OpenFileOutputStream output(ToOSString(filename), hgl::io::FileOpenMode::CreateTrunc);
            if(!output)
                return false;

            return SaveBitmapToTGA(output,
                bmp->GetData(),
                bmp->GetWidth(),
                bmp->GetHeight(),
                bmp->GetChannels(),
                bmp->GetChannelBits());
        }

        /**
         * Save bitmap to TGA file (specialization for Bitmap<Color3ub, 3>)
         * Handles Color3ub (RGB) packed in uint32 by converting to proper RGB format
         * Only used when sizeof(Color3ub) != 3 (indicating padding exists)
         * @param filename Output TGA filename
         * @param bmp Bitmap pointer
         * @return true if save successful, false otherwise
         */
        template<>
        inline bool SaveTga<Bitmap<Color3ub, 3>>(const char *filename, Bitmap<Color3ub, 3> *bmp)
        {
            if(!filename || !bmp)
                return false;

            hgl::io::OpenFileOutputStream output(ToOSString(filename), hgl::io::FileOpenMode::CreateTrunc);
            if(!output)
                return false;

            const uint width = bmp->GetWidth();
            const uint height = bmp->GetHeight();

            // If Color3ub has padding (sizeof != 3), we need to repack the data
            if constexpr(sizeof(Color3ub) != 3)
            {
                const Color3ub *src = bmp->GetData();
                std::vector<uint8> rgb_data(width * height * 3);

                for(size_t i = 0; i < width * height; ++i)
                {
                    rgb_data[i * 3] = src[i].r;
                    rgb_data[i * 3 + 1] = src[i].g;
                    rgb_data[i * 3 + 2] = src[i].b;
                }

                return SaveBitmapToTGA(output,
                    rgb_data.data(),
                    width,
                    height,
                    3,
                    8);
            }
            else
            {
                // No padding, can save directly
                return SaveBitmapToTGA(output,
                    bmp->GetData(),
                    width,
                    height,
                    3,
                    8);
            }
        }
    }//namespace bitmap
}//namespace hgl
