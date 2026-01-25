#ifdef HGL_IMAGEMAGICK_SUPPORT

#include<hgl/2d/BitmapImageMagick.h>
#include<iostream>
#include<string>

using namespace hgl;
using namespace hgl::bitmap;

void PrintImageMagickInfo()
{
    std::cout<<"=== ImageMagick Integration Demo ==="<<std::endl;
    std::cout<<"ImageMagick Version: "<<imagemagick::GetVersion()<<std::endl;
    std::cout<<std::endl;
}

void PrintSupportedFormats()
{
    std::cout<<"=== Supported Image Formats ==="<<std::endl;
    std::string formats=imagemagick::GetSupportedFormats();
    std::cout<<formats<<std::endl;
    std::cout<<std::endl;
}

void TestFormatSupport()
{
    std::cout<<"=== Format Support Test ==="<<std::endl;

    constexpr const char* test_formats[]={"PNG","JPEG","BMP","GIF","TIFF","WebP","TGA"};

    for(auto format : test_formats)
    {
        bool supported=imagemagick::IsSupportedFormat(format);
        std::cout<<format<<": "<<(supported?"✓ Supported":"✗ Not supported")<<std::endl;
    }
    std::cout<<std::endl;
}

void DemoLoadSave()
{
    std::cout<<"=== Load and Save Demo ==="<<std::endl;

    // 创建一个测试图像
    BitmapRGBA8 *test_bitmap=new BitmapRGBA8();
    test_bitmap->Create(256,256);

    // 填充渐变色
    auto *data=test_bitmap->GetData();
    for(int y=0;y<256;y++)
    {
        for(int x=0;x<256;x++)
        {
            data[y*256+x].r=x;
            data[y*256+x].g=y;
            data[y*256+x].b=(x+y)/2;
            data[y*256+x].a=255;
        }
    }

    std::cout<<"Created test bitmap: 256x256 RGBA8"<<std::endl;

    // 保存为不同格式
    const char* output_formats[][2]={
        {"test_output.png","PNG"},
        {"test_output.jpg","JPEG"},
        {"test_output.bmp","BMP"},
        {"test_output.tga","TGA"}
    };

    for(auto &format_pair : output_formats)
    {
        const char* filename=format_pair[0];
        const char* format=format_pair[1];

        if(imagemagick::IsSupportedFormat(format))
        {
            bool success=SaveBitmapToImageMagick(filename,test_bitmap,format);
            std::cout<<"Save "<<filename<<": "<<(success?"✓ Success":"✗ Failed")<<std::endl;
        }
        else
        {
            std::cout<<"Save "<<filename<<": ✗ Format not supported"<<std::endl;
        }
    }

    delete test_bitmap;
    std::cout<<std::endl;
}

void DemoLoadImage()
{
    std::cout<<"=== Load Image Demo ==="<<std::endl;

    // 尝试加载刚才保存的PNG文件
    auto *loaded_bitmap=LoadBitmapRGBA8FromImageMagick("test_output.png");

    if(loaded_bitmap)
    {
        std::cout<<"Loaded test_output.png successfully"<<std::endl;
        std::cout<<"  Size: "<<loaded_bitmap->GetWidth()<<"x"<<loaded_bitmap->GetHeight()<<std::endl;
        std::cout<<"  Channels: "<<loaded_bitmap->GetChannels()<<std::endl;
        std::cout<<"  Bits per channel: "<<loaded_bitmap->GetChannelBits()<<std::endl;

        // 验证数据
        auto *data=loaded_bitmap->GetData();
        std::cout<<"  First pixel: R="<<(int)data[0].r
                 <<" G="<<(int)data[0].g
                 <<" B="<<(int)data[0].b
                 <<" A="<<(int)data[0].a<<std::endl;

        delete loaded_bitmap;
    }
    else
    {
        std::cout<<"Failed to load test_output.png"<<std::endl;
    }

    std::cout<<std::endl;
}

void DemoConversion()
{
    std::cout<<"=== Format Conversion Demo ==="<<std::endl;

    // 演示：PNG -> TGA -> JPEG 转换流程
    std::cout<<"Converting: PNG -> TGA -> JPEG"<<std::endl;

    // 1. 加载PNG
    auto *png_bitmap=LoadBitmapRGBA8FromImageMagick("test_output.png");
    if(!png_bitmap)
    {
        std::cout<<"Failed to load PNG"<<std::endl;
        return;
    }
    std::cout<<"  ✓ Loaded PNG"<<std::endl;

    // 2. 保存为TGA
    bool tga_ok=SaveBitmapToImageMagick("converted.tga",png_bitmap,"TGA");
    std::cout<<"  "<<(tga_ok?"✓":"✗")<<" Saved as TGA"<<std::endl;

    // 3. 重新加载TGA
    auto *tga_bitmap=LoadBitmapRGBA8FromImageMagick("converted.tga");
    if(tga_bitmap)
    {
        std::cout<<"  ✓ Loaded TGA"<<std::endl;

        // 4. 保存为JPEG (注意：JPEG不支持alpha通道，需要用RGB8)
        BitmapRGB8 *rgb_bitmap=new BitmapRGB8();
        rgb_bitmap->Create(tga_bitmap->GetWidth(),tga_bitmap->GetHeight());

        // 转换RGBA到RGB
        auto *src=tga_bitmap->GetData();
        auto *dst=rgb_bitmap->GetData();
        int total=tga_bitmap->GetWidth()*tga_bitmap->GetHeight();

        for(int i=0;i<total;i++)
        {
            dst[i].r=src[i].r;
            dst[i].g=src[i].g;
            dst[i].b=src[i].b;
        }

        bool jpeg_ok=SaveBitmapToImageMagick("converted.jpg",rgb_bitmap,"JPEG");
        std::cout<<"  "<<(jpeg_ok?"✓":"✗")<<" Saved as JPEG"<<std::endl;

        delete rgb_bitmap;
        delete tga_bitmap;
    }

    delete png_bitmap;
    std::cout<<std::endl;
}

int main(int argc,char **argv)
{
    std::cout<<std::endl;

    try
    {
        // 打印基本信息
        PrintImageMagickInfo();

        // 测试格式支持
        TestFormatSupport();

        // 演示保存功能
        DemoLoadSave();

        // 演示加载功能
        DemoLoadImage();

        // 演示格式转换
        DemoConversion();

        std::cout<<"=== All tests completed ==="<<std::endl;
        std::cout<<std::endl;

        return 0;
    }
    catch(const std::exception &e)
    {
        std::cerr<<"Exception: "<<e.what()<<std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr<<"Unknown exception occurred"<<std::endl;
        return 1;
    }
}

#else

#include<iostream>

int main()
{
    std::cout<<"ImageMagick support is not enabled."<<std::endl;
    std::cout<<"Please compile with -DCM2D_USE_IMAGEMAGICK=ON"<<std::endl;
    return 1;
}

#endif //HGL_IMAGEMAGICK_SUPPORT
