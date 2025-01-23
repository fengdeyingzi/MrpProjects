// 加载系统头文件base.h
#include <base.h>
#include "android.h"
#include "SeekBar.h"
#include "View.h"
#include "TextView.h"

void helloworld()
{
    // 用指定颜色清除屏幕
    cls(0, 0, 0);
    // 画文字
    dtext("helloworld", 0, 0, 255, 255, 255, 0, 1);
    // 刷新屏幕
    ref(0, 0, SCRW, SCRH);
}

void onColor(int id)
{
    char temp[200];
    int a = seek_getProgress(seek_a);
    int r = seek_getProgress(seek_r);
    int g = seek_getProgress(seek_g);
    int b = seek_getProgress(seek_b);

    int color = (a << 24) | (r << 16) | (g << 8) | b;

    view_setBackgroundColor(view_color, color);
    sprintf(temp, "#%02x%x%x%x", a, r, g, b);
    textview_setText(text_color, temp);
    draw_main();
}

void onRed(int id)
{
}

void onGreen(int id)
{
}

void onBlue(int id)
{
}

View *view_color;
SeekBar *seek_a, *seek_r, *seek_g, *seek_b;
TextView *text_color;

// 绘制画面
void draw_main()
{
    cls(240, 240, 240);

    // 绘制
    view_draw(view_color);
    seek_draw(seek_a);
    seek_draw(seek_r);
    seek_draw(seek_g);
    seek_draw(seek_b);
    textview_draw(text_color);
    // 刷新屏幕
    ref(0, 0, SCRW, SCRH);
}

// 入口函数，程序启动时开始执行
int init()
{
    int DECVIDE_WIDTH = 0;
    int DECVIDE_HEIGHT = 0;
    if (SCRW < SCRH)
    {
        DECVIDE_WIDTH = 720;
        DECVIDE_HEIGHT = 720 * SCRH / SCRW;
    }
    else
    {
        DECVIDE_WIDTH = 720;
        DECVIDE_HEIGHT = 720 * SCRW / SCRH;
    }

    //  setscrsize(720,720*DECVIDE_HEIGHT/DECVIDE_WIDTH);
    // 创建
    view_color = view_create(30 * SCRW / DECVIDE_WIDTH, 150 * SCRW / DECVIDE_WIDTH, SCRW - 60 * SCRW / DECVIDE_WIDTH, 90 * SCRW / DECVIDE_WIDTH);
    seek_a = seek_create(10 * SCRW / DECVIDE_WIDTH, 300 * SCRW / DECVIDE_WIDTH, SCRW - 20 * SCRW / DECVIDE_WIDTH, 80 * SCRW / DECVIDE_WIDTH);
    seek_r = seek_create(10 * SCRW / DECVIDE_WIDTH, 400 * SCRW / DECVIDE_WIDTH, SCRW - 20 * SCRW / DECVIDE_WIDTH, 80 * SCRW / DECVIDE_WIDTH);
    seek_g = seek_create(10 * SCRW / DECVIDE_WIDTH, 500 * SCRW / DECVIDE_WIDTH, SCRW - 20 * SCRW / DECVIDE_WIDTH, 80 * SCRW / DECVIDE_WIDTH);
    seek_b = seek_create(10 * SCRW / DECVIDE_WIDTH, 600 * SCRW / DECVIDE_WIDTH, SCRW - 20 * SCRW / DECVIDE_WIDTH, 80 * SCRW / DECVIDE_WIDTH);
    text_color = textview_create(30 * SCRW / DECVIDE_WIDTH, 150 * SCRW / DECVIDE_WIDTH, SCRW - 60 * SCRW / DECVIDE_WIDTH, 90 * SCRW / DECVIDE_WIDTH);
    textview_setpadding(text_color,10*SCRW / DECVIDE_WIDTH);
    
    view_setMode(view_color, 2);

    seek_setMax(seek_a, 255);
    seek_setMax(seek_r, 255);
    seek_setMax(seek_g, 255);
    seek_setMax(seek_b, 255);
    seek_setProgress(seek_a, 255);
    seek_setColor(seek_a, 0xff909090);
    seek_setColor(seek_r, 0xfff05d6e);
    seek_setColor(seek_g, 0xff70de7d);
    seek_setColor(seek_b, 0xff60a0f0);

    seek_onProgressChanged(seek_a, "onColor");
    seek_onProgressChanged(seek_r, "onColor");
    seek_onProgressChanged(seek_g, "onColor");
    seek_onProgressChanged(seek_b, "onColor");

    onColor(0);
    return 0;
}

void exit_free()
{
    view_free(view_color);
    seek_free(seek_a);
    seek_free(seek_r);
    seek_free(seek_g);
    seek_free(seek_b);
    textview_free(text_color);
}

// event函数，接收消息事件
int event(int type, int p1, int p2)
{
    // 监听event事件
    seek_event(seek_a, type, p1, p2);
    seek_event(seek_r, type, p1, p2);
    seek_event(seek_g, type, p1, p2);
    seek_event(seek_b, type, p1, p2);
    view_event(view_color, type, p1, p2);
    if (KY_UP == type)
    {
        switch (p1)
        {
        case _BACK:
            exit_free();
            exit();
            break;
        case _MENU:
            break;
        }
    }

    return 0;
}

// 应用暂停，当程序进入后台或变为不可见时会调用此函数
int pause()
{
    return 0;
}

// 应用恢复，程序变为可见时调用此函数
int resume()
{
    return 0;
}
