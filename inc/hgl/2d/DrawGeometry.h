#pragma once

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/blend.h>
#include<hgl/math/FastTriangle.h>

namespace hgl
{
    namespace bitmap
    {
        template<typename T> class DrawGeometry
        {
        public:

            using FormatBitmap=Bitmap<T>;

        protected:

            FormatBitmap *bitmap;

            T draw_color;
            float alpha;

            BlendColor<T> no_blend;
            BlendColor<T> *blend;

        public:

            DrawGeometry(FormatBitmap *fb)
            {
                bitmap=fb;
                hgl_zero(draw_color);
                alpha=1;
                blend=&no_blend;
            }

            virtual ~DrawGeometry()=default;

            virtual void SetDrawColor(const T &color)
            {
                draw_color=color;
            }

            void SetBlend(BlendColor<T> *bc)
            {
                blend=bc;
            }

            void CloseBlend()
            {
                blend=&no_blend;
            }

            void SetAlpha(const float &a)
            {
                alpha=a;
            }

            bool GetPixel(int x,int y,T &color)
            {
                if(!data)return(false);

                T *p=GetData(x,y);

                if(!p)return(false);

                color=*p;

                return(true);
            }

            bool PutPixel(int x,int y)
            {
                if(!bitmap)return(false);

                T *p=bitmap->GetData(x,y);

                if(!p)return(false);

                *p=(*blend)(draw_color,*p,alpha);

                return(true);
            }

            bool DrawHLine(int x,int y,int length)
            {
                if(!bitmap)return(false);

                const int width=bitmap->GetWidth();
                const int height=bitmap->GetHeight();

                if(y<0||y>=height)return(false);
                if(x>=width)return(false);
                if(x<0){length+=x;x=0;}
                if(x+length>width)length=width-x;

                if(length<=0)return(false);

                T *p=bitmap->GetData(x,y);

                for(int i=0;i<length;i++)
                    *p++=(*blend)(draw_color,*p,alpha);

                return(true);
            }

            bool DrawBar(int l,int t,int w,int h)
            {
                if(!bitmap)return(false);

                const int width=bitmap->GetWidth();
                const int height=bitmap->GetHeight();

                if(l>=width||t>=height)return(false);

                if(l<0){w+=l;l=0;}
                if(t<0){h+=t;t=0;}

                if(l+w>width)w=width-l;
                if(t+h>height)h=height-t;

                if(w<=0||h<=0)return(false);

                T *p=bitmap->GetData(x,y);

                for(int y=t;y<t+h;y++)
                {
                    for(int i=0;i<length;i++)
                        *p++=(*blend)(draw_color,*p,alpha);

                    p+=width-w;
                }

                return(true);
            }

            bool DrawVLine(int x,int y,int length)
            {
                if(!bitmap)return(false);

                const int width=bitmap->GetWidth();
                const int height=bitmap->GetHeight();

                if(x<0||x>=width)return(false);
                if(y>=height)return(false);
                if(y<0){length+=y;y=0;}
                if(y+length>height)length=height-y;

                if(length<=0)return(false);

                const uint line_bytes=bitmap->GetLineBytes();
                T *p=bitmap->GetData(x,y);

                for(int i=0;i<length;i++)
                {
                    *p=(*blend)(draw_color,*p,alpha);
                    p+=line_bytes;
                }

                return(true);
            }

            bool DrawWireCircle(int x0,int y0,int radius)
            {
                if(!bitmap)return(false);

                if(radius<=0)return(false);

                int tn;
                int x,y;
                int xmax;

                y=radius;
                x=0;
                xmax=int(radius*HGL_SIN_45);
                tn=(1-radius*2);

                while(x<=xmax)
                {
                    if(tn>=0)
                    {
                        tn+=(6+((x-y)<<2));
                        y--;
                    }
                    else
                        tn+=((x<<2)+2);

                    PutPixel(x0+y,y0+x);
                    PutPixel(x0+x,y0+y);
                    PutPixel(x0-x,y0+y);
                    PutPixel(x0-y,y0+x);
                    PutPixel(x0-y,y0-x);
                    PutPixel(x0-x,y0-y);
                    PutPixel(x0+x,y0-y);
                    PutPixel(x0+y,y0-x);

                    x++;
                }

                PutPixel(x0+y,y0+x);
                PutPixel(x0+x,y0+y);
                PutPixel(x0-x,y0+y);
                PutPixel(x0-y,y0+x);
                PutPixel(x0-y,y0-x);
                PutPixel(x0-x,y0-y);
                PutPixel(x0+x,y0-y);
                PutPixel(x0+y,y0-x);

                return(true);
            }

            bool DrawSolidCircle(int x,int y,int radius)
            {
                if(!bitmap)return(false);

                if(radius<=0)return(false);

                int x0=x-radius;
                int y0=y-radius;
                int x1=x+radius;
                int y1=y+radius;

                const int width=bitmap->GetWidth();
                const int height=bitmap->GetHeight();

                if(x0<0)x0=0;
                if(y0<0)y0=0;
                if(x1>=width)x1=width-1;
                if(y1>=height)y1=height-1;

                if(x0>x1||y0>y1)return(false);

                int r2=radius*radius;

                for(int cy=y0;cy<=y1;cy++)
                {
                    int dy=cy-y;
                    int dy2=dy*dy;

                    for(int cx=x0;cx<=x1;cx++)
                    {
                        int dx=cx-x;
                        int dx2=dx*dx;

                        if(dx2+dy2<=r2)
                            PutPixel(cx,cy);
                    }
                }

                return(true);
            }

            void DrawLine(int x1, int y1, int x2, int y2)
            {
                int p, n, x, y, tn;

                if(y1==y2)
                {
                    if(x1>x2)
                    {
                        x=x2;x2=x1;x1=x;
                    }

                    DrawHLine(x1, y1, x2-x1+1);
                    return;
                }

                if(x1==x2)
                {
                    if(y1>y2)
                    {
                        y=y2;y2=y1;y1=y;
                    }

                    DrawVLine(x1, y1, y2-y1+1);
                    return;
                }

                if(abs(y2-y1)<=abs(x2-x1))
                {
                    if((y2<y1&&x2<x1)||(y1<=y2&&x1>x2))
                    {
                        x=x2;y=y2;x2=x1;y2=y1;x1=x;y1=y;
                    }

                    if(y2>=y1&&x2>=x1)
                    {
                        x=x2-x1;y=y2-y1;
                        p=2*y;n=2*x-2*y;tn=x;

                        while(x1<=x2)
                        {
                            if(tn>=0)tn-=p;
                            else { tn+=n;y1++; }

                            PutPixel(x1++, y1);
                        }
                    }
                    else
                    {
                        x=x2-x1;y=y2-y1;
                        p=-2*y;n=2*x+2*y;tn=x;
                        while(x1<=x2)
                        {
                            if(tn>=0)tn-=p;
                            else { tn+=n;y1--; }

                            PutPixel(x1++, y1);
                        }
                    }
                }
                else
                {
                    x=x1;x1=y2;y2=x;y=y1;y1=x2;x2=y;

                    if((y2<y1&&x2<x1)||(y1<=y2&&x1>x2))
                    {
                        x=x2;y=y2;x2=x1;y2=y1;x1=x;y1=y;
                    }

                    if(y2>=y1&&x2>=x1)
                    {
                        x=x2-x1;y=y2-y1;p=2*y;n=2*x-2*y;tn=x;

                        while(x1<=x2)
                        {
                            if(tn>=0)tn-=p;
                            else { tn+=n;y1++; }

                            PutPixel(y1, x1++);
                        }
                    }
                    else
                    {
                        x=x2-x1;y=y2-y1;p=-2*y;n=2*x+2*y;tn=x;

                        while(x1<=x2)
                        {
                            if(tn>=0)tn-=p;
                            else { tn+=n;y1--; }

                            PutPixel(y1, x1++);
                        }
                    }
                }
            }

            void DrawSector(int x0, int y0, uint r, uint stangle, uint endangle)
            {
                int i, j;
                int *xy;
                int bx, ex, bxd, exd, bxf, exf, ben;
                int tn, x, y;
                int xmax;

                y=r; x=0;
                xmax=(int)(r*HGL_SIN_45);
                tn=(1-r*2);

                xy=(int *)calloc(20, sizeof(int));
                xy[0]=x0+r;xy[1]=y0;
                xy[2]=x0;  xy[3]=y0-r;
                xy[4]=x0;  xy[5]=y0-r;
                xy[6]=x0-r;xy[7]=y0;
                xy[8]=x0-r;xy[9]=y0;
                xy[10]=x0;  xy[11]=y0+r;
                xy[12]=x0;  xy[13]=y0+r;
                xy[14]=x0+r;xy[15]=y0;

                bx=stangle/45;
                ex=endangle/45;
                ben=ex-bx-1;

                xy[16]=(int)(r*Lcos(stangle));
                xy[17]=(int)(r*Lsin(stangle));
                xy[18]=(int)(r*Lcos(endangle));
                xy[19]=(int)(r*Lsin(endangle));

                DrawLine(x0+xy[16], y0-xy[17], x0, y0);
                DrawLine(x0+xy[18], y0-xy[19], x0, y0);

                if(bx==1||bx==2||bx==5||bx==6)bxd=abs(xy[16]);else bxd=abs(xy[17]);
                if(ex==1||ex==2||ex==5||ex==6)exd=abs(xy[18]);else exd=abs(xy[19]);
                if(bx==0||bx==2||bx==4||bx==6)bxf=0;          else bxf=1;
                if(ex==0||ex==2||ex==4||ex==6)exf=1;          else exf=0;

                while(x<=xmax)
                {
                    if(tn>=0)
                    {
                        tn+=(6+((x-y)*4));
                        y--;
                        xy[0]--;
                        xy[3]++;
                        xy[5]++;
                        xy[6]++;
                        xy[8]++;
                        xy[11]--;
                        xy[13]--;
                        xy[14]--;
                    }
                    else tn+=((x*4)+2);

                    if(stangle<endangle)
                    {
                        j=(bx+1)*2;
                        for(i=0;i<ben;i++)
                        {
                            PutPixel(xy[j], xy[j+1]);
                            j+=2;
                        }
                    }
                    else if(stangle>endangle)
                    {
                        j=(bx+1)*2;
                        for(i=bx+1;i<8;i++)
                        {
                            PutPixel(xy[j], xy[j+1]);
                            j+=2;
                        }
                        j=0;
                        for(i=0;i<ex;i++)
                        {
                            PutPixel(xy[j], xy[j+1]);
                            j+=2;
                        }
                    }

                    i=bx*2;
                    if((x>bxd)^bxf)PutPixel(xy[i], xy[i+1]);i=ex*2;
                    if((x>exd)^exf)PutPixel(xy[i], xy[i+1]);x++;
                    xy[1]--;
                    xy[2]++;
                    xy[4]--;
                    xy[7]--;
                    xy[9]++;
                    xy[10]--;
                    xy[12]++;
                    xy[15]++;
                }
                free(xy);
            }

            void DrawMonoBitmap(const int left,const int top,const uint8 *data,const int w,const int h)
            {
                if(!data)return;

                if(left<0||left>=bitmap->GetWidth()-w)return;
                if(top<0||top>=bitmap->GetHeight()-h)return;

                const uint8 *sp=data;
                uint8 bit;
                T *tp=bitmap->GetData(left,top);

                const uint line_wrap=bitmap->GetWidth()-w;

                bit=1<<7;

                for(int row=0;row<h;row++)
                {
                    for(int col=0;col<w;col++)
                    {
                        if(*sp&bit)
                        {
                            *tp=(*blend)(draw_color,*tp,alpha);
                        }

                        ++tp;

                        bit>>=1;
                        if(bit==0)
                        {
                            ++sp;
                            bit=1<<7;
                        }
                    }

                    tp+=line_wrap;
                }
            }
        };//template<typename T,uint CHANNELS> class DrawGeometry
    }//namespace bitmap
}//namespace hgl
