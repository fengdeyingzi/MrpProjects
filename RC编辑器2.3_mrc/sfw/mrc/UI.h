
#ifndef _UI_H_
#define _UI_H_
#include <mrc_base.h>
#include "xl.h"
enum
{
    MN_SL=4, //菜单选择

    MN_SR=5, //菜单返回

    MR_DLG=6, //对话框

    MR_FILE=10


};
enum
{
    DLG_SL=0,
    DLG_SR=1
};
enum
{
    FILE_SL=0,
    FILE_SR=1
};

enum
{
    FILE_ALL=1,//获取全部文件
    FILE_HZ=3,//获取指定后缀文件
    FILE_DIR=2//只获取文件目录
};

/*
enum {

    DLG_OK,         //对话框/文本框等的"确定"键被点击(选择)

    DLG_CANCEL  //对话框/文本框等的"取消"("返回")键被点击(选择)

};
*/

enum
{
    GB2312=0,//2
    UNICODE=1//3

};

enum
{
    UI_MENU=1,
    UI_DLG,
    UI_TEXT,
    UI_EDIT,
    UI_FILE
};

extern int liemax;
extern long menuref(long);
//extern void font_init();
extern int fonth;
void gdt(int x,int y,int w,int h,int len,int start,int liemax);
extern void linerect(int x,int y,int w,int h,int r, int g,int b);

extern int _event(int type,int p1,int p2);
extern int ui_menuevent(int type,int ,int);
int ui_dlgevent(int type,int p1,int p2);
int ui_fileevent(int type,int p1,int p2);
int ui_textevent(int type,int p1,int p2);

int k_ui=0;//判断是否为ui画面
int event(int type,int p1,int p2)
{
    if(k_ui==0)
    {
        _event(type,p1,p2);
    }

    else if(k_ui==UI_MENU)
    {
        ui_menuevent(type,p1,p2);
        return 0;
    }

    else if(k_ui==UI_TEXT)
    {
        ui_textevent(type,p1,p2);
        return 0;
    }
    else if(k_ui==UI_DLG)
    {
        ui_dlgevent(type,p1,p2);
        return 0;
    }
//else if(k_ui==UI_EDIT)
//ui_editevent(type,p1,p2);

    else if(k_ui==UI_FILE)
    {
        ui_fileevent(type,p1,p2);
        return 0;
    }

    return 0;
}


typedef struct
{
    char *title;//菜单标题(gb编码)
    char **text;//菜单列表
    int max;//菜单数目
    int start;//绘制菜单的起始项
    int gb;//光标位置
    int coding;//编码

    int type;//菜单类型
    int x;//菜单扩展：定义坐标
    int y;
    int w;
    int h;
} Menu ;
Menu *ui_menu;//全局菜单


