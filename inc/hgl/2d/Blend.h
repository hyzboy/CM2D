#pragma once

#include<hgl/type/DataType.h>

namespace hgl
{
    namespace bitmap
    {
        template<typename T> struct BlendColor
        {
            virtual const T operator()(const T &src,const T &)const
            {
                return src;
            }

            virtual const T operator()(const T &src,const T &,const float &)const
            {
                return src;
            }
        };//template<typename T> struct BlendColor
    }//namespace bitmap
}//namespace hgl