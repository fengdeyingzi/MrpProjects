#include <base.h>
#include "c/mrpedit/mrpedit.h"
//�����ļ�
char*file="��װ��.mrp";
//����Ŀ¼
char*path="��ѹ/";
int init(void)
{
//����ȫ������printf�����Ϣ
//��ȡmrp��Ϣ
ReadMrpInfo(file);
//д��mrp��Ϣ,1��Ŀ���ļ�
WriteMrpInfo("pack.mrp");
//��ȡmrp�ڲ��ļ��б�1 Ŀ���ļ�
 ReadMrpFiles(file);
//��ѹȫ����1 Ŀ���ļ��� 2 ����Ŀ¼��3��0Ϊ��ѹ������Ϊ���
ExtractAll (file,path,0);
//���mrp1,���ݽṹ��2 Ŀ���ļ���3 ����Ŀ¼��4 �����ļ�
PackagedMrp(pHead,file,path,"abc.mrp");
//�ͷ�ʹ���ڴ�
FreeFileData();
ShowMsg("������...!�뵽printf.txt�鿴",0);

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
//�����ľ���
void DrawVoidRect(uint16 x,uint16 y,uint16 w,uint16 h,uint8 r,uint8 g,uint8 b)
{
 dline(x,y,x,y+h-1,r,g,b);
 dline(x+1,y,x+w-2,y,r,g,b);
 dline(x+w-1,y,x+w-1,y+h-1,r,g,b);
 dline(x+1,y+h-1,x+w-2,y+h-1,r,g,b);
}