int ui_menuevent(int type,int p1,int p2)
{

//printf("type=%d\n p1=%d\n p2=%d\n",type,p1,p2);
    if(type==KY_DOWN)
    {

        switch(p1)

        {

        case _UP:

            if(ui_menu->gb>0)ui_menu->gb--;
            else //光标在顶部时
            {
                if(ui_menu->start>0)
                {
                    ui_menu->start--;
                }
                else //到达列表顶部，返回底部
                {
                    if(ui_menu->max>liemax)
                    {
                        ui_menu->start=-liemax;
                        ui_menu->gb=liemax-1;
                    }
                    else //列表数不足以覆盖全屏时
                    {
                        ui_menu->start=0;
                        ui_menu->gb=ui_menu->max-1;
                    }
                }

            }

            menuref((long)ui_menu);
            

            break;



        case _DOWN:


            if(ui_menu->gb<liemax-1)
            {
                if(ui_menu->gb>=ui_menu->max-1)ui_menu->gb=0;
                else
                    ui_menu->gb++;
            }
            else //光标在底部时
            {
                if(ui_menu->start+liemax<ui_menu->max)
                {
                    ui_menu->start++;

                }
                else //列表到底之后返回顶部
                {
                    ui_menu->start=0;
                    ui_menu->gb=0;
                }

            }


            menuref((long)ui_menu);
            break;



        case _RIGHT:

            if(ui_menu->max>ui_menu->start+liemax)ui_menu->start+=liemax;
            if(ui_menu->start>ui_menu->max-liemax)ui_menu->start=ui_menu->max-liemax;
            if(ui_menu->start<0)ui_menu->start=0;
            menuref((long)ui_menu);
            break;



        case _LEFT:

            if(ui_menu->max>=liemax)
                ui_menu->start-=liemax;
            if(ui_menu->start<0)ui_menu->start=0;
            menuref((long)ui_menu);
            break;


        }

    }



    else if(type==KY_UP)
    {
        if(p1==_SRIGHT)
            _event(MN_SR,ui_menu->start+ui_menu->gb,p2);
        else if(p1==_SLEFT || p1==_SELECT)
            _event(MN_SL,ui_menu->start+ui_menu->gb,p2);
    }


    else if(type==MS_DOWN)
    {
//点中光标区域
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            ui_menu->gb=(p2-30)/26;
            menuref((long)ui_menu);
        }
//点中滚动条
        else if(p1>SCRW-30 && p1<SCRW && p2>30&& p2<SCRH-30)
        {
            ui_menuevent(KY_DOWN,_LEFT+p2/(SCRH/2),0);

        }
        /*
        //

        if(p1>0 &&p1<SCRW/3 && p2>SCRH-30 && p2<SCRH)
        list_out();
        //
        if(p1>SCRW* 2/3 && p1<SCRW && p2>SCRH-30 &&p2<SCRH)
        gb_ref();
        */

    }

    else if(type==MS_UP)
    {
        if((p2-30)/26==ui_menu->gb)
        {
            _event(MN_SL,ui_menu->start+ui_menu->gb,p2);


        }
//菜单确定键
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            k_ui=0;
            _event(MN_SL,ui_menu->start+ui_menu->gb,p2);

        }

//菜单返回键
        if(p1>SCRW* 2/3 && p1<SCRW && p2>SCRH-30 &&p2<SCRH)
        {
            k_ui=0;
            _event(MN_SR,ui_menu->start+ui_menu->gb,p2);

        }


    }


    return 0;
}

/*
创建一个菜单，并返回菜单句柄，title是菜单的标题。

输入:
title	菜单的标题，unicode编码，网络字节序。
num	菜单项的数目

返回:
      正整数   菜单句柄
      -1   失败
*/

long menucreate(char* title, int num)
{
    Menu *menu=malloc(sizeof(Menu));
    menu->title=title;
    menu->max=num;
    menu->text=malloc(3000);
    menu->start=0;
    menu->coding=UNICODE;
    font_init();
    menu->gb=0;
    liemax=(SCRH-60)/26;


//printf("创建菜单：%d",menu->max);
    ui_menu=menu;
    return (long)menu;
}


/*
    在菜单里设置一个菜单项，参数index是该菜单项的
    序号，当用户选中某个菜单项时，把index号通过
    event函数发送到应用中。

输入:
menu  菜单的句柄
text 菜单项的名字，使用unicode编码，网络字节序。
index    菜单项的index号（从0开始）

返回:
      0  成功
      -1   失败
*/

long menuset(long menu,  char *text, long index)
{
    Menu *me;
    me=(void *)menu;

    me->text[index]=text;

    return 0;
}

