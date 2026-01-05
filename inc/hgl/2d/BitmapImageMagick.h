#pragma once

#ifdef HGL_IMAGEMAGICK_SUPPORT

#include<hgl/2d/Bitmap.h>
#include<hgl/type/String.h>
#include<string>

namespace hgl
{
    namespace bitmap
    {
        /**
         * ImageMagick加载器结构
         * 提供从ImageMagick加载图像到Bitmap的功能
         */
        struct ImageMagickLoader
        {
            virtual const uint OnChannels()const=0;
            virtual const uint OnChannelBits()const=0;
            
            virtual void *OnRecvBitmap(uint w,uint h)=0;
            virtual void OnLoadFailed()=0;
            
            virtual ~ImageMagickLoader()=default;
        };
        
        /**
         * ImageMagick加载器实现模板
         */
        template<typename T> 
        struct ImageMagickLoaderImpl:public ImageMagickLoader
        {
            T *bmp;
            
            ImageMagickLoaderImpl()
            {
                bmp=nullptr;
            }
            
            ~ImageMagickLoaderImpl()
            {
                // 不删除bmp，由调用者管理
            }
            
            const uint OnChannels()const override
            {
                return bmp?bmp->GetChannels():0;
            }
            
            const uint OnChannelBits()const override
            {
                return bmp?bmp->GetChannelBits():0;
            }
            
            void *OnRecvBitmap(uint w,uint h) override
            {
                if(!bmp)
                    bmp=new T;
                    
                bmp->Create(w,h);
                return bmp->GetData();
            }
            
            void OnLoadFailed() override
            {
                if(bmp)
                {
                    delete bmp;
                    bmp=nullptr;
                }
            }
        };
        
        /**
         * 从文件加载图像（使用ImageMagick）
         * @param filename 图像文件路径
         * @param loader ImageMagick加载器
         * @return 是否加载成功
         */
        bool LoadBitmapFromImageMagick(const OSString &filename, ImageMagickLoader *loader);
        
        /**
         * 从文件加载图像到指定的Bitmap类型（模板函数）
         * @tparam T Bitmap类型
         * @param filename 图像文件路径
         * @return 加载的Bitmap指针，失败返回nullptr
         */
        template<typename T>
        inline T *LoadBitmapFromImageMagick(const OSString &filename)
        {
            ImageMagickLoaderImpl<T> loader;
            
            if(LoadBitmapFromImageMagick(filename, &loader))
                return loader.bmp;
                
            return nullptr;
        }
        
        /**
         * 保存Bitmap到文件（使用ImageMagick）
         * @param filename 输出文件路径
         * @param data 图像数据指针
         * @param width 图像宽度
         * @param height 图像高度
         * @param channels 通道数
         * @param single_channel_bits 单通道位深度
         * @param format 图像格式（如"PNG", "JPEG", "BMP"等），默认从文件扩展名推断
         * @return 是否保存成功
         */
        bool SaveBitmapToImageMagick(const OSString &filename,
                                     void *data,
                                     uint width,
                                     uint height,
                                     uint channels,
                                     uint single_channel_bits,
                                     const std::string &format="");
        
        /**
         * 保存Bitmap对象到文件（模板函数）
         * @tparam T Bitmap类型
         * @param filename 输出文件路径
         * @param bmp Bitmap对象指针
         * @param format 图像格式，默认从文件扩展名推断
         * @return 是否保存成功
         */
        template<typename T>
        inline bool SaveBitmapToImageMagick(const OSString &filename,
                                           const T *bmp,
                                           const std::string &format="")
        {
            if(!bmp)return false;
            
            return SaveBitmapToImageMagick(filename,
                                          (void *)(bmp->GetData()),
                                          bmp->GetWidth(),
                                          bmp->GetHeight(),
                                          bmp->GetChannels(),
                                          bmp->GetChannelBits(),
                                          format);
        }
        
        /**
         * 便捷函数：加载RGB8格式图像
         */
        inline BitmapRGB8 *LoadBitmapRGB8FromImageMagick(const OSString &filename)
        {
            return LoadBitmapFromImageMagick<BitmapRGB8>(filename);
        }
        
        /**
         * 便捷函数：加载RGBA8格式图像
         */
        inline BitmapRGBA8 *LoadBitmapRGBA8FromImageMagick(const OSString &filename)
        {
            return LoadBitmapFromImageMagick<BitmapRGBA8>(filename);
        }
        
        /**
         * 便捷函数：加载灰度8格式图像
         */
        inline BitmapGrey8 *LoadBitmapGrey8FromImageMagick(const OSString &filename)
        {
            return LoadBitmapFromImageMagick<BitmapGrey8>(filename);
        }
        
        /**
         * ImageMagick功能函数
         */
        namespace imagemagick
        {
            /**
             * 获取ImageMagick支持的所有格式列表
             */
            std::string GetSupportedFormats();
            
            /**
             * 获取ImageMagick版本信息
             */
            std::string GetVersion();
            
            /**
             * 检查指定格式是否支持
             * @param format 格式名称（如"PNG", "JPEG"）
             */
            bool IsSupportedFormat(const std::string &format);
        }//namespace imagemagick
    }//namespace bitmap
}//namespace hgl

#endif //HGL_IMAGEMAGICK_SUPPORT
