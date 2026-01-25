#pragma once

/**
 * @file Blend.h
 * @brief 用于位图操作的灵活 Alpha 混合系统
 *
 * 本文件提供了一个基于模板的混合系统，支持来自 CMMath 库的多种混合模式。
 * 依赖以下外部内容：
 * - hgl::AlphaBlendMode 枚举（来自 CMCoreType）
 * - hgl::graph::GetAlphaBlendFunc*() 系列函数（来自 CMMath）
 * - hgl::Color 类型（Color3f、Color4f、Color3ub、Color4ub，来自 CMCoreType）
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

    // ==================== 混合函数类型 ====================

    /**
        * @brief Alpha 混合的函数指针类型
        * @tparam T 数据类型（Color3f、Color4f、Color3ub、Color4ub 等）
        */
    template<typename T>
    using BlendFunc = T (*)(const T &src, const T &dst, float alpha);

    // ==================== BlendColor 模板类 ====================

    /**
        * @brief 灵活 Alpha 混合的模板类
        * 使用函数指针而非虚函数以获得更高性能
        * @tparam T 颜色值的数据类型
        */
    template<typename T>
    class BlendColor
    {
    protected:
        BlendFunc<T> blend_func;

    private:
        /**
            * @brief 获取指定混合模式和类型的混合函数
            * @param mode Alpha 混合模式
            * @return 混合操作的函数指针
            */
        BlendFunc<T> GetBlendFuncForType(AlphaBlendMode mode);

    public:
        /**
            * @brief 默认构造函数 - 使用 Normal 混合模式
            */
        BlendColor()
        {
            SetBlendMode(AlphaBlendMode::Normal);
        }

        /**
            * @brief 使用自定义混合函数的构造函数
            * @param func 自定义混合函数指针
            */
        explicit BlendColor(BlendFunc<T> func) : blend_func(func)
        {
        }

        /**
            * @brief 设置自定义混合函数
            * @param func 自定义混合函数指针
            */
        void SetBlendFunc(BlendFunc<T> func)
        {
            blend_func = func;
        }

        /**
            * @brief 通过 AlphaBlendMode 枚举设置混合模式
            * @param mode Alpha 混合模式
            */
        void SetBlendMode(AlphaBlendMode mode)
        {
            blend_func = GetBlendFuncForType(mode);
        }

        /**
            * @brief 混合操作符，默认 alpha=1.0
            * @param src 源颜色
            * @param dst 目标颜色
            * @return 混合后的颜色
            */
        T operator()(const T &src, const T &dst) const
        {
            return operator()(src, dst, 1.0f);
        }

        /**
            * @brief 混合操作符，带自定义 alpha
            * @param src 源颜色
            * @param dst 目标颜色
            * @param alpha 混合因子 [0,1]
            * @return 混合后的颜色
            */
        T operator()(const T &src, const T &dst, float alpha) const
        {
            if (blend_func)
                return blend_func(src, dst, alpha);
            return src;
        }
    };//template<typename T> class BlendColor

    // ==================== GetBlendFuncForType 模板特化 ====================

    /**
        * @brief float 类型的特化 - 直接使用 CMMath 函数
        */
    template<>
    inline BlendFunc<float> BlendColor<float>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return math::GetAlphaBlendFunc(mode);
    }

    /**
        * @brief Vector3f 类型的特化 - 直接使用 CMMath 函数
        */
    template<>
    inline BlendFunc<Color3f> BlendColor<Color3f>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return math::GetAlphaBlendFuncColor3f(mode);
    }

    /**
        * @brief Vector4f 类型的特化 - 直接使用 CMMath 函数
        */
    template<>
    inline BlendFunc<Color4f> BlendColor<Color4f>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return math::GetAlphaBlendFuncColor4f(mode);
    }

    // ==================== uint8 类型的包装函数 ====================

    namespace detail
    {
        // ==================== 通用包装模板 ====================

        /**
         * @brief 通用混合包装器，将 uint8 颜色通过 float 混合
         * @tparam T 颜色类型（Color3ub 或 Color4ub）
         * @tparam F 浮点颜色类型（Color3f 或 Color4f）
         * @tparam MODE 混合模式（编译期常量）
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

        // ==================== 模式选择模板 ====================

        /**
         * @brief 通用模板，根据混合模式选择包装器
         * @tparam T 颜色类型
         * @tparam F 浮点颜色类型
         */
        template<typename T, typename F>
        inline void* GetWrapperForMode(AlphaBlendMode mode)
        {
            #define HGL_BLEND_CASE(MODE) case AlphaBlendMode::MODE: return (void*)BlendColorWrapper<T, F, AlphaBlendMode::MODE>
            switch (mode)
            {
                HGL_BLEND_CASE(None);
                HGL_BLEND_CASE(Normal);
                HGL_BLEND_CASE(Add);
                HGL_BLEND_CASE(Subtract);
                HGL_BLEND_CASE(Multiply);
                HGL_BLEND_CASE(Screen);
                HGL_BLEND_CASE(Overlay);
                HGL_BLEND_CASE(HardLight);
                HGL_BLEND_CASE(SoftLight);
                HGL_BLEND_CASE(ColorDodge);
                HGL_BLEND_CASE(ColorBurn);
                HGL_BLEND_CASE(LinearDodge);
                HGL_BLEND_CASE(LinearBurn);
                HGL_BLEND_CASE(Darken);
                HGL_BLEND_CASE(Lighten);
                HGL_BLEND_CASE(Difference);
                HGL_BLEND_CASE(Exclusion);
                HGL_BLEND_CASE(VividLight);
                HGL_BLEND_CASE(LinearLight);
                HGL_BLEND_CASE(PinLight);
                HGL_BLEND_CASE(HardMix);
                HGL_BLEND_CASE(DarkerColor);
                HGL_BLEND_CASE(LighterColor);
                HGL_BLEND_CASE(Hue);
                HGL_BLEND_CASE(Saturation);
                HGL_BLEND_CASE(Color);
                HGL_BLEND_CASE(Luminosity);
                HGL_BLEND_CASE(Divide);
                HGL_BLEND_CASE(PremultipliedAlpha);
                default: return nullptr;
            }
            #undef HGL_BLEND_CASE
        }
    }

    /**
     * @brief Color3ub 类型的特化 - 通过 trait 系统实现完全通用
     */
    template<>
    inline BlendFunc<Color3ub> BlendColor<Color3ub>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return reinterpret_cast<BlendFunc<Color3ub>>(detail::GetWrapperForMode<Color3ub, Color3f>(mode));
    }

    /**
     * @brief Color4ub 类型的特化 - 通过 trait 系统实现完全通用
     */
    template<>
    inline BlendFunc<Color4ub> BlendColor<Color4ub>::GetBlendFuncForType(AlphaBlendMode mode)
    {
        return reinterpret_cast<BlendFunc<Color4ub>>(detail::GetWrapperForMode<Color4ub, Color4f>(mode));
    }

    // ==================== 便捷类型别名 ====================

    using BlendColorRGB8 = BlendColor<Color3ub>;
    using BlendColorRGBA8 = BlendColor<Color4ub>;
    using BlendColorRGB32F = BlendColor<Color3f>;
    using BlendColorRGBA32F = BlendColor<Color4f>;

    // ==================== BlendBitmap 模板类 ====================

    /**
        * @brief 位图混合模板类
        * 仅声明混合操作符，不做通用实现。
        * 每种具体位图组合需专门特化。
        * @tparam ST 源位图类型
        * @tparam DT 目标位图类型
        */
    template<typename ST, typename DT>
    class BlendBitmap
    {
    public:
        /**
            * @brief 混合操作符 - 将源位图混合到目标位图
            * @param src 源位图
            * @param dst 目标位图
            * @param alpha 全局 alpha 值 [0,1]
            */
        void operator()(const ST *src, DT *dst, const float alpha) const;
    };//template<typename ST,typename DT> class BlendBitmap

    // ==================== 兼容性旧类 ====================

    /**
        * @brief 兼容旧版的 uint32 加法混合
        * 保留用于向后兼容
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

    // ==================== BlendBitmap 专用实现 ====================

    /**
        * @brief RGBA8 到 RGB8 的专用混合实现
        * 默认使用 Normal 混合模式（标准 alpha 混合）
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
