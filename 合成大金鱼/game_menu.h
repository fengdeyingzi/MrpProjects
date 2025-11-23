#ifndef _PMENU_H_
#define _PMENU_H_
#include "mrc_graphics.h"

typedef struct
{
    char *text; //菜单文字
    int x;
    int y;
    int w;
    int h;
} PMENU_ITEM;

typedef struct
{
    PMENU_ITEM *items;
    BITMAP_565 *logobmp;
    int index; //当前选择项
} PMENU;
void pmenu_init(PMENU *menu);
void pmenu_draw(PMENU *menu);
void pmenu_event(PMENU *menu, int type, int p1, int p2);
void pmenu_free(PMENU *menu);

#endif
