#ifndef _MENU_H
#define _MENU_H

/*龙魂噬心(6369)
2012-7-11*/

#include <base.h>
#include "c/控件/type.h"


#define MAX_NUM 20//最大菜单项
#define MAX_ITEMSIZE 10//菜单项最多字数
#define MENUBG 0x00ffffff//菜单背景
#define MENUITEM 0x00000000//菜单项
#define MENUFOCUS 0x0000ff00//菜单焦点


typedef struct
{
  int x;int y;int w;int h;
  int num;
  int focus;
  char* item[MAX_ITEMSIZE];
}Menu;
typedef (Menu*) PMenu;

/*创建一个菜单
 rect 菜单位置，大小
 菜单高度计算方式为：
   (字体高度+5)*菜单项数+4
  宽度最低为：字体宽度+20
 num 菜单项数目

 返回值：
  0 创建菜单失败
  非0 指向菜单内存的指针
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


/*删除一个菜单
 a 菜单的指针
 
 返回值：无
*/
void menuDel(PMenu a)
{
  for(int i = 0; i<a->num; i++)
    free(a->item[i]);
  free(a);
  a= 0;
}


/*设置一个菜单项
  a 菜单句柄
  index 菜单项序号
  title 菜单项内容
  
  返回值：-1  失败
             0 成功
*/
int setMenuItem(PMenu a,int index, char* title)
{
  if(index >= a->num) return -1;
  memcpy(a->item[index],title, MAX_ITEMSIZE);
  return 0;
}
 

/*设置菜单焦点
  a 菜单句柄
  index 菜单项序号
  
  返回值：
    -1  失败
    0 成功
*/

int setMenuFocus(PMenu a, int index)
{
  if(a->num <= index) return -1;
  a->focus = index;
  return 0;
}

/*获得菜单焦点
  a  菜单句柄
返回值 ：无
*/
int getMenuFocus(PMenu a)
{
  return a->focus;
}


/*菜单焦点移动
  a  菜单句柄
  is_next  是否向下移动
    1 向下
    0 向上
  
  返回值 ：无
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



/*显示菜单
  a  菜单句柄
  返回值：无
*/
void menuShow(PMenu a)
{
  int x, y, w, h, zw, zh;
  x = a->x; y = a->y; w = a -> w; h= a->h;

  drect(x, y, w, h, MAKER(MENUBG)/2, MAKEG(MENUBG)/2, MAKEB(MENUBG)/2);
  drect(x+1, y+1, w-2, h-2, MAKER(MENUBG), MAKEG(MENUBG), MAKEB(MENUBG));
  textwh("鼎", 0, FONT, &zw, &zh);
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