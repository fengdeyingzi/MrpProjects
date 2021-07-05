
#ifndef _UI_H_
#define _UI_H_
#include <mrc_base.h>
#include "xl.h"
enum
{
    MN_SL=4, //�˵�ѡ��

    MN_SR=5, //�˵�����

    MR_DLG=6, //�Ի���

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
    FILE_ALL=1,//��ȡȫ���ļ�
    FILE_HZ=3,//��ȡָ����׺�ļ�
    FILE_DIR=2//ֻ��ȡ�ļ�Ŀ¼
};

/*
enum {

    DLG_OK,         //�Ի���/�ı���ȵ�"ȷ��"�������(ѡ��)

    DLG_CANCEL  //�Ի���/�ı���ȵ�"ȡ��"("����")�������(ѡ��)

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

int k_ui=0;//�ж��Ƿ�Ϊui����
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
    char *title;//�˵�����(gb����)
    char **text;//�˵��б�
    int max;//�˵���Ŀ
    int start;//���Ʋ˵�����ʼ��
    int gb;//���λ��
    int coding;//����

    int type;//�˵�����
    int x;//�˵���չ����������
    int y;
    int w;
    int h;
} Menu ;
Menu *ui_menu;//ȫ�ֲ˵�


int ui_menuevent(int type,int p1,int p2)
{

//printf("type=%d\n p1=%d\n p2=%d\n",type,p1,p2);
    if(type==KY_DOWN)
    {

        switch(p1)

        {

        case _UP:

            if(ui_menu->gb>0)ui_menu->gb--;
            else //����ڶ���ʱ
            {
                if(ui_menu->start>0)
                {
                    ui_menu->start--;
                }
                else //�����б��������صײ�
                {
                    if(ui_menu->max>liemax)
                    {
                        ui_menu->start=-liemax;
                        ui_menu->gb=liemax-1;
                    }
                    else //�б��������Ը���ȫ��ʱ
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
            else //����ڵײ�ʱ
            {
                if(ui_menu->start+liemax<ui_menu->max)
                {
                    ui_menu->start++;

                }
                else //�б���֮�󷵻ض���
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
//���й������
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            ui_menu->gb=(p2-30)/26;
            menuref((long)ui_menu);
        }
//���й�����
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
//�˵�ȷ����
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            k_ui=0;
            _event(MN_SL,ui_menu->start+ui_menu->gb,p2);

        }

//�˵����ؼ�
        if(p1>SCRW* 2/3 && p1<SCRW && p2>SCRH-30 &&p2<SCRH)
        {
            k_ui=0;
            _event(MN_SR,ui_menu->start+ui_menu->gb,p2);

        }


    }


    return 0;
}

/*
����һ���˵��������ز˵������title�ǲ˵��ı��⡣

����:
title	�˵��ı��⣬unicode���룬�����ֽ���
num	�˵������Ŀ

����:
      ������   �˵����
      -1   ʧ��
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


//printf("�����˵���%d",menu->max);
    ui_menu=menu;
    return (long)menu;
}


/*
    �ڲ˵�������һ���˵������index�Ǹò˵����
    ��ţ����û�ѡ��ĳ���˵���ʱ����index��ͨ��
    event�������͵�Ӧ���С�

����:
menu  �˵��ľ��
text �˵�������֣�ʹ��unicode���룬�����ֽ���
index    �˵����index�ţ���0��ʼ��

����:
      0  �ɹ�
      -1   ʧ��
*/

long menuset(long menu,  char *text, long index)
{
    Menu *me;
    me=(void *)menu;

    me->text[index]=text;

    return 0;
}

/*
��ʾ�˵������˵���ʾʱ������û�ѡ���˲˵���
��ĳһ�ϵͳ������MythroadӦ����Ϣ��ͨ��
event�������͸�MythroadӦ�ã���Ϣ����ΪMN_SL��
����1Ϊ�ò˵����index������û�ѡ��
���˳��ò˵���ϵͳ������MythroadӦ����Ϣ��ͨ��
event�������͸�MythroadӦ�ã���Ϣ����ΪMN_SR��

����:
menu �˵��ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
long menushow(long menu)
{
    Menu *me=(void*)menu;
    int i;
    k_ui=UI_MENU;
    cls(255,255,255);


//����
    drect(0,30,SCRW,SCRH-60,240,240,240);

//���
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
    printf("����%d",input_len);
    titlebar(output);
    */
    drect(1,1,SCRW-2,28,220,220,220);
    linerect(0,0,SCRW-1,29,150,150,150);
    dtext(me->title,2,(30-fonth)/2,textcolor.r,textcolor.g,textcolor.b,me->coding,FONT);

    toolbar("ȷ��","����");

    ref(0,0,SCRW,SCRH);

    return 0;
}

