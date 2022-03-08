#include <mrc_base.h>

int32 SCRW,SCRH;
//sound
enum{_MIDI=1,_WAVE,_MP3,_AMR,_PCM,_M4A,_AMR_WB};

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
    KY_DOWN,     //按键按下
    KY_UP,       //按键释放
    MS_DOWN,     //鼠标按下
    MS_UP,       //鼠标释放
    MN_SLT, //菜单选择
    MN_RET, //菜单返回
    MR_DIALOG, //对话框
    MS_MOVE=12   //鼠标移动
};

enum {
    DLG_OK,         //对话框/文本框等的"确定"键被点击(选择)
    DLG_CANCEL  //对话框/文本框等的"取消"("返回")键被点击(选择)
};

enum
{
    SEEK_SET,             //从文件起始开始
    SEEK_CUR,             //从当前位置开始
    SEEK_END             //从文件末尾开始
};
enum
{
    IS_FILE=1,      //文件
    IS_DIR=2,      //目录
    IS_INVALID=8  //无效(非文件、非目录)
};

typedef struct {
    uint16            x;
    uint16            y;
    uint16            w;
    uint16            h;
}rectst;

typedef struct {
    uint8            r;
    uint8            g;
    uint8            b;
}colorst;



enum
{
    SHADE_UPDOWN,       //从上到下
    SHADE_LEFTRIGHT,    //从左到右
    SHADE_DOWNUP,       //从下到上
    SHADE_RIGHTLEFT     //从右到左
};

void mpc_init()
{
    mr_screeninfo screen;

    mrc_getScreenInfo(&screen);
    SCRW = screen.width;
    SCRH = screen.height;
}

void mpc_exit(){
    
}

void *readmrpfile(char *filename,int32 *len)
{
return mrc_readFileFromMrp (filename,len,0);


}

void freefiledata(void *data,int32 filelen)
{
 mrc_freeFileData(data,(int32)filelen);
}


void ShadeRect(int x, int y, int w, int h, int AR, int AG, int AB, int BR, int BG, int BB, int mode)
{
    int16 i,j,t;

    BR-=AR; BG-=AG; BB-=AB;
    switch(mode)
    {
    case SHADE_UPDOWN:
        t=x+w-1;
        for(i=0;     i<h;     i++)
            mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*i/h),(uint8)(AG+BG*i/h),(uint8)(AB+BB*i/h));
        return;
    case SHADE_DOWNUP:
        t=x+w-1;
        for(i=h-1,j=0;    i>=0;    i--,j++)
            mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*j/h),(uint8)(AG+BG*j/h),(uint8)(AB+BB*j/h));
        return;
    case SHADE_LEFTRIGHT:
        t=y+h-1;
        for(i=0;     i<w;     i++)
            mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*i/w),(uint8)(AG+BG*i/w),(uint8)(AB+BB*i/w));
        return;
    case SHADE_RIGHTLEFT:
        t=y+h-1;
        for(i=w-1,j=0;    i>=0;    i--,j++)
            mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*j/w),(uint8)(AG+BG*j/w),(uint8)(AB+BB*j/w));
        return;
    }
}

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode)
{
    //RGBA+(RGBB-RGBA)*N/Step
    int32 AR,AG,AB;
    int32 BR,BG,BB;

    AR = (uint8)(pixelA >> 16) & 0xff;
    AG = (uint8)(pixelA >> 8) & 0xff;
    AB = (uint8)(pixelA) & 0xff;

    BR = (uint8)((pixelB >> 16) & 0xff);
    BG = (uint8)((pixelB >> 8) & 0xff);
    BB = (uint8)((pixelB) & 0xff);
    ShadeRect(x,y,w,h,AR,AG,AB,BR,BG,BB,mode);

}

