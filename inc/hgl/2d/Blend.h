#pragma once

/**
 * @file Blend.h
 * @brief Flexible alpha blending system for bitmap operations
 * 
 * This file provides a template-based blending system that supports multiple
 * blend modes from CMMath library. It requires the following external dependencies:
 * - hgl::AlphaBlendMode enum from CMCoreType
 * - hgl::graph::GetAlphaBlendFunc*() functions from CMMath
 * - hgl::Color types from CMCoreType (Color3f, Color4f, Color3ub, Color4ub)
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/math/AlphaBlend.h>
#include<hgl/math/ScalarConversion.h>
#include<algorithm>

namespace hgl::bitmap
{
    // Import AlphaBlendMode from hgl::math namespace for convenience
    using math::AlphaBlendMode;

    // Import color conversion functions from hgl::math namespace
    using math::ClampU8;
    using math::ToColorFloat;
    using math::ToColorByte;

    // ==================== Blend Function Types ====================

    /**
        * @brief Function pointer type for alpha blending
        * @tparam T Data type (Color3f, Color4f, Color3ub, Color4ub, etc.)
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
        return math::GetAlphaBlendFunc(mode);
    }

    /**
        * @brief Specialization for Vector3f - uses CMMath functions directly
        */
    template<>
    inline BlendFunc<Color3f> BlendColor<Color3f>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return math::GetAlphaBlendFuncColor3f(mode);
    }

    /**
        * @brief Specialization for Vector4f - uses CMMath functions directly
        */
    template<>
    inline BlendFunc<Color4f> BlendColor<Color4f>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return math::GetAlphaBlendFuncColor4f(mode);
    }

    // ==================== Wrapper Functions for uint8 Types ====================

    namespace detail
    {
        // ==================== Generic Wrapper Template ====================

        /**
         * @brief Generic blend wrapper that adapts uint8 colors through float blending
         * @tparam T Color type (Color3ub or Color4ub)
         * @tparam F Float color type (Color3f or Color4f)
         * @tparam MODE Blend mode (compile-time constant)
         */
        template<typename T, typename F, AlphaBlendMode MODE>
        inline T BlendColorWrapper(const T &src, const T &dst, float alpha)
        {
            F src_f = ToColorFloat(src);
            F dst_f = ToColorFloat(dst);

            auto func = math::detail::GetAlphaBlendFuncImpl<F>(MODE);
            if (!func) return src;

            F result_f = func(src_f, dst_f, alpha);

            return ToColorByte(result_f);
        }

        // ==================== Mode Selector Template ====================

        /**
         * @brief Generic template to select blend wrapper by mode
         * @tparam T Color type
         * @tparam F Float color type
         */
        template<typename T, typename F>
        inline void* GetWrapperForMode(AlphaBlendMode mode)
        {
            switch (mode)
            {
                case AlphaBlendMode::None:              return (void*)BlendColorWrapper<T, F, AlphaBlendMode::None>;
                case AlphaBlendMode::Normal:            return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Normal>;
                case AlphaBlendMode::Add:               return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Add>;
                case AlphaBlendMode::Subtract:          return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Subtract>;
                case AlphaBlendMode::Multiply:          return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Multiply>;
                case AlphaBlendMode::Screen:            return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Screen>;
                case AlphaBlendMode::Overlay:           return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Overlay>;
                case AlphaBlendMode::HardLight:         return (void*)BlendColorWrapper<T, F, AlphaBlendMode::HardLight>;
                case AlphaBlendMode::SoftLight:         return (void*)BlendColorWrapper<T, F, AlphaBlendMode::SoftLight>;
                case AlphaBlendMode::ColorDodge:        return (void*)BlendColorWrapper<T, F, AlphaBlendMode::ColorDodge>;
                case AlphaBlendMode::ColorBurn:         return (void*)BlendColorWrapper<T, F, AlphaBlendMode::ColorBurn>;
                case AlphaBlendMode::LinearDodge:       return (void*)BlendColorWrapper<T, F, AlphaBlendMode::LinearDodge>;
                case AlphaBlendMode::LinearBurn:        return (void*)BlendColorWrapper<T, F, AlphaBlendMode::LinearBurn>;
                case AlphaBlendMode::Darken:            return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Darken>;
                case AlphaBlendMode::Lighten:           return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Lighten>;
                case AlphaBlendMode::Difference:        return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Difference>;
                case AlphaBlendMode::Exclusion:         return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Exclusion>;
                case AlphaBlendMode::VividLight:        return (void*)BlendColorWrapper<T, F, AlphaBlendMode::VividLight>;
                case AlphaBlendMode::LinearLight:       return (void*)BlendColorWrapper<T, F, AlphaBlendMode::LinearLight>;
                case AlphaBlendMode::PinLight:          return (void*)BlendColorWrapper<T, F, AlphaBlendMode::PinLight>;
                case AlphaBlendMode::HardMix:           return (void*)BlendColorWrapper<T, F, AlphaBlendMode::HardMix>;
                case AlphaBlendMode::DarkerColor:       return (void*)BlendColorWrapper<T, F, AlphaBlendMode::DarkerColor>;
                case AlphaBlendMode::LighterColor:      return (void*)BlendColorWrapper<T, F, AlphaBlendMode::LighterColor>;
                case AlphaBlendMode::Hue:               return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Hue>;
                case AlphaBlendMode::Saturation:        return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Saturation>;
                case AlphaBlendMode::Color:             return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Color>;
                case AlphaBlendMode::Luminosity:        return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Luminosity>;
                case AlphaBlendMode::Divide:            return (void*)BlendColorWrapper<T, F, AlphaBlendMode::Divide>;
                case AlphaBlendMode::PremultipliedAlpha:return (void*)BlendColorWrapper<T, F, AlphaBlendMode::PremultipliedAlpha>;
                default: return nullptr;
            }
        }
    }

    /**
     * @brief Specialization for Color3ub - fully generic via trait system
     */
    template<>
    inline BlendFunc<Color3ub> BlendColor<Color3ub>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return reinterpret_cast<BlendFunc<Color3ub>>(detail::GetWrapperForMode<Color3ub, Color3f>(mode));
    }

    /**
     * @brief Specialization for Color4ub - fully generic via trait system
     */
    template<>
    inline BlendFunc<Color4ub> BlendColor<Color4ub>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return reinterpret_cast<BlendFunc<Color4ub>>(detail::GetWrapperForMode<Color4ub, Color4f>(mode));
    }

    // ==================== Convenience Type Aliases ====================

    using BlendColorRGB8 = BlendColor<Color3ub>;
    using BlendColorRGBA8 = BlendColor<Color4ub>;
    using BlendColorRGB32F = BlendColor<Color3f>;
    using BlendColorRGBA32F = BlendColor<Color4f>;

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

        auto *dst = dst_bitmap->GetData();
        auto *src = src_bitmap->GetData();

        // Use default Normal blend mode for standard alpha blending
        // Static is safe here - const and thread-safe initialization in C++11+
        static const BlendColorRGBA8 blend_color;

        for (uint i = 0; i < width * height; i++)
        {
            // Scale source alpha by global alpha
            Color4ub src_scaled = *src;
            src_scaled.a = ClampU8(static_cast<float>(src->a) * alpha);

            // Convert RGB8 dst to RGBA8 for blending
            Color4ub dst_rgba(dst->r, dst->g, dst->b, 255);
                
            // Perform blending
            Color4ub blended = blend_color(src_scaled, dst_rgba);

            // Write back RGB components
            dst->r = blended.r;
            dst->g = blended.g;
            dst->b = blended.b;

            ++dst;
            ++src;
        }
    }

    using BlendBitmapRGBA8toRGB8 = BlendBitmap<BitmapRGBA8, BitmapRGB8>;
}//namespace hgl::bitmap