/*
�ͷŲ˵���

����:
menu �˵��ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
long menudel(long menu)
{
    Menu *ptr=(void*)menu;
    free(ptr->text);

    free(ptr);
    k_ui=0;
    return 0;
}

//���ò˵��ı���
void menucod(long menu, int coding)
{
    Menu *me=(void*)menu;
    me->coding=coding;
}


/*
ˢ�²˵���ʾ��

����:
menu �˵��ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
long menuref(long menu)
{
    menushow(menu);
    return 0;
}



typedef struct
{
    char *title;
//����
    char* text;
//�ı�
    long type;
//����
    int coding;//����
    int line;//��ǰ�к�
    int max;//������
    int font;//�����С
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
����һ���Ի��򣬲����ضԻ����������Ի�����
ʾʱ������û����˶Ի����ϵ�ĳ������ϵͳ����
��MythroadӦ����Ϣ��ͨ��event�������͸�MythroadӦ
�ã���Ϣ����ΪMR_DIALOG������Ϊ�ð�����ID��
"ȷ��"��IDΪ��0��"ȡ��"��IDΪ��1��

����:
title	�Ի���ı��⣬unicode���룬�����ֽ���
text	�Ի������ݣ�unicode���룬�����ֽ���
type	�Ի������ͣ�
      0���Ի�����"ȷ��"����
      1���Ի�����"ȷ��"��"ȡ��"����
      2���Ի����� "����"����

����:
      ������   �Ի�����
      -1   ʧ��
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
        dlg->lefttext="ȷ��";
        dlg->righttext=NULL;
        break;
    case 1:
        dlg->lefttext="ȷ��";
        dlg->righttext="����";
        break;
    case 2:
        dlg->lefttext=NULL;
        dlg->righttext="����";
    }


    return (long)dlg;
}

/*
�ͷŶԻ���

����:
dialog  �Ի���ľ��

����:
      0  �ɹ�
      -1   ʧ��
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
ˢ�¶Ի������ʾ��

����:
dialog	�Ի���ľ��
title	�Ի���ı��⣬unicode���룬�����ֽ���
text	�Ի������ݣ�unicode���룬�����ֽ���
type	��typeΪ-1����ʾtype���䡣
�Ի������ͣ�
      0���Ի�����"ȷ��"����
      1���Ի�����"ȷ��"��"ȡ��"����
      2���Ի����� "����"����

����:
      0  �ɹ�
      -1   ʧ��
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
����һ���ı��򣬲������ı��������ı�������
��ʾֻ����������Ϣ���ı���ͶԻ���û�б���
�����𣬽�������ʾ��ʽ�ϵĲ�ͬ����ʹ��������
����Ҫ�����ǣ��Ի��������һ��϶̣��ı����
����һ��ϳ����Ի���һ��ʵ��Ϊ����ʽ�Ĵ��ڣ�
�ı���һ��ʵ��Ϊȫ��ʽ�Ĵ��ڡ�Ҳ�������ֻ���
�Ի�����ı���ʹ������ͬ�ķ�ʽʵ�֡�
�ı���ͶԻ������Ϣ������һ���ġ����ı�����
ʾʱ������û�ѡ�����ı����ϵ�ĳ������ϵͳ��
����MythroadӦ����Ϣ��ͨ��event�������͸�Mythroad
ƽ̨����Ϣ����ΪMR_DIALOG������Ϊ�ð�����ID��
"ȷ��"��IDΪ��0��"ȡ��"��IDΪ��1��

����:
title	�ı���ı��⣬unicode���룬�����ֽ���
text	�ı������ݣ�unicode���룬�����ֽ���
type	�ı������ͣ�
      0���ı�����"ȷ��"����
      1���ı�����"ȷ��"��"ȡ��"����
      2���ı����� "ȡ��/����"����

����:
      ������   �ı�����
      -1   ʧ��
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
        dlg->lefttext="ȷ��";
        dlg->righttext=NULL;
        break;
    case 1:
        dlg->lefttext="ȷ��";
        dlg->righttext="����";
        break;
    case 2:
        dlg->lefttext=NULL;
        dlg->righttext="����";
    }
    dlg->line=0;
    printf("......");
    dlg->max=(int)unitextrow((unsigned short*)(dlg->text), dlg->font, dlg->rect.w);
    printf("%d",dlg->max);


    return (long)dlg;
}



/*
�ͷ��ı���

����:
text �ı���ľ��

����:
      0  �ɹ�
      -1   ʧ��
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
ˢ���ı�����ʾ��

����:
handle	�ı���ľ��
title	�ı���ı��⣬unicode���룬�����ֽ���
text	�ı������ݣ�unicode���룬�����ֽ���

����:
      0  �ɹ�
      -1   ʧ��
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
    printf("����%d ��ʼλ��%d ",dlg->max,dlg->line);
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










//FILE�ļ�
extern int FONT;
extern void toolbar(char*,char*);
extern void gdt(int x,int y,int w,int h,int len,int start,int liemax);
//extern int k_ui;




int liemax;//��ʾ���б��ı�����

//int texth;
void find_txt(char *file,char *text);
void find_dir(char *file,char *text);
extern void find_hz(char *file,char *text);
extern long textListLoad(char *text,char **list_rc);
extern void linerect(int x,int ,int w,int h,int r,int g,int b);
extern colorst textcolor;

char *filepath;//·��
int fontw,fonth;


typedef struct
{
    uint16 *iconbmp;//ͼ��
    char path[300];//·��
    char *hz;//��׺
    int fonth;//���ָ߶�
    int font;//�ı������С
    char *title;//����
    char *buf;//�б��ַ�����
    char **text;//�б��ı�
    int start;//��ʾ����ʼλ��
    int gb;//���
    int max;//�б���
    int liemax;//�б������ʾ��Ŀ
    int type;//Ŀ¼��ȡ��ʽ

} FileList;
FileList *ui_file;



long fileinit()
{
    liemax=(SCRH-60)/26;
    textwh("��",0,1,&fontw,&fonth);

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
        //��ȡ�����ļ����ı�
        find_txt(filelist->path,filelist->buf);
        break;
        //��ȡĿ¼���ı�
    case FILE_DIR:
        find_dir(filelist->path,filelist->buf);
        break;
        //��ȡָ���ļ����ı�
    case FILE_HZ:
        find_hz(filelist->path,filelist->buf);
        break;
    }

    printf("�ļ�����%d\n",type);

//�ı�����Ϊ�б��ַ�
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

//�������·��
long fileregin(char *path)
{
    filepath=path;
    return 0;
}

//ȡ���ļ�·��
char *filegetpath(long file)
{
    FileList *filelist=(void*)file;
    return filelist->path;
}

//ȡ���ļ���
char *filegetname(long file)
{
    FileList *filelist=(void*)file;
    return filelist->text[filelist->start+filelist->gb];

}

//ȡ���ļ�ȫ��
char *filegetfilename(long file)
{
    FileList *filelist=(void*)file;
    char text[300];
    sprintf(text,"%s%s",filelist->path,filelist->text[filelist->start+filelist->gb]);
    return text;
}


/*
//�����ļ��б�
long filetext(long file,char **text)
{
FileList *filelist=(void*)file;

return 0;
}
*/

