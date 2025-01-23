//加载系统头文件base.h
#include <base.h>
#include "android.h"
#include "CheckBox.h"

 void helloworld()
{
    //用指定颜色清除屏幕
    cls(0,0,0);
    //画文字
    dtext ( "helloworld", 0, 0, 255, 255, 255, 0, 1);
    //刷新屏幕
    ref(0,0,SCRW,SCRH);
}

void click_ok(int id)
{
 printf("%s：%d\n","点击",id);
 toast("点击",0);
 draw_main();
 //printf("点\n");
// return 0;
 }


CheckBox *view;


//绘制画面
void draw_main()
{
    cls(240,240,240);
    //绘制按钮
    check_draw(view);
    
    //刷新屏幕
    ref(0,0,SCRW,SCRH);
 
}



//入口函数，程序启动时开始执行
int init()
{
 int DECVIDE_WIDTH=0;
 int DECVIDE_HEIGHT=0;
 if(SCRW<SCRH)
 {
  DECVIDE_WIDTH=720;
  DECVIDE_HEIGHT = 720*SCRH/SCRW;
 }
 else
 {
  DECVIDE_WIDTH=720;
  DECVIDE_HEIGHT = 720*SCRW/SCRH;
 }
 
 
 
 setscrsize(720,720*DECVIDE_HEIGHT/DECVIDE_WIDTH);
    //创建按钮
    view = check_create(20,20,720-40,220);
    
    //设置点击事件
    check_setonclick(view, "click_ok");
 
    //设置控件id
    check_setid(view,10);
    
    //设置背景色 蓝色
    check_setBackgroundColor(view,0xffa0a0a0);
    
    draw_main();
    return 0;
}

void exit_free()
{
 check_free(view);
 
}


//event函数，接收消息事件
int event(int type, int p1, int p2)
{
 //监听event事件
 check_event(view,type,p1,p2);
 
    if(KY_UP == type)
    {
        switch(p1)
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

//应用暂停，当程序进入后台或变为不可见时会调用此函数
int pause()
{
    return 0;
}

//应用恢复，程序变为可见时调用此函数
int resume()
{
    return 0;
}
 