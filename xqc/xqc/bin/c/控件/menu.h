#ifndef _MENU_H
#define _MENU_H

/*��������(6369)
2012-7-11*/

#include <base.h>
#include "c/�ؼ�/type.h"


#define MAX_NUM 20//���˵���
#define MAX_ITEMSIZE 10//�˵����������
#define MENUBG 0x00ffffff//�˵�����
#define MENUITEM 0x00000000//�˵���
#define MENUFOCUS 0x0000ff00//�˵�����


typedef struct
{
  int x;int y;int w;int h;
  int num;
  int focus;
  char* item[MAX_ITEMSIZE];
}Menu;
typedef (Menu*) PMenu;

/*����һ���˵�
 rect �˵�λ�ã���С
 �˵��߶ȼ��㷽ʽΪ��
   (����߶�+5)*�˵�����+4
  ������Ϊ��������+20
 num �˵�����Ŀ

 ����ֵ��
  0 �����˵�ʧ��
  ��0 ָ��˵��ڴ��ָ��
*/
void* menuCreate(int x,int y,int w,int h, int num)
{
  PMenu a;
  a = malloc(sizeof(Menu));

  if(a == NULL) return 0;

  a -> x = x;
a->y=y;
a->w=w;
a->h=h;
a->focus=0;
  a -> num = num;
  for(int i = 0; i<num; i++)
  {
    a -> item[i] = malloc(MAX_ITEMSIZE);
    if(a->item[i]==NULL)
{free(a);return 0;}
    memset(a->item[i], 0, MAX_ITEMSIZE);
  }
return a;
}


/*ɾ��һ���˵�
 a �˵���ָ��
 
 ����ֵ����
*/
void menuDel(PMenu a)
{
  for(int i = 0; i<a->num; i++)
    free(a->item[i]);
  free(a);
  a= 0;
}


/*����һ���˵���
  a �˵����
  index �˵������
  title �˵�������
  
  ����ֵ��-1  ʧ��
             0 �ɹ�
*/
int setMenuItem(PMenu a,int index, char* title)
{
  if(index >= a->num) return -1;
  memcpy(a->item[index],title, MAX_ITEMSIZE);
  return 0;
}
 

/*���ò˵�����
  a �˵����
  index �˵������
  
  ����ֵ��
    -1  ʧ��
    0 �ɹ�
*/

int setMenuFocus(PMenu a, int index)
{
  if(a->num <= index) return -1;
  a->focus = index;
  return 0;
}

/*��ò˵�����
  a  �˵����
����ֵ ����
*/
int getMenuFocus(PMenu a)
{
  return a->focus;
}


/*�˵������ƶ�
  a  �˵����
  is_next  �Ƿ������ƶ�
    1 ����
    0 ����
  
  ����ֵ ����
*/
void menuFocusNext(PMenu a, int is_next)
{
  if(is_next)
  {
    a -> focus++;
    if(a -> focus >= a->num) 
      a -> focus = 0;

  }
  else
  {
    a -> focus--;
    if(a->focus<0)
      a->focus = a->num - 1;
      
  }
  menuShow(a);
}



/*��ʾ�˵�
  a  �˵����
  ����ֵ����
*/
void menuShow(PMenu a)
{
  int x, y, w, h, zw, zh;
  x = a->x; y = a->y; w = a -> w; h= a->h;

  drect(x, y, w, h, MAKER(MENUBG)/2, MAKEG(MENUBG)/2, MAKEB(MENUBG)/2);
  drect(x+1, y+1, w-2, h-2, MAKER(MENUBG), MAKEG(MENUBG), MAKEB(MENUBG));
  textwh("��", 0, FONT, &zw, &zh);
  zh+=5;
  shaderect(x+2, y+2+(a->focus*zh), w-4, zh, MENUFOCUS, MENUFOCUS-MENUFOCUS/5,0);
  for(int i = 0; i<(a->num); i++)
  {
    if(i == a->focus)
      dtext(a->item[i], x+10, y+2+(i*zh)+2,MAKER(MENUBG), MAKEG(MENUBG), MAKEB(MENUBG),0 , FONT);
    else
       dtext(a->item[i], x+10, y+2+(i*zh)+2,MAKER(MENUITEM), MAKEG(MENUITEM), MAKEB(MENUITEM),0 , FONT);
  }

}




#endif