/*
显示菜单。当菜单显示时，如果用户选择了菜单上
的某一项，系统将构造Mythroad应用消息，通过
event函数传送给Mythroad应用，消息代码为MN_SL，
参数1为该菜单项的index。如果用户选择
了退出该菜单，系统将构造Mythroad应用消息，通过
event函数传送给Mythroad应用，消息代码为MN_SR。

输入:
menu 菜单的句柄

返回:
      0  成功
      -1   失败
*/
long menushow(long menu)
{
    Menu *me=(void*)menu;
    int i;
    k_ui=UI_MENU;
    cls(255,255,255);


//背景
    drect(0,30,SCRW,SCRH-60,240,240,240);

//光标
    drect(24,me->gb*26+30,SCRW-24,26,95,185,240);


    for(i=0; i<liemax; i++)

    {
        if(i>=me->max)
            break;




        dtext(me->text[me->start+i],28,i*26+30+(26-fonth)/2,40,40,40,me->coding,1);

    }
    gdt(SCRW-3,30,3,SCRH-60,me->max,me->start,liemax);

    /*
    uint8* input=me->title; int32 input_len=wstrlen(me->title); uint8* output=malloc(input_len); int32 output_len=0;
    u2c(input,input_len,&output,(int *)&input_len);
    printf("标题%d",input_len);
    titlebar(output);
    */
    drect(1,1,SCRW-2,28,220,220,220);
    linerect(0,0,SCRW-1,29,150,150,150);
    dtext(me->title,2,(30-fonth)/2,textcolor.r,textcolor.g,textcolor.b,me->coding,FONT);

    toolbar("确定","返回");

    ref(0,0,SCRW,SCRH);

    return 0;
}

/*
释放菜单。

输入:
menu 菜单的句柄

返回:
      0  成功
      -1   失败
*/
long menudel(long menu)
{
    Menu *ptr=(void*)menu;
    free(ptr->text);

    free(ptr);
    k_ui=0;
    return 0;
}

//设置菜单的编码
void menucod(long menu, int coding)
{
    Menu *me=(void*)menu;
    me->coding=coding;
}


/*
刷新菜单显示。

输入:
menu 菜单的句柄

返回:
      0  成功
      -1   失败
*/
long menuref(long menu)
{
    menushow(menu);
    return 0;
}



typedef struct
{
    char *title;
//标题
    char* text;
//文本
    long type;
//类型
    int coding;//编码
    int line;//当前行号
    int max;//总行数
    int font;//字体大小
    colorst color;//
    rectst rect;//
    char *lefttext;
    char *righttext;


} DLG;


int ui_dlgevent(int type,int p1,int p2)
{
    if(type==KY_DOWN)
    {



    }

    else if(type==KY_UP)
    {
        if(p1==_SLEFT)
        {
            _event(MR_DLG,DLG_SL,p2);
            k_ui=0;
        }
        else if(p1==_SRIGHT)
        {
            _event(MR_DLG,DLG_SR,p2);
            k_ui=0;
        }

    }

    else if(type==MS_DOWN)
    {


    }

    else if(type==MS_MOVE)
    {


    }
    else if(type==MS_UP)
    {


    }


    return 0;

}

/*
创建一个对话框，并返回对话框句柄。当对话框显
示时，如果用户按了对话框上的某个键，系统将构
造Mythroad应用消息，通过event函数传送给Mythroad应
用，消息类型为MR_DIALOG，参数为该按键的ID。
"确定"键ID为：0；"取消"键ID为：1。

输入:
title	对话框的标题，unicode编码，网络字节序。
text	对话框内容，unicode编码，网络字节序。
type	对话框类型：
      0：对话框有"确定"键。
      1：对话框有"确定"和"取消"键。
      2：对话框有 "返回"键。

返回:
      正整数   对话框句柄
      -1   失败
*/

long dlgcreate( char * title,  char * text, long type)
{
    DLG *dlg=malloc(sizeof(DLG));
    dlg->title=title;
    dlg->text=text;
    dlg->type=type;

    dlg->coding=UNICODE;
    dlg->line=0;
    dlg->font=1;

    dlg->rect.x=0;
    dlg->rect.y=0;
    dlg->rect.w=SCRW;
    dlg->rect.h=SCRH-60;
    dlg->color.r=50;
    dlg->color.g=50;
    dlg->color.b=50;

    switch(type)
    {
    case 0:
        dlg->lefttext="确定";
        dlg->righttext=NULL;
        break;
    case 1:
        dlg->lefttext="确定";
        dlg->righttext="返回";
        break;
    case 2:
        dlg->lefttext=NULL;
        dlg->righttext="返回";
    }


    return (long)dlg;
}

