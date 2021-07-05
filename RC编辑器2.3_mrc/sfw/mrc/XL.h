
#ifndef _XL_H__
#define _XL_H__
#include <mrc_base.h>
#include <mpc.h>
int FONT=1;

int fontw,fonth;

colorst textcolor;
//初始化文字颜色(不规范)

void textsetcolor(int r,int g,int b)
{
textcolor.r=r;
textcolor.g=g;
textcolor.b=b;

}
//textsetcolor(50,50,50);

void font_init()
{
	textsetcolor(50,50,50);
textwh("字",0,FONT,&fontw,&fonth);

}
//font_init();

void font_set(int font)
{
FONT=font;
}


long _len;

void filebmp(char *in,uint16 *ptr)//本地读取图片到指针
{
	long f;
_len=getlen(in);
 f=open(in,1);
read(f,ptr,_len);
close(f);
}

void rf()
{
ref(0,0,SCRW,SCRH);

}

void cl()
{
cls(240,240,240);
}


//圆角矩形框
void nrect(int x,int y,int w,int h,int r,int g,int b)
{
drect(x+1,y,w-2,2,r,g,b);
drect(x+1,y+h-2,w-2,2,r,g,b);
drect(x,y+1,2,h-2,r,g,b);
drect(x+w-2,y+1,2,h-2,r,g,b);
}



 void drawtxt2(char *str,int x,int y,int w,int h,int r,int g,int b,int font)
{
rectst rect;
colorst color;
rect.x=x;
rect.y=y;
rect.w=w;
rect.h=h;
color.r=r;
color.g=g;
color.b=b;
dtextex(str,x,y,&rect,&color,2,font);
}



void srect(int16 xx,int16 yy,int16 ww,int16 hh,uint8 rr,uint8 gg,uint8 bb,uint8 r1,uint8 g1,uint8 b1,uint8 nn)
//渐变矩形
{
int p=rr*0x10000+gg*0x100+bb,q=r1*0x10000+g1*0x100+b1;
shaderect(xx,yy,ww,hh,p,q,nn);}






void textcenter(char *txt,int x,int y,int w,int h)//文字居中显示
{
int fontw,fonth;
textwh(txt,0,FONT,&fontw,&fonth);
dtext(txt,x+(w-fontw)/2,y+(h-fonth)/2,textcolor.r,textcolor.g,textcolor.b,0,FONT);
}



void textleft(char *txt, int x, int y, int h)//纵向居中
{
int fontw,fonth;
textwh(txt,0,1,&fontw,&fonth);
dtext(txt,x,y+(h-fonth)/2,textcolor.r,textcolor.g,textcolor.b,0,FONT);
}



void textright(char *txt, int x, int y, int h)//右对齐纵向居中
{
int fontw,fonth;
textwh(txt,0,FONT,&fontw,&fonth);
dtext(txt,x-fontw,y+(h-fonth)/2,textcolor.r,textcolor.g,textcolor.b,0,FONT);
}


void toolbar(char *text,char *text2)//工具条
{
//int fontw,fonth;
//textwh(text2,0,FONT,&fontw,&fonth);
drect(0,SCRH-27,SCRW,27,50,130,210);
dline(0,SCRH-29,SCRW,SCRH-29,50,100,170);
//dtext(text,2,SCRH-30+(30-fonth)/2,50,50,50,0,FONT);
//dtext(text2,SCRW-2- fontw,SCRH-30 +(30-fonth)/2,50,50,50,0,FONT);
textleft(text,2,SCRH-30,30);
textright(text2,SCRW-2,SCRH-30,30);
}


void linerect(int x,int y,int w,int h,int r,int g,int b)
//线段框
{
dline(x,y,x+w,y,r,g,b);
dline(x,y+h,x+w,y+h,r,g,b);
dline(x,y,x,y+h,r,g,b);
dline(x+w,y,x+w,y+h,r,g,b);
}

void titlebar(char *text)
{
drect(1,1,SCRW-2,28,220,220,220);
linerect(0,0,SCRW-1,29,150,150,150);
textcenter(text,0,0,SCRW,30);
}






void trect(char *txt)
//提示框
{
int ftw,fth;
textwh(txt,0,1,&ftw,&fth);
drect((SCRW-ftw-16)/2,SCRH-68,ftw+16,fth+2,200,200,200);
nrect( (SCRW-ftw-20)/2,SCRH-70,ftw+20,fth+6,75,165,205);
dtext(txt,(SCRW-ftw)/2,SCRH-67,20,20,20,0,1);
}











/*
void gdt(int x,int y,int w,int h,int len,int xh,int max)//画滚动条
{
drect(x,y,w,h,100,100,100);
drect(x,y+h*xh/len,w,h*max/len,30,120,180);
}
*/


void unpack(char*in,char*out)
//释放mrp内部文件
{
int32 f; uint8 *buf;
int32 len;
buf=NULL;readfilefrommrp("temp.mrp",in,&buf,&len,0);f=open(out,12);write(f,buf,len);close(f);free(buf);
}



void yrect(int x,int y,int w,int h,int r, int g, int b)//圆角矩形
{
drect(x+1,y,w-2,h,r,g,b);
drect(x,y+1,w,h-2,r,g,b);
}



#endif

