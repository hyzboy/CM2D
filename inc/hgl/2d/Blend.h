#pragma once

#include<hgl/type/DataType.h>

namespace hgl
{
    namespace bitmap
    {
        enum class BlendMode
        {
            NoBlend,            ///<不混合
            Alpha,              ///<Alpha混合
            Add,                ///<加法混合
            Sub,                ///<减法混合
        };//enum class BlendMode

        template<typename T> struct BlendColor
        {
            BlendMode mode=BlendMode::NoBlend;

        public:

            virtual const T &BlendNoBlend(const T &src,const T &dst,const float alpha)
            {
                return src;
            }

            virtual const T &BlendAlpha(const T &src,const T &dst,const float alpha)
            {
                return src*alpha+dst*(1-alpha);
            }

            virtual const T &BlendAdd(const T &src,const T &dst,const float alpha)
            {
                return src*alpha+dst;
            }

            virtual const T &BlendSub(const T &src,const T &dst,const float alpha)
            {
                return src*alpha-dst;
            }

            virtual const T &Blend(const T &src,const T &dst,const float alpha)
            {
                switch(mode)
                {
                    case BlendMode::NoBlend:    return BlendNoBlend(src,dst,alpha);
                    case BlendMode::Alpha:      return BlendAlpha(src,dst,alpha);
                    case BlendMode::Add:        return BlendAdd(src,dst,alpha);
                    case BlendMode::Sub:        return BlendSub(src,dst,alpha);
                }

                return src;
            }
        };//template<typename T> class BlendColor
    }//namespace bitmap
}//namespace hgl