/*
释放对话框。

输入:
dialog  对话框的句柄

返回:
      0  成功
      -1   失败
*/
long dlgdel(long dialog)
{
    DLG *dlg=(void*)dialog;
    free(dlg->title);
    free(dlg->text);
    free(dlg);
    return 0;
}

/*
刷新对话框的显示。

输入:
dialog	对话框的句柄
title	对话框的标题，unicode编码，网络字节序。
text	对话框内容，unicode编码，网络字节序。
type	若type为-1，表示type不变。
对话框类型：
      0：对话框有"确定"键。
      1：对话框有"确定"和"取消"键。
      2：对话框有 "返回"键。

返回:
      0  成功
      -1   失败
*/
long dlgref(long dialog, char * title, char * text, long type)
{
    DLG *dlg=(void *)dialog;



    dlg->title=title;
    dlg->text=text;
    dlg->type=type;

    cls(255,255,255);
    linerect(0,30,SCRW-1,SCRH-31,68,168,240);
    dtextex(dlg->text,0,30+dlg->line*(fonth+2),&dlg->rect,&dlg->color,2+dlg->coding,dlg->font);

    toolbar(dlg->lefttext,dlg->righttext);
    ref(0,0,SCRW,SCRH);
    k_ui=UI_DLG;
    return 0;
}

int dlgset(long dialog,int coding)
{
    DLG *dlg=(void*)dialog;
    dlg->coding=coding;
    return 0;
}

int dlgshow(long dialog)
{
    DLG *dlg=(void*)dialog;
    return dlgref(dialog,dlg->title,dlg->text,dlg->type);
}





int ui_textevent(int type,int p1,int p2)
{
    if(type==KY_DOWN)
    {



    }

    else if(type==KY_UP)
    {
        if(p1==_SLEFT)
        {
            _event(MR_DLG,DLG_SL,p2);
            k_ui=0;
        }
        else if(p1==_SRIGHT)
        {
            _event(MR_DLG,DLG_SR,p2);
            k_ui=0;
        }

    }

    else if(type==MS_DOWN)
    {


    }

    else if(type==MS_MOVE)
    {


    }
    else if(type==MS_UP)
    {


    }


    return 0;

}


/*
创建一个文本框，并返回文本框句柄。文本框用来
显示只读的文字信息。文本框和对话框并没有本质
的区别，仅仅是显示方式上的不同，在使用上它们
的主要区别是：对话框的内容一般较短，文本框的
内容一般较长，对话框一般实现为弹出式的窗口，
文本框一般实现为全屏式的窗口。也可能在手机上
对话框和文本框使用了相同的方式实现。
文本框和对话框的消息参数是一样的。当文本框显
示时，如果用户选择了文本框上的某个键，系统将
构造Mythroad应用消息，通过event函数传送给Mythroad
平台，消息类型为MR_DIALOG，参数为该按键的ID。
"确定"键ID为：0；"取消"键ID为：1。

输入:
title	文本框的标题，unicode编码，网络字节序。
text	文本框内容，unicode编码，网络字节序。
type	文本框类型：
      0：文本框有"确定"键。
      1：文本框有"确定"和"取消"键。
      2：文本框有 "取消/返回"键。

返回:
      正整数   文本框句柄
      -1   失败
*/
long textcreate(char * title,  char * text, long type)
{
    DLG *dlg=malloc(sizeof(DLG));
    dlg->title=title;
    dlg->text=text;
    dlg->type=type;

    dlg->coding=UNICODE;
    dlg->line=0;
    dlg->font=1;

    dlg->rect.x=0;
    dlg->rect.y=0;
    dlg->rect.w=SCRW;
    dlg->rect.h=SCRH-60;
    dlg->color.r=50;
    dlg->color.g=50;
    dlg->color.b=50;

    switch(type)
    {
    case 0:
        dlg->lefttext="确定";
        dlg->righttext=NULL;
        break;
    case 1:
        dlg->lefttext="确定";
        dlg->righttext="返回";
        break;
    case 2:
        dlg->lefttext=NULL;
        dlg->righttext="返回";
    }
    dlg->line=0;
    printf("......");
    dlg->max=(int)unitextrow((unsigned short*)(dlg->text), dlg->font, dlg->rect.w);
    printf("%d",dlg->max);


    return (long)dlg;
}



