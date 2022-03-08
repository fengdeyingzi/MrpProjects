#include "c/控件/type.h"

#define MSGBOXBK 0x00ff0000
#define MSGBOXBG 0x000011ff
#define MSGBOXCH 0x00ffffff

/*显示提示框(800)ms后消失
x,y显示位置
title 显示内容
*/

void showMsgBox(int x,int y,char*title)
{
int zw,zh;
int r,g,b;
make(MSGBOXBK,&r,&g,&b);
textwh(title,0,FONT,&zw,&zh);
zw+=10;zh+=5;
dline(x+1,y,x+zw-1,y,r,g,b);
dline(x+1,y+zh,x+zw-1,y+zh,r,g,b);
dline(x,y+1,x,y+zh-1,r,g,b);
dline(x+zw,y+1,x+zw,y+zh-1,r,g,b);
make(MSGBOXBG,&r,&g,&b);
drect(x+1,y+1,zw-1,zh-1,r,g,b);
make(MSGBOXCH,&r,&g,&b);
dtext(title,x+5,y+3,r,g,b,0,FONT);
ref(x,y,zw+1,zh+1);
sleep(800);
}

/*显示标题栏
x y w h 标题栏的位置区域
uclr，dclr，背景上部颜色，下部颜色 888格式(0x00005500)(绿色)
tclr 文字颜色 同上
title 标题
*/
void drawTitleBar(int x,int y,int w,int h,int32 uclr,int32 dclr,int32 tclr,char* title)
{
int zw,zh;
textwh(title,0,FONT,&zw,&zh);
shaderect(x,y,w,h,uclr,dclr,0);
dtext(title,x+2,y+(h-zh)/2,MAKER(tclr),MAKEG(tclr),MAKEB(tclr),0,FONT);
}

/*显示工具栏
x y w h 位置信息
uclr dclr tclr 背景上、下颜色，文字颜色
left 左边文字
zhon 中间
right 右边
*/
void drawToolBar(int x,int y,int w,int h,int32 uclr,int32 dclr,int32 tclr,char* left,char* zhon,char* right)
{
int zw,zh;
int r,g,b;
textwh(left,0,FONT,&zw,&zh);
shaderect(x,y,w,h,uclr,dclr,0);
make(tclr,&r,&g,&b);
dtext(left,x+2,y+(h-zh)/2,r,g,b,0,FONT);
textwh(zhon,0,FONT,&zw,&zh);
dtext(zhon,x+(w-zw)/2,y+(h-zh)/2,r,g,b,0,FONT);
textwh(right,0,FONT,&zw,&zh);
dtext(right,x+w-zw-2,y+(h-zh)/2,r,g,b,0,FONT);
}