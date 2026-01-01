#pragma once

#include<hgl/CoreType.h>
#include<hgl/2d/Bitmap.h>
#include<hgl/math/AlphaBlend.h>
#include<algorithm>

namespace hgl
{
    namespace bitmap
    {
        // ==================== Helper Functions ====================

        /**
         * @brief Clamp a float value to uint8 range [0, 255]
         * @param value Float value to clamp
         * @return Clamped uint8 value
         */
        inline uint8 ClampByte(float value)
        {
            if (value <= 0.0f) return 0;
            if (value >= 255.0f) return 255;
            return static_cast<uint8>(value + 0.5f);
        }

        /**
         * @brief Convert uint8 [0,255] to float [0,1]
         */
        inline float ByteToFloat(uint8 b)
        {
            return b / 255.0f;
        }

        /**
         * @brief Convert float [0,1] to uint8 [0,255] with clamping
         */
        inline uint8 FloatToByte(float f)
        {
            return ClampByte(f * 255.0f);
        }

        // ==================== Blend Function Types ====================

        /**
         * @brief Function pointer type for alpha blending
         * @tparam T Data type (Vector3f, Vector4f, Vector3u8, Vector4u8, etc.)
         */
        template<typename T>
        using BlendFunc = T (*)(const T &src, const T &dst, float alpha);

        // ==================== BlendColor Template Class ====================

        /**
         * @brief Template class for flexible alpha blending
         * Uses function pointers instead of virtual functions for better performance
         * @tparam T Data type for color values
         */
        template<typename T>
        class BlendColor
        {
        protected:
            BlendFunc<T> blend_func;

        private:
            /**
             * @brief Get blend function for the specified mode and type
             * @param mode Alpha blend mode
             * @return Function pointer for the blend operation
             */
            BlendFunc<T> GetBlendFuncForType(AlphaBlendMode mode);

        public:
            /**
             * @brief Default constructor - uses Normal blend mode
             */
            BlendColor()
            {
                SetBlendMode(AlphaBlendMode::Normal);
            }

            /**
             * @brief Constructor with custom blend function
             * @param func Custom blend function pointer
             */
            explicit BlendColor(BlendFunc<T> func) : blend_func(func)
            {
            }

            /**
             * @brief Set custom blend function
             * @param func Custom blend function pointer
             */
            void SetBlendFunc(BlendFunc<T> func)
            {
                blend_func = func;
            }

            /**
             * @brief Set blend mode from AlphaBlendMode enum
             * @param mode Alpha blend mode
             */
            void SetBlendMode(AlphaBlendMode mode)
            {
                blend_func = GetBlendFuncForType(mode);
            }

            /**
             * @brief Blend operator with default alpha=1.0
             * @param src Source color
             * @param dst Destination color
             * @return Blended color
             */
            T operator()(const T &src, const T &dst) const
            {
                return operator()(src, dst, 1.0f);
            }

            /**
             * @brief Blend operator with custom alpha
             * @param src Source color
             * @param dst Destination color
             * @param alpha Blend factor [0,1]
             * @return Blended color
             */
            T operator()(const T &src, const T &dst, float alpha) const
            {
                if (blend_func)
                    return blend_func(src, dst, alpha);
                return src;
            }
        };//template<typename T> class BlendColor

        // ==================== Template Specializations for GetBlendFuncForType ====================

        /**
         * @brief Specialization for float type - uses CMMath functions directly
         */
        template<>
        inline BlendFunc<float> BlendColor<float>::GetBlendFuncForType(AlphaBlendMode mode)
        {
            return graph::GetAlphaBlendFunc(mode);
        }

        /**
         * @brief Specialization for Vector3f - uses CMMath functions directly
         */
        template<>
        inline BlendFunc<math::Vector3f> BlendColor<math::Vector3f>::GetBlendFuncForType(AlphaBlendMode mode)
        {
            return graph::GetAlphaBlendFuncVec3f(mode);
        }

        /**
         * @brief Specialization for Vector4f - uses CMMath functions directly
         */
        template<>
        inline BlendFunc<math::Vector4f> BlendColor<math::Vector4f>::GetBlendFuncForType(AlphaBlendMode mode)
        {
            return graph::GetAlphaBlendFuncVec4f(mode);
        }

        // ==================== Wrapper Functions for uint8 Types ====================

        // Helper wrapper functions for Vector3u8
        namespace detail
        {
            template<AlphaBlendMode MODE>
            inline math::Vector3u8 BlendVec3u8Wrapper(const math::Vector3u8 &src, const math::Vector3u8 &dst, float alpha)
            {
                math::Vector3f src_f(ByteToFloat(src.r), ByteToFloat(src.g), ByteToFloat(src.b));
                math::Vector3f dst_f(ByteToFloat(dst.r), ByteToFloat(dst.g), ByteToFloat(dst.b));
                
                auto func = graph::GetAlphaBlendFuncVec3f(MODE);
                if (!func) return src;
                
                math::Vector3f result_f = func(src_f, dst_f, alpha);
                
                math::Vector3u8 result;
                result.r = FloatToByte(result_f.r);
                result.g = FloatToByte(result_f.g);
                result.b = FloatToByte(result_f.b);
                return result;
            }