/*
释放文本框。

输入:
text 文本框的句柄

返回:
      0  成功
      -1   失败
*/
long textdel(long dialog)
{
    DLG *dlg=(void*)dialog;
    free(dlg->title);
    free(dlg->text);
    free(dlg);
    return 0;
}


/*
刷新文本框显示。

输入:
handle	文本框的句柄
title	文本框的标题，unicode编码，网络字节序。
text	文本框内容，unicode编码，网络字节序。

返回:
      0  成功
      -1   失败
*/
long textref(long dialog,  char * title,  char * text)
{
    DLG *dlg=(void *)dialog;



    dlg->title=title;
    dlg->text=text;


    cls(255,255,255);
    linerect(0,30,SCRW-1,SCRH-31,68,168,240);
    dtextex(dlg->text,0,30+dlg->line*(fonth+2),&dlg->rect,&dlg->color,2+dlg->coding,dlg->font);

    gdt(SCRW-3,30,3,SCRH-60,dlg->max,dlg->line,(SCRH-60)/(fonth+2));

    toolbar(dlg->lefttext,dlg->righttext);
    printf(".....");
    printf("行数%d 起始位置%d ",dlg->max,dlg->line);
    ref(0,0,SCRW,SCRH);
    k_ui=UI_DLG;
    return 0;
}


int textset(long dialog,int coding)
{
    DLG *dlg=(void*)dialog;
    dlg->coding=coding;
    return 0;
}

int textshow(long dialog)
{
    DLG *dlg=(void*)dialog;
    return textref(dialog,dlg->title,dlg->text);
    printf("......");
}










//FILE文件
extern int FONT;
extern void toolbar(char*,char*);
extern void gdt(int x,int y,int w,int h,int len,int start,int liemax);
//extern int k_ui;




int liemax;//显示的列表文本行数

//int texth;
void find_txt(char *file,char *text);
void find_dir(char *file,char *text);
extern void find_hz(char *file,char *text);
extern long textListLoad(char *text,char **list_rc);
extern void linerect(int x,int ,int w,int h,int r,int g,int b);
extern colorst textcolor;

char *filepath;//路径
int fontw,fonth;


typedef struct
{
    uint16 *iconbmp;//图标
    char path[300];//路径
    char *hz;//后缀
    int fonth;//文字高度
    int font;//文本字体大小
    char *title;//标题
    char *buf;//列表字符缓存
    char **text;//列表文本
    int start;//显示的起始位置
    int gb;//光标
    int max;//列表项
    int liemax;//列表最大显示数目
    int type;//目录获取方式

} FileList;
FileList *ui_file;



long fileinit()
{
    liemax=(SCRH-60)/26;
    textwh("字",0,1,&fontw,&fonth);

    return 0;
}
//fileinit();

long filecreate(char *path,char *hz,int type)
{
    long len;
    FileList *filelist=malloc(sizeof(FileList));
    filepath=path;

    ui_file=filelist;
    //filelist->path=(char *)malloc(255);
    memset(filelist->path,0,300);
    sprintf(filelist->path, "%s",path);
    //strcat(filelist->path,path);
    //strcpy(filelist->path,path); 
    filelist->iconbmp=readmrpfile("fileicon.bmp",&len);
    filelist->hz=hz;
    filelist->type=type;
    filelist->fonth=fonth;
    filelist->font=1;
    filelist->title=NULL;
    filelist->text=malloc(3000);
    filelist->buf=malloc(30000);

    filelist->start=0;
    filelist->gb=0;
    filelist->liemax=(SCRH-60)/26;


    switch(filelist->type)
    {
    case FILE_ALL:
        //获取所有文件到文本
        find_txt(filelist->path,filelist->buf);
        break;
        //获取目录到文本
    case FILE_DIR:
        find_dir(filelist->path,filelist->buf);
        break;
        //获取指定文件到文本
    case FILE_HZ:
        find_hz(filelist->path,filelist->buf);
        break;
    }

    printf("文件类型%d\n",type);

//文本处理为列表字符
    filelist->max=textListLoad(filelist->buf,filelist->text);

    return (long)filelist;
}

