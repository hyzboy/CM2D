#ifdef HGL_IMAGEMAGICK_SUPPORT

#include<hgl/2d/BitmapImageMagick.h>
#include<hgl/type/StdString.h>
#include<Magick++.h>
#include<sstream>
#include<mutex>

namespace hgl
{
    namespace bitmap
    {
        /**
         * 线程安全的ImageMagick初始化
         */
        static void EnsureImageMagickInitialized()
        {
            static std::once_flag init_flag;
            std::call_once(init_flag, []() {
                Magick::InitializeMagick(nullptr);
            });
        }

        /**
         * 将ImageMagick的StorageType转换为对应的通道数和位深度
         */
        static bool GetImageMagickFormat(uint channels, uint bits_per_channel,
                                        Magick::StorageType &storage_type,
                                        std::string &map)
        {
            // 设置存储类型
            switch(bits_per_channel)
            {
                case 8:  storage_type=Magick::CharPixel;  break;
                case 16: storage_type=Magick::ShortPixel; break;
                case 32: storage_type=Magick::FloatPixel; break;
                default: return false;
            }

            // 设置通道映射
            switch(channels)
            {
                case 1: map="I"; break;      // Intensity (grayscale)
                case 2: map="IA"; break;     // Intensity + Alpha
                case 3: map="RGB"; break;    // Red, Green, Blue
                case 4: map="RGBA"; break;   // Red, Green, Blue, Alpha
                default: return false;
            }

            return true;
        }

        /**
         * 从ImageMagick加载图像
         */
        bool LoadBitmapFromImageMagick(const OSString &filename, ImageMagickLoader *loader)
        {
            if(!loader)return false;

            try
            {
                // 线程安全的初始化
                EnsureImageMagickInitialized();

                // 加载图像
                Magick::Image image;

                #ifdef _WIN32
                    // Windows下使用宽字符
                    const std::string u8_fn=ToStdString(filename);

                    image.read(u8_fn.c_str());
                #else
                    // Unix-like系统使用UTF-8
                    image.read(std::string(filename.c_str()));
                #endif

                // 获取图像信息
                uint width=image.columns();
                uint height=image.rows();
                uint channels=loader->OnChannels();
                uint bits_per_channel=loader->OnChannelBits();

                if(width==0||height==0)
                {
                    loader->OnLoadFailed();
                    return false;
                }

                // 分配Bitmap内存
                void *data=loader->OnRecvBitmap(width,height);
                if(!data)
                {
                    loader->OnLoadFailed();
                    return false;
                }

                // 确定存储格式
                Magick::StorageType storage_type;
                std::string map;

                if(!GetImageMagickFormat(channels,bits_per_channel,storage_type,map))
                {
                    loader->OnLoadFailed();
                    return false;
                }

                // 导出像素数据
                // ImageMagick使用左上角为原点，与OpenGL的左下角不同
                // 但TGA等格式通常也是左上角，所以这里保持一致
                image.write(0,0,width,height,map,storage_type,data);

                return true;
            }
            catch(Magick::Exception &e)
            {
                // 捕获ImageMagick异常
                // 可以记录日志：e.what()
                loader->OnLoadFailed();
                return false;
            }
            catch(...)
            {
                loader->OnLoadFailed();
                return false;
            }
        }

        /**
         * 保存Bitmap到文件
         */
        bool SaveBitmapToImageMagick(const OSString &filename,
                                     void *data,
                                     uint width,
                                     uint height,
                                     uint channels,
                                     uint single_channel_bits,
                                     const std::string &format)
        {
            if(!data||width==0||height==0||channels==0||single_channel_bits==0)
                return false;

            try
            {
                // 线程安全的初始化
                EnsureImageMagickInitialized();

                // 确定存储格式
                Magick::StorageType storage_type;
                std::string map;

                if(!GetImageMagickFormat(channels,single_channel_bits,storage_type,map))
                    return false;

                // 创建图像对象
                Magick::Image image;
                image.size(Magick::Geometry(width,height));

                // 设置图像深度
                switch(single_channel_bits)
                {
                    case 8:  image.depth(8);  break;
                    case 16: image.depth(16); break;
                    case 32: image.depth(32); break;
                }

                // 导入像素数据
                image.read(width,height,map,storage_type,data);

                // 设置输出格式（如果指定）
                if(!format.empty())
                {
                    image.magick(format);
                }

                // 保存文件
                #ifdef _WIN32
                    const std::string u8_fn=ToStdString(filename);

                    image.write(u8_fn);
                #else
                    image.write(std::string(filename.c_str()));
                #endif

                return true;
            }
            catch(Magick::Exception &e)
            {
                // 捕获ImageMagick异常
                // 可以记录日志：e.what()
                return false;
            }
            catch(...)
            {
                return false;
            }
        }

        namespace imagemagick
        {
            /**
             * 获取支持的格式列表
             */
            std::string GetSupportedFormats()
            {
                try
                {
                    // 线程安全的初始化
                    EnsureImageMagickInitialized();

                    std::list<Magick::CoderInfo> coders;
                    Magick::coderInfoList(&coders,
                                         Magick::CoderInfo::TrueMatch,
                                         Magick::CoderInfo::AnyMatch,
                                         Magick::CoderInfo::AnyMatch);

                    std::ostringstream oss;
                    bool first=true;

                    for(auto &coder : coders)
                    {
                        if(coder.isReadable()||coder.isWritable())
                        {
                            if(!first)oss<<", ";
                            oss<<coder.name();
                            first=false;
                        }
                    }

                    return oss.str();
                }
                catch(...)
                {
                    return "Error retrieving format list";
                }
            }

            /**
             * 获取ImageMagick版本
             */
            std::string GetVersion()
            {
                try
                {
                    return MagickLibVersionText;
                }
                catch(...)
                {
                    return "Unknown";
                }
            }

            /**
             * 检查格式是否支持
             */
            bool IsSupportedFormat(const std::string &format)
            {
                try
                {
                    // 线程安全的初始化
                    EnsureImageMagickInitialized();

                    Magick::CoderInfo info(format);
                    return (info.isReadable()||info.isWritable());
                }
                catch(...)
                {
                    return false;
                }
            }
        }//namespace imagemagick
    }//namespace bitmap
}//namespace hgl

#endif //HGL_IMAGEMAGICK_SUPPORT
