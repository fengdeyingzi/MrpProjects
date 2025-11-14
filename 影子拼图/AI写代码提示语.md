
我要在屏幕为240*320的功能机上实现一个迷宫游戏,请帮我用C语言代码完成
其中,功能机的C语言代码遵循以下设定:
```c
typedef  unsigned short     uint16;      //有符号16bit整型
typedef  unsigned long int  uint32;      //无符号32bit整型
typedef  long int                int32;      //有符号32bit整型
typedef  unsigned char      uint8;        //无符号8bit整型
typedef  signed char          int8;        //有符号8bit整型
typedef  signed short         int16;       //有符号16bit整型
//基本按键值（未定义的其他按键也可以使用，但需知道其键值）
enum {  
   _0,           //按键 0
   _1,           //按键 1
   _2,           //按键 2
   _3,           //按键 3
   _4,           //按键 4
   _5,           //按键 5
   _6,           //按键 6
   _7,           //按键 7
   _8,           //按键 8
   _9,           //按键 9
   _STAR,        //按键 *
   _POUND,       //按键 #
   _UP,          //按键 上
   _DOWN,        //按键 下
   _LEFT,        //按键 左
   _RIGHT,       //按键 右
   _SLEFT=17,    //按键 左软键
   _SRIGHT,      //按键 右软键
   _SEND,        //按键 接听键
   _SELECT       //按键 确认/选择（若方向键中间有确认键，建议设为该键）
};

//基本事件（其他事件需自己定义）
enum {
    KY_DOWN, 	 //按键按下
    KY_UP,       //按键释放
    MS_DOWN, 	 //鼠标按下
    MS_UP 	     //鼠标释放
};
extern int16 SCRW;   //表示屏幕宽度
extern int16 SCRH ; //表示屏幕高度

/*
程序入口函数,类似于main
*/
int32 mrc_init(void):
/*
程序事件函数,主要用于接受触屏/按键事件
当code==KY_UP或KY_DOWN时表示案件按键事件,param0为按键键值
当code==MS_UP或MS_DOWN时表示触屏事件 param0和param1为点击的屏幕坐标
*/
int32 mrc_event(int32 code, int32 param0, int32 param1);
/*
程序退出事件函数,在这里处理释放内存等操作
*/
int32 mrc_exitApp();

```
功能机可用以下绘图函数:
```
/*
使用指定的颜色清除屏幕。
输入:
r,g,b	          绘制颜色
*/
void mrc_clearScreen(int32 r, int32 g, int32 b);

/*
刷新屏幕指定的区域。该函数的功能是将mythroad屏幕
缓冲指定的区域刷新到屏幕上。
输入:
x, y, w, h	       屏幕指定的区域（左上角为（x,y），宽高
                        为（w,h））
*/
void mrc_refreshScreen(int16 x, int16 y, uint16 w, uint16 h);


/*
绘制矩形于指定区域。
输入:
x,y,w,h:	位置
r,g,b	     绘制颜色
*/
void mrc_drawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);

/*
绘制线段。
输入:
x1,y1,x2,y2:	起末点位置
r,g,b	          绘制颜色
*/
void mrc_drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 r, uint8 g, uint8 b);

/*
在屏幕的指定位置绘制点。
输入:
x, y	           绘制点的位置
r,g,b	          绘制颜色
*/
void mrc_drawPointEx(int16 x, int16 y, int32 r, int32 g, int32 b);
```