int filesettitle(long file,char *title)
{
    FileList *filelist=(void*)file;
    filelist->title=title;
    return 0;
}
long filesethz(long file,char *hz)
{
    FileList *filelist=(void*)file;
    filelist->type=FILE_HZ;
    filelist->hz=hz;
    return 0;
}

//设置浏览路径
long fileregin(char *path)
{
    filepath=path;
    return 0;
}

//取得文件路径
char *filegetpath(long file)
{
    FileList *filelist=(void*)file;
    return filelist->path;
}

//取得文件名
char *filegetname(long file)
{
    FileList *filelist=(void*)file;
    return filelist->text[filelist->start+filelist->gb];

}

//取得文件全名
char *filegetfilename(long file)
{
    FileList *filelist=(void*)file;
    char text[300];
    sprintf(text,"%s%s",filelist->path,filelist->text[filelist->start+filelist->gb]);
    return text;
}


/*
//设置文件列表
long filetext(long file,char **text)
{
FileList *filelist=(void*)file;

return 0;
}
*/

//将文件列表加载到屏幕上
long fileshow(long file)
{
    FileList *filelist=(void*)file;

    int i=0;
    int icon=0;
    char *hz=NULL;
    char *out;
    out=malloc(300);
    printf("开始加载目录");
//列表背景
    drect(0,30,SCRW,SCRH-60,240,240,240);
//画光标
    drect(24,filelist->gb*26+30,SCRW-24,26,95,185,240);
    printf("画光标");
    for(i=0; i<filelist->liemax; i++)
    {
        if(i>=filelist->max)break;


        sprintf(out,"%s/%s", filelist->path, filelist->text[filelist->start+i]);
        //printf("合成文件名");

        if(filestate(out)==2)
//是目录
        {
            icon=1;
        }
        else
        {
            hz = strrchr(filelist->text[filelist->start+i],'.');

            if(hz!=NULL)
            {
                if(strcmp(hz,".wav")==0)icon=3;
                else if(strcmp(hz,".mp3")==0)icon=3;

                else if(strcmp(hz,".bmp")==0)icon=4;
                else if(strcmp(hz,".amr")==0)icon=3;
                else if(strcmp(hz,".txt")==0)icon=5;
                else if(strcmp(hz,".acc")==0)icon=3;
                else if(strcmp(hz,".lrc")==0)icon=5;
                else if(strcmp(hz,".mrp")==0)icon=6;
                else if(strcmp(hz,".MRP")==0)icon=6;
                else icon=0;
            }
        }
        bmpshowflip (filelist->iconbmp,4,i*26+30+5,16,16,16,6,0,icon*16,*(filelist->iconbmp));
        dtext (filelist->text[filelist->start+i],28,i*26+30+(26-filelist->fonth)/2,40,40,40,0,1);
        printf("显示文本");


    }

    //标题栏
    drect(1,1,SCRW-2,28,220,220,220);
    linerect(0,0,SCRW-1,29,150,150,150);
    dtext(filelist->title,2,(30-fonth)/2,textcolor.r,textcolor.g,textcolor.b,0,FONT);

    //菜单栏
    toolbar("确定","返回");
    //滚动条
    gdt (SCRW-3,30,3,SCRH-60,filelist->max,filelist->start,filelist->liemax);

    printf("加载完毕");
    ref(0,0,SCRW,SCRH);
    free(out);
    k_ui=UI_FILE;
    ui_file=filelist;

    return 0;
}

long fileref(long file)
{
//FileList *filelist=(void*)file;
    return fileshow(file);

}

long fileend(long file)
{
    FileList *filelist=(void*)file;
    free(filelist->buf);
    free(filelist->text);
    free(filelist->iconbmp);
    free(filelist);
    return 0;
}

