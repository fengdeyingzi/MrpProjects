#ifndef _EDIT_H
#define _EDIT_H

/*��������(6369)
2012-7-10*/

#include <base.h>
#include "c/�ؼ�/type.h"
#define TITMAX 20
#define SPEED 800


#define FOCUSBK 0x00ff0000
#define FOCUSBKW 2

typedef struct
{
int x; int y; int w; int h;
char *title;
char*str;
int32 titleclr;
int32 bkclr;
int32 bgclr;

int max;
int type;
int timeEdit;
int speed;
}Edit;
typedef (Edit*) PEdit;

PEdit EditFocus;
int x,_z;
int focusdia;
/*����һ���༭��
������
@ x��y��w��h���༭��ߴ�
@ title �༭�����( <=20)
@ str �༭������
@ titleclr ������ɫ
@ bkclr �߿���ɫ
@ bgclr ������ɫ
@ max �����������
@ type�༭�����ͣ�
     0: ȫ��
     1���ַ�
     2������
����ֵ��
 0 ����ʧ��
 >0 �༭��ָ��
*/
void* editCreate(int x, int y, int w, int h, char* title,char* str, int32 titleclr, int32 bkclr, int32 bgclr, int max, int type)
{
PEdit a;
a=malloc(sizeof(Edit)+2);
if(a==NULL) return 0;
a->title=malloc(TITMAX);
memset(a->title,0,TITMAX);
memcpy(a->title,title,TITMAX);
a->str=malloc(max);
if(a->str==NULL) return 0;
memcpy(a->str,str,max);
a->max=max;
a->type = type;
a->x = x;
a->y = y;
a->w =w;
a->h = h;
a->titleclr = titleclr;
a->bkclr = bkclr;
a->bgclr = bgclr;
a->timeEdit=timercreate();
a->speed=SPEED;
return a;

}
/*ɾ���༭��
������
 a���༭��ָ��
����ֵ����
*/
void editDel(PEdit a)
{
free(a->title);
timerdel(a->timeEdit);
free(a);
a=0;
}

/*���ý���༭��
@ a ���༭��ָ�룬
*��ΪNULLʱ���൱�����ر༭��
*ע�⣺�뱣ָ֤��Ϸ��ԣ������������������
����ֵ����Ч
 */

int setEditFocus(PEdit a)
{
int r,g,b;
editDraw(a);
r=MAKER(FOCUSBK);
g=MAKEG(FOCUSBK);
b=MAKEB(FOCUSBK);
EditFocus=a;
drect(a->x+1, a->y, a->w-1, FOCUSBKW,  r, g, b);
drect(a->x+1, a->y+a->h-FOCUSBKW+1,a->w-1, FOCUSBKW,r,g,b);
drect(a->x,a->y+1,FOCUSBKW,a->h-1,r,g,b);
drect(a->x+a->w-FOCUSBKW+1,a->y+1,FOCUSBKW,a->h-1,r,g,b);
/*
 timerstop(EditFocus -> timeEdit);
 EditFocus=a;
x=a->x+2;
if(a==NULL)return 0;
 timerstart(a->timeEdit, a->speed, 0, "_time",1);
*/
 return 0;
}


//�õ��༭������
char* getEditStr(PEdit a)
{
return a->str;
}

/*���Ʊ༭��
*/
void editDraw(PEdit a)
{
 int r, g, b;
 int zw,zh;
 rectst rect;
 colorst clr;
 r=MAKER(a->bkclr);g=MAKEG(a->bkclr);b=MAKEB(a->bkclr);
 dline(a->x+1, a->y, a->x+a->w-1, a->y, r, g, b);
 dline(a->x+1, a->y+a->h, a->x+a->w-1, a->y+a->h, r, g, b);
 dline(a->x, a->y+1, a->x, a->y+a->h-1, r, g, b);
 dline(a->x+a->w, a->y+1, a->x+a->w,a->y+a->h-1,r, g, b);
 
 drect(a->x+1, a->y+1, a->w-1, a->h-1, MAKER(a->bgclr), MAKEG(a->bgclr), MAKEB(a->bgclr));

 rect.x=a->x+FOCUSBKW;rect.y=a->y+FOCUSBKW;
 rect.w=a->w-FOCUSBKW*2;rect.h=a->h-FOCUSBKW*2;
 clr.r=MAKER(a->titleclr);
 clr.g=MAKEG(a->titleclr);
 clr.b=MAKEB(a->titleclr);
dtextex(a->str, a->x+FOCUSBKW,a->y+FOCUSBKW, &rect, &clr, 02,FONT);
}

//����༭����ص�(������ʱ����
void _editTime(int a)
{
_z=!_z;
if(_z)
dline(x,EditFocus->y+2,x,EditFocus->y+EditFocus->h-2,MAKER(EditFocus->bgclr),MAKEG(EditFocus->bgclr),MAKEB(EditFocus->bgclr));
else 
dline(x,EditFocus->y+2,x,EditFocus->y+EditFocus->h-2,MAKER(EditFocus->titleclr),MAKEG(EditFocus->titleclr),MAKEB(EditFocus->titleclr));
ref(x,EditFocus->y,1,EditFocus->h);
}


//����༭���¼�����
int editEvent(int msg,int p1,int p2)
{
 if(KY_UP==msg)
 {
  switch(p1)
  {
   case _SELECT:
   focusdia=editcreate(gb_uni(EditFocus->title,0),gb_uni(EditFocus->str,0),EditFocus->type,EditFocus->max);
    break;
   
  }  
 }
//�Ա༭�¼�����
  if(msg == MR_DIALOG)
 {
  switch(p1)
  {
   case DLG_OK:
   char* p;
  p=  editget(focusdia);
memset(EditFocus->str,0,EditFocus->max);
memcpy(EditFocus->str,gb_uni(p,1),EditFocus->max);
    editdel(focusdia);
    break;
   case DLG_CANCEL:
    editdel(focusdia);
  }
 return 1;//��ע�⣬����ֵΪ1�����ж��Ƿ�Ϊ1���ػ����
 }
return 0;
}

#endif
