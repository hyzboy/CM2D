#pragma once

#include<hgl/CoreType.h>
#include<hgl/2d/Bitmap.h>
#include<vector>
#include<cmath>

namespace hgl::bitmap
{
    /**
     * 所有噪声生成器的抽象基类
     * 提供可设种子的伪随机生成，用于过程内容
     */
    class NoiseGenerator
    {
    protected:
        uint32 seed;

    public:
        NoiseGenerator(uint32 s = 12345) : seed(s) {}
        virtual ~NoiseGenerator() = default;

        /**
         * 在给定二维坐标处生成噪声值
         * @param x X坐标
         * @param y Y坐标
         * @return 噪声值，范围在[-1, 1]
         */
        virtual float Generate(float x, float y) const = 0;

        void SetSeed(uint32 s) { seed = s; }
        uint32 GetSeed() const { return seed; }
    };

    /**
     * 经典Perlin噪声实现
     * 使用置换表与基于梯度的插值
     */
    class PerlinNoise : public NoiseGenerator
    {
    private:
        std::vector<int> permutation;

        float Fade(float t) const;
        float Lerp(float t, float a, float b) const;
        float Grad(int hash, float x, float y) const;

    public:
        PerlinNoise(uint32 seed = 12345);
        virtual ~PerlinNoise() = default;

        float Generate(float x, float y) const override;
    };

    /**
     * Simplex噪声实现
     * 比Perlin更快，视觉质量更好且伪影更少
     */
    class SimplexNoise : public NoiseGenerator
    {
    private:
        std::vector<int> permutation;

        struct Grad
        {
            float x, y;
        };

        static const Grad gradients[8];

        int FastFloor(float x) const;
        float Dot(const Grad& g, float x, float y) const;

    public:
        SimplexNoise(uint32 seed = 12345);
        virtual ~SimplexNoise() = default;

        float Generate(float x, float y) const override;
    };

    /**
     * Voronoi/蜂窝噪声实现
     * 生成基于区域的模式，适用于生物群系和纹理
     */
    class VoronoiNoise : public NoiseGenerator
    {
    private:
        float Hash2D(float x, float y) const;
        void GetCellPoint(int ix, int iy, float& px, float& py) const;

    public:
        VoronoiNoise(uint32 seed = 12345);
        virtual ~VoronoiNoise() = default;

        float Generate(float x, float y) const override;
    };

    /**
     * 分形布朗运动（FBM）噪声
     * 通过组合多层噪声产生自然地形效果
     */
    class FractalNoise : public NoiseGenerator
    {
    private:
        NoiseGenerator* baseNoise;
        bool ownsNoise;
        int octaves;
        float lacunarity;
        float persistence;

    public:
        /**
         * @param noise 基础噪声生成器（如果ownNoise为true则由本对象拥有）
         * @param oct 八度数（细节层数）
         * @param lac 每层的频率倍数（通常为2.0）
         * @param pers 每层的振幅衰减（通常为0.5）
         * @param ownNoise 若为true，则在析构时删除噪声生成器
         */
        FractalNoise(NoiseGenerator* noise, int oct = 6, float lac = 2.0f,
                     float pers = 0.5f, bool ownNoise = true);
        virtual ~FractalNoise();

        float Generate(float x, float y) const override;

        void SetOctaves(int oct) { octaves = oct; }
        void SetLacunarity(float lac) { lacunarity = lac; }
        void SetPersistence(float pers) { persistence = pers; }

        int GetOctaves() const { return octaves; }
        float GetLacunarity() const { return lacunarity; }
        float GetPersistence() const { return persistence; }
    };

}//namespace hgl::bitmap