int ui_fileevent(int type,int p1,int p2)
{
    FileList *filelist=ui_file;
    //printf("\ntype=%d\n p1=%d\n p2=%d\n",type,p1,p2);
//return 0;
    if(type==KY_DOWN)
    {

        switch(p1)

        {

        case _UP:

            printf("上\n");
            if(ui_file->gb>0)
            {
                ui_file->gb--;
            }

            else //光标在顶部时
            {
                if(ui_file->start>0)
                {
                    ui_file->start--;
                    //printf("光标在顶部时\n");
                }

                else //到达列表顶部，返回底部
                {
                    //printf("到达列表顶部，返回底部\n");
                    if(ui_file->max>liemax)
                    {

                        ui_file->start=ui_file->max-liemax;
                        ui_file->gb=liemax-1;
                    }
                    else //列表数不足以覆盖全屏时
                    {
                        ui_file->start=0;
                        ui_file->gb=ui_file->max-1;
                    }
                }

            }

            fileref((long)ui_file);
            return 0;

            break;



        case _DOWN:


            if(ui_file->gb<ui_file->liemax-1)
            {
                if(ui_file->gb>=ui_file->max-1)ui_file->gb=0;
                else
                    ui_file->gb++;
            }
            else //光标在底部时
            {
                if(ui_file->start+ui_file->liemax<ui_file->max)
                {
                    ui_file->start++;

                }
                else //列表到底之后返回顶部
                {
                    ui_file->start=0;
                    ui_file->gb=0;
                }

            }


            fileref((long)ui_file);
            break;



        case _RIGHT:

            if(ui_file->max>ui_file->start+ui_file->liemax)ui_file->start+=ui_file->liemax;
            if(ui_file->start>ui_file->max-ui_file->liemax)ui_file->start=ui_file->max-ui_file->liemax;
            if(ui_file->start<0)ui_file->start=0;
            fileref((long)ui_file);
            break;



        case _LEFT:

            if(ui_file->max>=ui_file->liemax)
                ui_file->start-=ui_file->liemax;
            if(ui_file->start<0)ui_file->start=0;
            fileref((long)ui_file);
            break;


        }

    }



    else if(type==KY_UP)
    {
        if(p1==_SRIGHT)
        {
        	k_ui=0;
          _event(MR_FILE,FILE_SR,p2);
            
        }


        else if(p1==_SLEFT || p1==_SELECT)
        {
            //当选中的是目录，则打开它
            if(ui_file->type==FILE_ALL)
            {
                if (filestate(filegetfilename((long)ui_file))==2)
                {


                    strcat(filelist->path,filelist->text[filelist->start+filelist->gb]);

                    ui_file->gb=0;
                    ui_file->start=0;
                    //获取所有文件到文本
                    find_txt(ui_file->path,ui_file->buf);
                    //文本处理为列表字符
                    ui_file->max=textListLoad(ui_file->buf,ui_file->text);
                    fileref((long)ui_file);
                }
                else
                {
                    _event(MR_FILE,FILE_SL,p2);
                    k_ui=0;
                }

            }
            if(ui_file->type==FILE_HZ)
            {
            	 k_ui=0;
                _event(MR_FILE,FILE_SL,p2);
               
            }
        }
    }


    else if(type==MS_DOWN)
    {
//点中光标区域
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            ui_file->gb=(p2-30)/26;
            fileref((long)ui_file);
        }
//点中滚动条
        else if(p1>SCRW-30 && p1<SCRW && p2>30&& p2<SCRH-30)
        {
            ui_fileevent(KY_DOWN,_LEFT+p2/(SCRH/2),0);

        }


    }

    else if(type==MS_UP)
    {
        if((p2-30)/26==ui_file->gb)//点中光标 
        {
        	k_ui=0;
            _event(MR_FILE,FILE_SL,p2);
        }
//菜单确定键
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            k_ui=0;
            _event(MR_FILE,FILE_SL,p2);

        }

//菜单返回键
        if(p1>SCRW* 2/3 && p1<SCRW && p2>SCRH-30 &&p2<SCRH)
        {
            k_ui=0;
            _event(MR_FILE,FILE_SR,p2);

        }


    }


    return 0;


}