            template<AlphaBlendMode MODE>
            inline math::Vector4u8 BlendVec4u8Wrapper(const math::Vector4u8 &src, const math::Vector4u8 &dst, float alpha)
            {
                math::Vector4f src_f(ByteToFloat(src.r), ByteToFloat(src.g), ByteToFloat(src.b), ByteToFloat(src.a));
                math::Vector4f dst_f(ByteToFloat(dst.r), ByteToFloat(dst.g), ByteToFloat(dst.b), ByteToFloat(dst.a));
                
                auto func = graph::GetAlphaBlendFuncVec4f(MODE);
                if (!func) return src;
                
                math::Vector4f result_f = func(src_f, dst_f, alpha);
                
                math::Vector4u8 result;
                result.r = FloatToByte(result_f.r);
                result.g = FloatToByte(result_f.g);
                result.b = FloatToByte(result_f.b);
                result.a = FloatToByte(result_f.a);
                return result;
            }
        }

        /**
         * @brief Specialization for Vector3u8 - converts uint8 to float, blends, converts back
         */
        template<>
        inline BlendFunc<math::Vector3u8> BlendColor<math::Vector3u8>::GetBlendFuncForType(AlphaBlendMode mode)
        {
            switch (mode)
            {
                case AlphaBlendMode::Normal:        return detail::BlendVec3u8Wrapper<AlphaBlendMode::Normal>;
                case AlphaBlendMode::Add:           return detail::BlendVec3u8Wrapper<AlphaBlendMode::Add>;
                case AlphaBlendMode::Subtract:      return detail::BlendVec3u8Wrapper<AlphaBlendMode::Subtract>;
                case AlphaBlendMode::Multiply:      return detail::BlendVec3u8Wrapper<AlphaBlendMode::Multiply>;
                case AlphaBlendMode::Screen:        return detail::BlendVec3u8Wrapper<AlphaBlendMode::Screen>;
                case AlphaBlendMode::Overlay:       return detail::BlendVec3u8Wrapper<AlphaBlendMode::Overlay>;
                case AlphaBlendMode::HardLight:     return detail::BlendVec3u8Wrapper<AlphaBlendMode::HardLight>;
                case AlphaBlendMode::SoftLight:     return detail::BlendVec3u8Wrapper<AlphaBlendMode::SoftLight>;
                case AlphaBlendMode::ColorDodge:    return detail::BlendVec3u8Wrapper<AlphaBlendMode::ColorDodge>;
                case AlphaBlendMode::ColorBurn:     return detail::BlendVec3u8Wrapper<AlphaBlendMode::ColorBurn>;
                case AlphaBlendMode::LinearDodge:   return detail::BlendVec3u8Wrapper<AlphaBlendMode::LinearDodge>;
                case AlphaBlendMode::LinearBurn:    return detail::BlendVec3u8Wrapper<AlphaBlendMode::LinearBurn>;
                case AlphaBlendMode::Darken:        return detail::BlendVec3u8Wrapper<AlphaBlendMode::Darken>;
                case AlphaBlendMode::Lighten:       return detail::BlendVec3u8Wrapper<AlphaBlendMode::Lighten>;
                case AlphaBlendMode::Difference:    return detail::BlendVec3u8Wrapper<AlphaBlendMode::Difference>;
                case AlphaBlendMode::Exclusion:     return detail::BlendVec3u8Wrapper<AlphaBlendMode::Exclusion>;
                case AlphaBlendMode::PremultipliedAlpha: return detail::BlendVec3u8Wrapper<AlphaBlendMode::PremultipliedAlpha>;
                default: return nullptr;
            }
        }

