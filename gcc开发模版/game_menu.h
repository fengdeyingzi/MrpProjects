#ifndef __GAME_MENU_H__
#define __GAME_MENU_H__

#include <mrc_base.h>
#include "mrc_graphics.h"

typedef struct {
    char* text;
    int x, y, w, h;
} PMENU_ITEM;

typedef struct {
    PMENU_ITEM* items;
    int index;
    BITMAP_565* logobmp;
} PMENU;

void pmenu_init(PMENU *menu);
void pmenu_draw(PMENU *menu);
void pmenu_event(PMENU *menu, int type, int p1, int p2);
void pmenu_free(PMENU *menu);

#endif