long textListLoad(char *text,char **list_rc)//将文本处理为字符序
{
    char *scr;
    int i=0;
    int list_n;
    while(text!=NULL)
    {
//查找换行符
        scr=strstr(text,"\x0a");
        if(scr==NULL)break;
//转换为rc字符指针序
        list_rc[i]=text;
        text=scr+1;
//将换行符替换为0
        if(scr!=NULL)*scr=0;
        else break;
        if(*(scr-1)=='\x0d')*(scr-1)=0;
        i++;
    }
    list_n=i;
    return list_n;
}




void find_txt(char *file,char *text)//搜索文件到文本，参数。：路径 文本
{
    int32 f;
//int32 i;
    int32 ret= 0;
    char *scr;
    char *temp=malloc(300);
    int list_n=0;

    f=findstart(file,temp,72);
    scr=text;
    *scr=0;
    if(temp!=NULL&& *temp!='.')
    {
        list_n++;
        strcpy(scr,temp);
        strcat(scr,"\x0d\x0a");
    }




    while(ret==0)
    {
        ret =findnext(f,temp,72);
        if(ret)break;
        if(temp==NULL)break;
        if(*temp!='.')
        {
            list_n++;
            strcat(scr,temp);
            strcat(scr,"\x0d\x0a");
            //scr+=strlen(scr);
        }
    }
    findstop(f);
    free(temp);


}


void find_dir(char *file,char *text)//搜索文件到文本，参数。：路径 文本
{
    int32 f;
    int list_n=0;

    int32 ret= 0;
    char *scr;
    char *temp=malloc(300);

    char *ptr=temp;

    sprintf(temp,"%s/",file);
    ptr+=strlen(temp);


    f=findstart(file,ptr,72);
    scr=text;
    *scr=0;
    //检测是否为文件夹
    if(filestate(temp)==2)
        if(ptr!=NULL&& *ptr!='.')
        {
            list_n++;
            strcpy(scr,ptr);
            strcat(scr,"\x0d\x0a");
        }




    while(ret==0)
    {
        ret =findnext(f,ptr,72);
        if(ret)break;
        if(ptr==NULL)break;
        if(filestate(temp)==2 && *ptr!='.')
        {
            list_n++;
            strcat(scr,ptr);
            strcat(scr,"\x0d\x0a");
            //scr+=strlen(scr);
        }
    }
    findstop(f);
    free(temp);


}


void find_hz(char *file,char *text)//搜索文件到文本，参数。：路径 文本
{
    int32 f;
// int32 i;
    int32 ret= 0;
    char *scr;
    char *hz;
    char *temp=malloc(300);
    int list_n=0;
    char *ptr=temp;

    sprintf(temp,"%s/",file);
    ptr+=strlen(temp);



    f=findstart(file,ptr,72);
    scr=text;
    *scr=0;

    hz = strrchr(ptr,'.');

    //检测是否为文件
    if(hz!=NULL)
        if(filestate(temp)==1)
            if(strcmp(hz,ui_file->hz)==0)
                if(ptr!=NULL&& *ptr!='.')
                {
                    list_n++;
                    strcpy(scr,ptr);
                    strcat(scr,"\x0d\x0a");
                }




    while(ret==0)
    {
        ret =findnext(f,ptr,72);
        if(ret)break;
        if(ptr==NULL)break;
        hz = strrchr(ptr,'.');
        if(hz!=NULL)
            if(strcmp(hz,ui_file->hz)==0)
                if(filestate(temp)==1 && *ptr!='.')
                {
                    list_n++;
                    strcat(scr,ptr);
                    strcat(scr,"\x0d\x0a");
                    //scr+=strlen(scr);
                }
    }
    findstop(f);
    free(temp);


}




















//画滚动条，参数：坐标x，坐标y，宽度，高度，列表数量，列表起始位置，列表显示的最大数量
void gdt(int x,int y,int w,int h,int len,int start,int liemax)
{
    if(liemax>len)liemax=len;
    drect(x,y,w,h,90,90,90);
    drect(x,y+h*start/len,w,h*liemax/len,100,190,240);
}

#endif











