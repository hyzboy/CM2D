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

            void operator ()(const ST *src,DT *dst,const float alpha)const;
        };//template<typename ST,typename DT> class BlendBitmap

        struct BlendColorU32Additive:public bitmap::BlendColor<uint32>
        {
            const uint32 operator()(const uint32 &src,const uint32 &dst)const
            {
                uint64 result=src+dst;

                return (result>HGL_U32_MAX)?HGL_U32_MAX:(result&HGL_U32_MAX);
            }

            const uint32 operator()(const uint32 &src,const uint32 &dst,const float &alpha)const
            {
                uint64 result=src*alpha+dst;

                return (result>HGL_U32_MAX)?HGL_U32_MAX:(result&HGL_U32_MAX);
            }
        };

        struct BlendColorRGBA8:public bitmap::BlendColor<Vector4u8>
        {
            const Vector4u8 operator()(const Vector4u8 &src,const Vector4u8 &dst)const
            {
                uint8 na=255-src.a;

                return Vector4u8((src.r*src.a+dst.r*na)/255,
                                 (src.g*src.a+dst.g*na)/255,
                                 (src.b*src.a+dst.b*na)/255,
                                 dst.a);
            }

            const Vector4u8 operator()(const Vector4u8 &src,const Vector4u8 &dst,const float &alpha)const
            {
                uint8 a=src.a*alpha;
                uint8 na=255-src.a;

                return Vector4u8((src.r*src.a+dst.r*na)/255,
                                 (src.g*src.a+dst.g*na)/255,
                                 (src.b*src.a+dst.b*na)/255,
                                 dst.a);
            }
        };

        template<> void bitmap::BlendBitmap<BitmapRGBA8,BitmapRGB8>::operator()(const BitmapRGBA8 *src_bitmap,BitmapRGB8 *dst_bitmap,const float alpha)const
        {
            if(!src_bitmap||!dst_bitmap||alpha<=0)return;

            const uint width=src_bitmap->GetWidth();
            const uint height=src_bitmap->GetHeight();

            if(width!=dst_bitmap->GetWidth()||height!=dst_bitmap->GetHeight())
                return;

                  Vector3u8 *dst=dst_bitmap->GetData();
            const Vector4u8 *src=src_bitmap->GetData();

            float a;
            float na;

            for(uint i=0;i<width*height;i++)
            {
                a=src->a*alpha;
                na=255-src->a;

                dst->r=(src->r*a+dst->r*na)/255;
                dst->g=(src->g*a+dst->g*na)/255;
                dst->b=(src->b*a+dst->b*na)/255;

                ++dst;
                ++src;
            }
        }

        using BlendBitmapRGBA8toRGB8=bitmap::BlendBitmap<BitmapRGBA8,BitmapRGB8>;
    }//namespace bitmap
}//namespace hgl
