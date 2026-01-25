#ifdef __APPLE__

#include<hgl/2d/BitmapPlatform.h>
#include<CoreGraphics/CoreGraphics.h>
#include<ApplicationServices/ApplicationServices.h>
#include<cstring>
#include<cstdlib>

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
        // Save data pointer for cleanup
        void* dataToFree = this->data;

        // Must set data to nullptr before releasing platform resources to prevent base class double-free
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

        // Free malloc-allocated memory
        if (dataToFree)
        {
            free(dataToFree);
        }
    }

    template<typename T, uint C>
    bool BitmapMacOS<T, C>::CreateCGContext(uint w, uint h)
    {
        if (!w || !h)
            return false;

        // Clean up old resources
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

        // Create color space
        if (C >= 3)
            colorSpace = CGColorSpaceCreateDeviceRGB();
        else
            colorSpace = CGColorSpaceCreateDeviceGray();

        if (!colorSpace)
            return false;

        // Calculate parameters
        size_t bytesPerRow = w * sizeof(T);
        size_t bitsPerComponent = 8;
        CGBitmapInfo bitmapInfo = 0;

        // Set bitmap info based on channel count
        // Note: This assumes standard RGBA/RGB byte ordering. For custom pixel formats,
        // users may need to adjust the bitmap info flags accordingly.
        if (C == 4)
        {
            // RGBA - assume RGBA format
            bitmapInfo = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
        }
        else if (C == 3)
        {
            // RGB
            bitmapInfo = kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big;
        }
        else if (C == 2)
        {
            // RG - Gray + Alpha
            bitmapInfo = kCGImageAlphaPremultipliedLast;
        }
        else
        {
            // Grayscale
            bitmapInfo = kCGImageAlphaNone;
        }

        // Allocate memory and create context
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

        // Point base class data pointer to bitmap data
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

        // Crop source region
        CGImageRef croppedImage = CGImageCreateWithImageInRect(fullImage, sourceRect);
        CGImageRelease(fullImage);

        if (!croppedImage)
            return false;

        CGContextDrawImage(destContext, destRect, croppedImage);
        CGImageRelease(croppedImage);

        return true;
    }

    // Explicit instantiation of common types
    template class BitmapMacOS<math::Vector4u8, 4>;
    template class BitmapMacOS<math::Vector3u8, 3>;
    template class BitmapMacOS<math::Vector2u8, 2>;
    template class BitmapMacOS<uint8, 1>;

} // namespace hgl::bitmap

#endif // __APPLE__
