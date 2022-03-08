#include <base.h>
#include "c/mrpedit/mrpedit.h"
//操作文件
char*file="安装包.mrp";
//工作目录
char*path="解压/";
int init(void)
{
//以下全部都用printf输出信息
//读取mrp信息
ReadMrpInfo(file);
//写入mrp信息,1，目标文件
WriteMrpInfo("pack.mrp");
//读取mrp内部文件列表，1 目标文件
 ReadMrpFiles(file);
//解压全部，1 目标文件， 2 工作目录，3，0为解压，其他为解包
ExtractAll (file,path,0);
//打包mrp1,数据结构，2 目标文件，3 工作目录，4 生成文件
PackagedMrp(pHead,file,path,"abc.mrp");
//释放使用内存
FreeFileData();
ShowMsg("输出完成...!请到printf.txt查看",0);

    return 0;
}

int event(int type, int p1, int p2)
{
    if(KY_DOWN == type)
    {
        switch(p1)
        {
        case _SRIGHT:
            exit();
            break;
        case _1:
            break;
        }
    }

    return 0;
}

int pause(void)
{
    return 0;
}

int resume(void)
{
    return 0;
}

void ShowMsg(char *Msg,int is_uni)
{
 int w,h;uint16 x,y;
 textwh(Msg,FALSE,0,&w,&h);
 if(w<SCRW)x=(SCRW-w)>>1;
 y=(SCRH-h)>>1;
 effsetcon(x-5,y-5,w+10,h+10,100,100,100);
 DrawVoidRect(x-5,y-5,w+10,h+10,255,0,0);
 dtext(Msg,x,y,255,255,250,is_uni,0);
 ref(x-5,y-5,w+10,h+10);
}
//画空心矩形
void DrawVoidRect(uint16 x,uint16 y,uint16 w,uint16 h,uint8 r,uint8 g,uint8 b)
{
 dline(x,y,x,y+h-1,r,g,b);
 dline(x+1,y,x+w-2,y,r,g,b);
 dline(x+w-1,y,x+w-1,y+h-1,r,g,b);
 dline(x+1,y+h-1,x+w-2,y+h-1,r,g,b);
}

