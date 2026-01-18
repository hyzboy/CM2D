#include<hgl/type/StringList.h>
#include<hgl/io/LoadStringList.h>
#include<hgl/type/Gradient.h>
#include<hgl/math/Vector.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/filesystem/Filename.h>
#include<iostream>
#include<hgl/2d/BitmapLoad.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/DrawGeometry.h>
#include"BitmapFont.h"

using namespace hgl;
using namespace hgl::bitmap;

OSString csv_filename;

uint POSITION_SCALE_RATE=100;

BitmapRGB8 *BackgroundBitmap=nullptr;

bool LoadBackgroundBitmap()
{
    BackgroundBitmap=bitmap::LoadBitmapRGB8FromTGA(OS_TEXT("mini_map.tga"));

    return(BackgroundBitmap);
}

uint CHAR_BITMAP_WIDTH=0;
uint CHAR_BITMAP_HEIGHT=0;

bool InitBitmapFont()
{
    if(!LoadBitmapFont())
        return(false);

    CHAR_BITMAP_WIDTH=8;
    CHAR_BITMAP_HEIGHT=16;

    return(true);
}

constexpr const float LOW_GAP=0.2f;
constexpr const Vector4u8 black_color={0,0,0,255};
constexpr const Vector4u8 white_color={255,255,255,255};

constexpr const Vector4u8 stop_color[]=
{
    {  0,  0,255,255},
    {  0,255,255,255},
    {  0,255,  0,255},
    {255,255,  0,255},
    {255,  0,  0,255}
};

constexpr const uint STOP_COUNT=sizeof(stop_color)/sizeof(Vector4u8);

uint stop_count[STOP_COUNT];
uint top_count=0;

HGL_GRADIENT_DEFINE(GradientColor3u8,uint,Vector3u8)
{
    result.r=start.r+float(end.r-start.r)*pos;
    result.g=start.g+float(end.g-start.g)*pos;
    result.b=start.b+float(end.b-start.b)*pos;
}

GradientColor3u8 ColorGradient;

void InitGradient(uint max_count)
{
    for(uint i=0;i<STOP_COUNT;i++)
        ColorGradient.Add(max_count*(1.0-float(i)/float(STOP_COUNT-1)),stop_color[i]);
}

int os_main(int argc,os_char **argv)
{
    std::cout<<"Distribution Chart 2D"<<std::endl<<std::endl;

    if(argc<2)
    {
        std::cout<<"example: DistributionChart2D data.csv"<<std::endl<<std::endl;
        return 0;
    }

    if(!LoadBackgroundBitmap())
    {
        std::cerr<<"can't load background mini_map.tga !"<<std::endl;
        return 1;
    }

    if(!InitBitmapFont())
    {
        std::cerr<<"can't load font file VGA8.F16 !"<<std::endl;
        return 2;
    }

    return 0;
}
