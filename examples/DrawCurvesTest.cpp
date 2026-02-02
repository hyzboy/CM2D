/**
 * Draw Curves Test Example
 * Demonstrates drawing arcs and spline curves using DrawGeometry
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/DrawGeometry.h>
#include<hgl/2d/TGA.h>
#include<hgl/color/Color3ub.h>
#include<hgl/math/Vector.h>
#include<iostream>
#include<cmath>

using namespace hgl;
using namespace hgl::bitmap;

namespace
{
    void Clear(BitmapRGB8 &bmp,const Color3ub &color)
    {
        auto *data=bmp.GetData();
        const int total=bmp.GetWidth()*bmp.GetHeight();
        for(int i=0;i<total;i++)
            data[i]=color;
    }

    void LogPixel(const char *label,const BitmapRGB8 &bmp,int x,int y)
    {
        const int w=bmp.GetWidth();
        const int h=bmp.GetHeight();
        if(x<0||y<0||x>=w||y>=h)
        {
            std::cout<<label<<" ("<<x<<","<<y<<"): out of bounds"<<std::endl;
            return;
        }

        const Color3ub *p=bmp.GetData(x,y);
        if(!p)
        {
            std::cout<<label<<" ("<<x<<","<<y<<"): null"<<std::endl;
            return;
        }

        std::cout<<label<<" ("<<x<<","<<y<<"): "
                 <<int(p->r)<<","<<int(p->g)<<","<<int(p->b)<<std::endl;
    }
}

void TestArcs()
{
    std::cout<<"Testing arcs..."<<std::endl;

    BitmapRGB8 bmp;
    bmp.Create(512,512);
    Clear(bmp,Color3ub(18,18,22));

    DrawGeometryRGB8 draw(&bmp);

    draw.SetDrawColor(Color3ub(255,80,80));
    draw.DrawArc(256,256,180,0.0f,3.14159f,64);

    draw.SetDrawColor(Color3ub(80,255,120));
    draw.DrawArc(256,256,140,3.14159f,6.28318f,64);

    draw.SetDrawColor(Color3ub(120,180,255));
    draw.DrawArc(256,256,100,0.78539f,4.71238f,48);

    LogPixel("Arc sample",bmp,256+180,256);
    LogPixel("Arc sample",bmp,256-180,256);
    LogPixel("Arc sample",bmp,256,256-180);

    SaveTga("draw_curve_arcs.tga",&bmp);
}

void TestQuadraticBeziers()
{
    std::cout<<"Testing quadratic Bezier curves..."<<std::endl;

    BitmapRGB8 bmp;
    bmp.Create(512,512);
    Clear(bmp,Color3ub(20,22,28));

    DrawGeometryRGB8 draw(&bmp);

    draw.SetDrawColor(Color3ub(255,255,255));
    draw.DrawQuadraticBezier({40,460},{256,40},{472,460},120);

    draw.SetDrawColor(Color3ub(80,170,255));
    draw.DrawQuadraticBezier({40,420},{256,140},{472,420},80);

    draw.SetDrawColor(Color3ub(255,180,80));
    draw.DrawQuadraticBezier({60,480},{256,260},{452,480},60);

    LogPixel("Quad sample",bmp,256,260);
    LogPixel("Quad sample",bmp,256,360);
    LogPixel("Quad sample",bmp,256,460);

    SaveTga("draw_curve_quadratic_bezier.tga",&bmp);
}

void TestCubicBeziers()
{
    std::cout<<"Testing cubic Bezier curves..."<<std::endl;

    BitmapRGB8 bmp;
    bmp.Create(512,512);
    Clear(bmp,Color3ub(16,20,16));

    DrawGeometryRGB8 draw(&bmp);

    draw.SetDrawColor(Color3ub(255,120,255));
    draw.DrawCubicBezier({40,420},{140,60},{372,60},{472,420},140);

    draw.SetDrawColor(Color3ub(120,255,160));
    draw.DrawCubicBezier({40,380},{120,500},{392,500},{472,380},120);

    draw.SetDrawColor(Color3ub(120,200,255));
    draw.DrawCubicBezier({60,460},{140,240},{372,240},{452,460},100);

    LogPixel("Cubic sample",bmp,256,240);
    LogPixel("Cubic sample",bmp,256,360);
    LogPixel("Cubic sample",bmp,256,460);

    SaveTga("draw_curve_cubic_bezier.tga",&bmp);
}

void TestCatmullRom()
{
    std::cout<<"Testing Catmull-Rom splines..."<<std::endl;

    BitmapRGB8 bmp;
    bmp.Create(512,512);
    Clear(bmp,Color3ub(14,18,22));

    DrawGeometryRGB8 draw(&bmp);

    draw.SetDrawColor(Color3ub(255,255,255));
    draw.DrawCatmullRomSpline({40,420},{140,120},{372,120},{472,420},120,0.5f);

    draw.SetDrawColor(Color3ub(255,170,80));
    draw.DrawCatmullRomSpline({40,380},{140,220},{372,220},{472,380},90,0.3f);

    draw.SetDrawColor(Color3ub(90,190,255));
    draw.DrawCatmullRomSpline({40,460},{140,320},{372,320},{472,460},90,0.8f);

    LogPixel("Catmull sample",bmp,140,120);
    LogPixel("Catmull sample",bmp,256,220);
    LogPixel("Catmull sample",bmp,372,120);

    SaveTga("draw_curve_catmull_rom.tga",&bmp);
}

int main(int argc,char **argv)
{
    std::cout<<"=== CM2D Draw Curves Test ==="<<std::endl;
    std::cout<<std::endl;

    TestArcs();
    TestQuadraticBeziers();
    TestCubicBeziers();
    TestCatmullRom();

    std::cout<<std::endl;
    std::cout<<"All curve tests completed!"<<std::endl;

    return 0;
}
