#include<hgl/2d/TGA.h>

namespace hgl
{
    namespace imgfmt
    {
        bool FillTGAHeader(TGAHeader *header,const uint16 width,const uint16 height,const uint8 channels,const uint8 single_channel_bits)
        {
            if(!header)return(false);
            if(!width||!height)return(false);

            if(channels!=1
             &&channels!=3
             &&channels!=4)return(false);

            memset(header,0,sizeof(TGAHeader));

            header->width=width;
            header->height=height;

            TGAImageDesc desc;

            desc.image_desc=0;

            if(channels==1)
            {
                header->image_type=3;
                header->bit=single_channel_bits;
            }
            else
            {
                header->image_type=2;
                header->bit=channels*single_channel_bits;

                if(channels==4)
                    desc.alpha_depth=single_channel_bits;
            }

            desc.direction=TGA_DIRECTION_UPPER_LEFT;

            header->image_desc=desc.image_desc;
            return(true);
        }
    }//namespace imgfmt
}//namespace hgl
