#ifndef HGL_VSBASE_INCLUDE
#define HGL_VSBASE_INCLUDE

namespace hgl
{
    namespace vs
    {
        /**
         * 为方便程序处理，内存中仅支持以下格式
         */
        enum class DataFormat
        {
            U8,U16,U32,
            S8,S16,S32,
            F32,F64,
        };

        struct VSDataSource
        {
            void *pixel_data=nullptr;                                           ///<象素数据

        public:

            virtual ~VSDataSource()=0;
        };//class VSDataSource

        struct VSDataSourceRef:public VSDataSource
        {
        public:

            ~VSDataSourceRef() override {}
        };//

        class VSDataSourceCreate

        /**
         * 虚拟屏幕数据源
         */
        class VSData
        {
        protected:

            uint width,height;                                                  ///<尺寸
            uint color_component;                                               ///<颜色成份数量(1-4)
            DataFormat data_format[4];                                          ///<数据格式

            uint pixel_bytes;                                                   ///<每象素字节数
            uint line_bytes;                                                    ///<每一行象素数据的字节数

        public:

            VSData()
            {
                pixel_data=nullptr;
            }

            virtual ~VSData()=0;

            void *GetPointer(){return pixel_data;}

            void *GetPointer(uint row)
            {
                if(row>=height)return(nullptr);

                return ((uint8 *)pixel_data)+row*line_bytes;
            }

            void *GetPointer(uint col,uint row)
            {
                if(col>=width)return(nullptr);
                if(row>=height)return(nullptr);

                return ((uint8 *)pixel_data)+row*line_bytes+col*pixel_bytes;
            }
        };//class VSData

        /**
         * 虚拟屏幕基类
         */
        class VSBase
        {
        };//class VSBase


    }//namespace vs
}//namespace hgl
#endif//HGL_VSBASE_INCLUDE