        /**
         * @brief Specialization for Vector4u8 - converts uint8 to float, blends, converts back
         */
        template<>
        inline BlendFunc<math::Vector4u8> BlendColor<math::Vector4u8>::GetBlendFuncForType(AlphaBlendMode mode)
        {
            switch (mode)
            {
                case AlphaBlendMode::Normal:        return detail::BlendVec4u8Wrapper<AlphaBlendMode::Normal>;
                case AlphaBlendMode::Add:           return detail::BlendVec4u8Wrapper<AlphaBlendMode::Add>;
                case AlphaBlendMode::Subtract:      return detail::BlendVec4u8Wrapper<AlphaBlendMode::Subtract>;
                case AlphaBlendMode::Multiply:      return detail::BlendVec4u8Wrapper<AlphaBlendMode::Multiply>;
                case AlphaBlendMode::Screen:        return detail::BlendVec4u8Wrapper<AlphaBlendMode::Screen>;
                case AlphaBlendMode::Overlay:       return detail::BlendVec4u8Wrapper<AlphaBlendMode::Overlay>;
                case AlphaBlendMode::HardLight:     return detail::BlendVec4u8Wrapper<AlphaBlendMode::HardLight>;
                case AlphaBlendMode::SoftLight:     return detail::BlendVec4u8Wrapper<AlphaBlendMode::SoftLight>;
                case AlphaBlendMode::ColorDodge:    return detail::BlendVec4u8Wrapper<AlphaBlendMode::ColorDodge>;
                case AlphaBlendMode::ColorBurn:     return detail::BlendVec4u8Wrapper<AlphaBlendMode::ColorBurn>;
                case AlphaBlendMode::LinearDodge:   return detail::BlendVec4u8Wrapper<AlphaBlendMode::LinearDodge>;
                case AlphaBlendMode::LinearBurn:    return detail::BlendVec4u8Wrapper<AlphaBlendMode::LinearBurn>;
                case AlphaBlendMode::Darken:        return detail::BlendVec4u8Wrapper<AlphaBlendMode::Darken>;
                case AlphaBlendMode::Lighten:       return detail::BlendVec4u8Wrapper<AlphaBlendMode::Lighten>;
                case AlphaBlendMode::Difference:    return detail::BlendVec4u8Wrapper<AlphaBlendMode::Difference>;
                case AlphaBlendMode::Exclusion:     return detail::BlendVec4u8Wrapper<AlphaBlendMode::Exclusion>;
                case AlphaBlendMode::PremultipliedAlpha: return detail::BlendVec4u8Wrapper<AlphaBlendMode::PremultipliedAlpha>;
                default: return nullptr;
            }
        }

        // ==================== Convenience Type Aliases ====================

        using BlendColorRGB8 = BlendColor<math::Vector3u8>;
        using BlendColorRGBA8 = BlendColor<math::Vector4u8>;
        using BlendColorRGB32F = BlendColor<math::Vector3f>;
        using BlendColorRGBA32F = BlendColor<math::Vector4f>;

        // ==================== BlendBitmap Template Class ====================

        /**
         * @brief Bitmap blending template class
         * Declares the blending operator but doesn't implement it generically.
         * Each specific bitmap combination needs its own specialization.
         * @tparam ST Source bitmap type
         * @tparam DT Destination bitmap type
         */
        template<typename ST, typename DT>
        class BlendBitmap
        {
        public:
            /**
             * @brief Blend operator - blends source bitmap onto destination
             * @param src Source bitmap
             * @param dst Destination bitmap
             * @param alpha Global alpha value [0,1]
             */
            void operator()(const ST *src, DT *dst, const float alpha) const;
        };//template<typename ST,typename DT> class BlendBitmap

        // ==================== Legacy Compatibility Classes ====================

        /**
         * @brief Legacy additive blending for uint32
         * Kept for backward compatibility
         */
        struct BlendColorU32Additive
        {
            const uint32 operator()(const uint32 &src, const uint32 &dst) const
            {
                uint64 result = src + dst;
                return (result > HGL_U32_MAX) ? HGL_U32_MAX : (result & HGL_U32_MAX);
            }

            const uint32 operator()(const uint32 &src, const uint32 &dst, const float &alpha) const
            {
                uint64 result = src * alpha + dst;
                return (result > HGL_U32_MAX) ? HGL_U32_MAX : (result & HGL_U32_MAX);
            }
        };

        // ==================== Specialized BlendBitmap Implementation ====================

        /**
         * @brief Specialization for RGBA8 to RGB8 blending
         * Uses Normal blend mode by default (standard alpha blending)
         */
        template<>
        inline void BlendBitmap<BitmapRGBA8, BitmapRGB8>::operator()(const BitmapRGBA8 *src_bitmap, BitmapRGB8 *dst_bitmap, const float alpha) const
        {
            if (!src_bitmap || !dst_bitmap || alpha <= 0) return;

            const uint width = src_bitmap->GetWidth();
            const uint height = src_bitmap->GetHeight();

            if (width != dst_bitmap->GetWidth() || height != dst_bitmap->GetHeight())
                return;

            Vector3u8 *dst = dst_bitmap->GetData();
            const Vector4u8 *src = src_bitmap->GetData();

            // Use default Normal blend mode for standard alpha blending
            static BlendColorRGBA8 blend_color;

            for (uint i = 0; i < width * height; i++)
            {
                // Scale source alpha by global alpha
                Vector4u8 src_scaled = *src;
                src_scaled.a = ClampByte(static_cast<float>(src->a) * alpha);

                // Convert RGB8 dst to RGBA8 for blending
                Vector4u8 dst_rgba(dst->r, dst->g, dst->b, 255);
                
                // Perform blending
                Vector4u8 blended = blend_color(src_scaled, dst_rgba);

                // Write back RGB components
                dst->r = blended.r;
                dst->g = blended.g;
                dst->b = blended.b;

                ++dst;
                ++src;
            }
        }

        using BlendBitmapRGBA8toRGB8 = BlendBitmap<BitmapRGBA8, BitmapRGB8>;
    }//namespace bitmap
}//namespace hgl
