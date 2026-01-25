#pragma once

#include<hgl/CoreType.h>
#include<hgl/2d/Bitmap.h>
#include<hgl/2d/NoiseMap.h>

namespace hgl::bitmap
{
    /**
     * 地形高程数据的HeightMap类
     * 继承自Bitmap32F（浮点位图），用于存储高度信息
     */
    class HeightMap : public Bitmap32F
    {
    public:
        HeightMap() : Bitmap32F() {}
        virtual ~HeightMap() = default;

        /**
         * 通过噪声生成器生成高程图
         * @param noise 使用的噪声生成器
         * @param scale 噪声坐标缩放因子
         * @param offsetX 噪声采样的X偏移
         * @param offsetY 噪声采样的Y偏移
         */
        void GenerateFromNoise(const NoiseGenerator& noise, float scale = 1.0f,
                              float offsetX = 0.0f, float offsetY = 0.0f);

        /**
         * 将高度值归一化到指定范围
         * @param minHeight 最小高度值
         * @param maxHeight 最大高度值
         */
        void Normalize(float minHeight = 0.0f, float maxHeight = 1.0f);

        /**
         * 计算坡度/梯度图
         * @param slopeMap 输出坡度图（值范围0-1，1为最陡）
         */
        void CalculateSlopeMap(Bitmap32F& slopeMap) const;

        /**
         * 应用热力侵蚀模拟
         * @param iterations 侵蚀迭代次数
         * @param talusAngle 安息角（典型值：0.5-0.9）
         */
        void ApplyThermalErosion(int iterations, float talusAngle = 0.7f);

        /**
         * 应用简化的水蚀模拟
         * @param iterations 侵蚀迭代次数
         * @param strength 侵蚀强度（0-1）
         */
        void ApplyHydraulicErosion(int iterations, float strength = 0.1f);
    };

    /**
     * 地形分类的生物群落类型
     */
    enum class BiomeType : uint8
    {
        Ocean = 0,
        Beach = 1,
        Plains = 2,
        Forest = 3,
        Desert = 4,
        Tundra = 5,
        Snow = 6,
        Mountain = 7,
        River = 8
    };

    /**
     * 生物群落分布的BiomeMap类
     * 继承自BitmapGrey8（8位灰度图），每个像素值代表一种生物群落类型
     */
    class BiomeMap : public BitmapGrey8
    {
    public:
        BiomeMap() : BitmapGrey8() {}
        virtual ~BiomeMap() = default;

        /**
         * 根据高度、温度和湿度图生成生物群落分布图
         * @param heightMap 高度/高程数据
         * @param temperatureMap 温度数据（可选，可为nullptr）
         * @param moistureMap 湿度数据（可选，可为nullptr）
         */
        void GenerateFromMaps(const HeightMap& heightMap,
                             const Bitmap32F* temperatureMap = nullptr,
                             const Bitmap32F* moistureMap = nullptr);

        /**
         * 获取指定坐标的生物群落类型
         * @param x X坐标
         * @param y Y坐标
         * @return 指定位置的BiomeType
         */
        BiomeType GetBiome(int x, int y) const;

        /**
         * 设置指定坐标的生物群落类型
         * @param x X坐标
         * @param y Y坐标
         * @param biome 要设置的BiomeType
         */
        void SetBiome(int x, int y, BiomeType biome);
    };

    /**
     * 高层次地形生成器，负责整体生成流程
     */
    class TerrainGenerator
    {
    private:
        uint32 seed;
        int width;
        int height;

    public:
        TerrainGenerator(int w, int h, uint32 s = 12345)
            : width(w), height(h), seed(s) {}

        /**
         * 快速生成地形（不包含侵蚀）
         * @param heightMap 输出的高程图
         * @param scale 噪声缩放因子
         * @param octaves FBM分形噪声的阶数
         */
        void GenerateQuick(HeightMap& heightMap, float scale = 1.0f, int octaves = 6);

        /**
         * 生成包含侵蚀模拟的详细地形
         * @param heightMap 输出的高程图
         * @param scale 噪声缩放因子
         * @param octaves FBM分形噪声的阶数
         * @param erosionIterations 侵蚀迭代次数
         */
        void GenerateDetailed(HeightMap& heightMap, float scale = 1.0f,
                            int octaves = 6, int erosionIterations = 100);

        /**
         * 生成生物群落分布图
         * @param biomeMap 输出的生物群落图
         * @param heightMap 输入的高程图
         * @param tempScale 温度噪声缩放因子
         * @param moistScale 湿度噪声缩放因子
         */
        void GenerateBiomes(BiomeMap& biomeMap, const HeightMap& heightMap,
                           float tempScale = 0.5f, float moistScale = 0.5f);

        void SetSeed(uint32 s) { seed = s; }
        uint32 GetSeed() const { return seed; }
    };

}//namespace hgl::bitmap