//���ļ��б���ص���Ļ��
long fileshow(long file)
{
    FileList *filelist=(void*)file;

    int i=0;
    int icon=0;
    char *hz=NULL;
    char *out;
    out=malloc(300);
    printf("��ʼ����Ŀ¼");
//�б���
    drect(0,30,SCRW,SCRH-60,240,240,240);
//�����
    drect(24,filelist->gb*26+30,SCRW-24,26,95,185,240);
    printf("�����");
    for(i=0; i<filelist->liemax; i++)
    {
        if(i>=filelist->max)break;


        sprintf(out,"%s/%s", filelist->path, filelist->text[filelist->start+i]);
        //printf("�ϳ��ļ���");

        if(filestate(out)==2)
//��Ŀ¼
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
        printf("��ʾ�ı�");


    }

    //������
    drect(1,1,SCRW-2,28,220,220,220);
    linerect(0,0,SCRW-1,29,150,150,150);
    dtext(filelist->title,2,(30-fonth)/2,textcolor.r,textcolor.g,textcolor.b,0,FONT);

    //�˵���
    toolbar("ȷ��","����");
    //������
    gdt (SCRW-3,30,3,SCRH-60,filelist->max,filelist->start,filelist->liemax);

    printf("�������");
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

            printf("��\n");
            if(ui_file->gb>0)
            {
                ui_file->gb--;
            }

            else //����ڶ���ʱ
            {
                if(ui_file->start>0)
                {
                    ui_file->start--;
                    //printf("����ڶ���ʱ\n");
                }

                else //�����б��������صײ�
                {
                    //printf("�����б��������صײ�\n");
                    if(ui_file->max>liemax)
                    {

                        ui_file->start=ui_file->max-liemax;
                        ui_file->gb=liemax-1;
                    }
                    else //�б��������Ը���ȫ��ʱ
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
            else //����ڵײ�ʱ
            {
                if(ui_file->start+ui_file->liemax<ui_file->max)
                {
                    ui_file->start++;

                }
                else //�б���֮�󷵻ض���
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
            //��ѡ�е���Ŀ¼�������
            if(ui_file->type==FILE_ALL)
            {
                if (filestate(filegetfilename((long)ui_file))==2)
                {


                    strcat(filelist->path,filelist->text[filelist->start+filelist->gb]);

                    ui_file->gb=0;
                    ui_file->start=0;
                    //��ȡ�����ļ����ı�
                    find_txt(ui_file->path,ui_file->buf);
                    //�ı�����Ϊ�б��ַ�
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
//���й������
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            ui_file->gb=(p2-30)/26;
            fileref((long)ui_file);
        }
//���й�����
        else if(p1>SCRW-30 && p1<SCRW && p2>30&& p2<SCRH-30)
        {
            ui_fileevent(KY_DOWN,_LEFT+p2/(SCRH/2),0);

        }


    }

    else if(type==MS_UP)
    {
        if((p2-30)/26==ui_file->gb)//���й�� 
        {
        	k_ui=0;
            _event(MR_FILE,FILE_SL,p2);
        }
//�˵�ȷ����
        if(p1>0 && p1<SCRW-30 && p2>30 && p2<SCRH-30)
        {
            k_ui=0;
            _event(MR_FILE,FILE_SL,p2);

        }

//�˵����ؼ�
        if(p1>SCRW* 2/3 && p1<SCRW && p2>SCRH-30 &&p2<SCRH)
        {
            k_ui=0;
            _event(MR_FILE,FILE_SR,p2);

        }


    }


    return 0;


}










long textListLoad(char *text,char **list_rc)//���ı�����Ϊ�ַ���
{
    char *scr;
    int i=0;
    int list_n;
    while(text!=NULL)
    {
//���һ��з�
        scr=strstr(text,"\x0a");
        if(scr==NULL)break;
//ת��Ϊrc�ַ�ָ����
        list_rc[i]=text;
        text=scr+1;
//�����з��滻Ϊ0
        if(scr!=NULL)*scr=0;
        else break;
        if(*(scr-1)=='\x0d')*(scr-1)=0;
        i++;
    }
    list_n=i;
    return list_n;
}




void find_txt(char *file,char *text)//�����ļ����ı�����������·�� �ı�
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


void find_dir(char *file,char *text)//�����ļ����ı�����������·�� �ı�
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
    //����Ƿ�Ϊ�ļ���
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


void find_hz(char *file,char *text)//�����ļ����ı�����������·�� �ı�
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

    //����Ƿ�Ϊ�ļ�
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




















//��������������������x������y����ȣ��߶ȣ��б��������б���ʼλ�ã��б���ʾ���������
void gdt(int x,int y,int w,int h,int len,int start,int liemax)
{
    if(liemax>len)liemax=len;
    drect(x,y,w,h,90,90,90);
    drect(x,y+h*start/len,w,h*liemax/len,100,190,240);
}

#endif











