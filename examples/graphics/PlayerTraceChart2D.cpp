#include<hgl/util/csv/CSVParse.h>
#include<hgl/type/ValueArray.h>
#include<hgl/type/Map.h>
#include<iostream>
#include<hgl/2d/BitmapLoad.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/DrawGeometry.h>
#include"BitmapFont.h"
#include<hgl/color/Color.h>
#include<hgl/filesystem/Filename.h>

using namespace hgl;
using namespace hgl::bitmap;

uint POSITION_SCALE_RATE=100;
uint64 BATTLE_FIELD_ID=0;

constexpr const uint CHAR_WIDTH=8;
constexpr const uint CHAR_HEIGHT=16;
constexpr const uint CHAR_HALF_WIDTH=4;
constexpr const uint CHAR_HALF_HEIGHT=8;

constexpr const uint ICON_SIZE=8;

BitmapRGB8 *BackgroundBitmap=nullptr;
DrawGeometryRGB8 *draw_bmp=nullptr;

using TraceList=ValueArray<Vector2i>;

ObjectMap<uint,TraceList> PlayerTrace;

Color3ub *PlayerColor=nullptr;

int os_main(int argc,os_char **argv)
{
    std::cout<<"PlayerTraceChart2D"<<std::endl;

    if(argc<5)
    {
        std::cout<<"Example: map.tga 100 data.csv XXXXXX"<<std::endl;
        return(0);
    }

    return(0);
}
