#ifdef __APPLE__

#include<hgl/2d/BitmapPlatform.h>
#include<CoreGraphics/CoreGraphics.h>
#include<ApplicationServices/ApplicationServices.h>
#include<cstring>

namespace hgl::bitmap
{
    template<typename T, uint C>
    BitmapMacOS<T, C>::BitmapMacOS()
        : Bitmap<T, C>()
    {
        cgContext = nullptr;
        colorSpace = nullptr;
    }

    template<typename T, uint C>
    BitmapMacOS<T, C>::~BitmapMacOS()
    {
        // 必须在释放平台资源前将data设为nullptr，防止基类重复释放
        this->data = nullptr;

        if (cgContext)
        {
            CGContextRelease(cgContext);
            cgContext = nullptr;
        }

        if (colorSpace)
        {
            CGColorSpaceRelease(colorSpace);
            colorSpace = nullptr;
        }
    }

    template<typename T, uint C>
    bool BitmapMacOS<T, C>::CreateCGContext(uint w, uint h)
    {
        if (!w || !h)
            return false;

        // 清理旧资源
        if (cgContext)
        {
            CGContextRelease(cgContext);
            cgContext = nullptr;
        }

        if (colorSpace)
        {
            CGColorSpaceRelease(colorSpace);
            colorSpace = nullptr;
        }

        this->data = nullptr;
        this->width = w;
        this->height = h;

        // 创建颜色空间
        if (C >= 3)
            colorSpace = CGColorSpaceCreateDeviceRGB();
        else
            colorSpace = CGColorSpaceCreateDeviceGray();

        if (!colorSpace)
            return false;

        // 计算参数
        size_t bytesPerRow = w * sizeof(T);
        size_t bitsPerComponent = 8;
        CGBitmapInfo bitmapInfo = 0;

        // 根据通道数设置bitmap info
        if (C == 4)
        {
            // RGBA - 假设是RGBA格式
            bitmapInfo = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
        }
        else if (C == 3)
        {
            // RGB
            bitmapInfo = kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big;
        }
        else if (C == 2)
        {
            // RG - 灰度+Alpha
            bitmapInfo = kCGImageAlphaPremultipliedLast;
        }
        else
        {
            // 灰度
            bitmapInfo = kCGImageAlphaNone;
        }

        // 分配内存并创建context
        size_t dataSize = w * h * sizeof(T);
        void* bitmapData = malloc(dataSize);
        if (!bitmapData)
        {
            CGColorSpaceRelease(colorSpace);
            colorSpace = nullptr;
            return false;
        }

        memset(bitmapData, 0, dataSize);

        cgContext = CGBitmapContextCreate(
            bitmapData,
            w,
            h,
            bitsPerComponent,
            bytesPerRow,
            colorSpace,
            bitmapInfo
        );

        if (!cgContext)
        {
            free(bitmapData);
            CGColorSpaceRelease(colorSpace);
            colorSpace = nullptr;
            return false;
        }

        // 将基类的data指针指向bitmap数据
        this->data = (T*)bitmapData;

        return true;
    }

    template<typename T, uint C>
    CGImageRef BitmapMacOS<T, C>::CreateCGImage() const
    {
        if (!cgContext || !this->data)
            return nullptr;

        return CGBitmapContextCreateImage(cgContext);
    }

    template<typename T, uint C>
    bool BitmapMacOS<T, C>::DrawToContext(CGContextRef destContext, CGRect destRect) const
    {
        if (!destContext || !cgContext || !this->data)
            return false;

        CGImageRef image = CreateCGImage();
        if (!image)
            return false;

        CGContextDrawImage(destContext, destRect, image);
        CGImageRelease(image);

        return true;
    }

    template<typename T, uint C>
    bool BitmapMacOS<T, C>::DrawToContext(CGContextRef destContext, CGRect destRect, CGRect sourceRect) const
    {
        if (!destContext || !cgContext || !this->data)
            return false;

        CGImageRef fullImage = CreateCGImage();
        if (!fullImage)
            return false;

        // 裁剪源区域
        CGImageRef croppedImage = CGImageCreateWithImageInRect(fullImage, sourceRect);
        CGImageRelease(fullImage);

        if (!croppedImage)
            return false;

        CGContextDrawImage(destContext, destRect, croppedImage);
        CGImageRelease(croppedImage);

        return true;
    }

    // 显式实例化常用类型
    template class BitmapMacOS<math::Vector4u8, 4>;
    template class BitmapMacOS<math::Vector3u8, 3>;
    template class BitmapMacOS<math::Vector2u8, 2>;
    template class BitmapMacOS<uint8, 1>;

} // namespace hgl::bitmap

#endif // __APPLE__
