
#include <mrc_base.h>
#include "mrc_graphics.h"
#include "screen_adapter.h"
#include "mpc.h"
#include "uc3_font.h"

static int32 screen_width,screen_height;
//设置屏幕宽高
void sc_setScreenSize(int32 width, int32 height){
    screen_width=width;
    screen_height=height;
}

//读取图片 并自动调整图片宽高到适配屏幕的大小
BITMAP_565* sc_readBitmapFromAssets(const char* filename){
    char temp[100];
    BITMAP_565 *bmp;
    BITMAP_565 *bmpnew;
    int32 width_new,height_new;
    mrc_sprintf(temp,"%d_%s", SCRW, filename);
    //先检测是否存在指定屏幕宽度的图片,图片前缀加宽度
    bmp = readBitmap565FromAssets(temp);
    if(bmp == NULL){
        bmp = readBitmap565FromAssets(filename);
        //对图片进行缩放
        width_new = bmp->width * SCRW / screen_width;
        height_new = bmp->width * SCRH / screen_height;
        bmpnew = createBitmapFromBitmap(bmp, width_new, height_new);
        bitmapFree(bmp);
        return bmpnew;
    }
    return bmp;
}

//绘制一张图片
void sc_drawBitmap(BITMAP_565 *bmp, int32 x, int32 y){
    drawBitmap(bmp, x*SCRW/screen_width, y*SCRH/screen_height);
}

//绘制文字
int32 sc_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode){
    uc3_drawText(pcText, x*SCRW/screen_width, y*SCRH/screen_height, r, g, b, is_unicode);
}

//绘制矩形
void sc_drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color){
    gl_drawRect(x*SCRW/screen_width, y*SCRH/screen_height, w*SCRW/screen_width, h*SCRH/screen_height, color);
}

//画线
void sc_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color){
    gl_drawLine(x1*SCRW/screen_width, y1*SCRH/screen_height, x2*SCRW/screen_width, y2*SCRH/screen_height, color);
}

//获取x坐标
int32 sc_getX(int32 x){
    return x * SCRW/screen_width;
}

int32 sc_getY(int32 y){
    return y * SCRH/screen_height;
}
