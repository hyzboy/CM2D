#pragma once

#include<hgl/type/DataType.h>
#include<hgl/2d/Bitmap.h>

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

        /**
         * 位图混合处理模板
         */
        template<typename ST,typename DT> class BlendBitmap
        {
        public:

            void operator ()(const Bitmap<ST> *src,Bitmap<DT> *dst,const float alpha)const;
        };//template<typename ST,typename DT> class BlendBitmap
    }//namespace bitmap
}//namespace hgl
