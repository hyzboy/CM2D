#pragma once

#include<hgl/platform/Platform.h>
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

        constexpr size_t TGAHeaderSize=sizeof(TGAHeader);

        bool FillTGAHeader(TGAHeader *header,const uint16 width,const uint16 height,const uint8 channels,const uint8 single_channel_bits=8);
    }//namespace imgfmt
}//namespace hgl
