#include <mrc_base.h>
#include "mpc.h"
#include "game_menu.h"
#include "uc3_font.h"
#include "mrc_graphics.h"

extern void switchWindow(int windex, int level);
void pmenu_init(PMENU *menu)
{
    int i;
    menu->items = malloc(4 * sizeof(PMENU_ITEM));
    menu->logobmp = readBitmap565FromAssets("logo.png");
    menu->items[0].text = "壁纸列表";
    menu->items[1].text = "帮助";
    menu->items[2].text = "关于";
    menu->items[3].text = "退出";
    menu->index = 0;
    for (i = 0; i < 4; i++)
    {
        menu->items[i].x = 50;
        menu->items[i].y = 140 + i * 20;
        menu->items[i].w = SCRW - 100;
        menu->items[i].h = 20;
    }
}

int pmenu_col(PMENU *menu, int32 x, int32 y)
{
    int i = 0;
    PMENU_ITEM item;
    for (i = 0; i < 4; i++)
    {
        item = menu->items[i];
        if (x > item.x && x < item.x + item.w && y > item.y && y < item.y + item.h)
        {
            return i;
        }
    }
    return -1;
}
void pmenu_draw(PMENU *menu)
{
    int32 textw;
    int32 i;
    BITMAPINFO logoinfo;
    mrc_clearScreen(240, 240, 240);
    drawBitmap(menu->logobmp, (SCRW - menu->logobmp->width) / 2, 20);

    mrc_drawRect(menu->items[menu->index].x, menu->items[menu->index].y, menu->items[menu->index].w, menu->items[menu->index].h, 255, 200, 0);
  
    for (i = 0; i < 4; i++)
    {
        textw = uc3_getWidth(menu->items[i].text, 0);
        // mrc_printf("文字宽度：%d", textw);
        if (i == menu->index)
        {
            uc3_drawText(menu->items[i].text, menu->items[i].x + (menu->items[i].w - textw) / 2, menu->items[i].y + 2, 20, 20, 20, 0);
        }
        else
        {
            uc3_drawText(menu->items[i].text, menu->items[i].x + (menu->items[i].w - textw) / 2, menu->items[i].y + 2, 220, 200, 50, 0);
        }
    }
    mrc_refreshScreen(0, 0, SCRW, SCRH);
    bitmapGetInfo(menu->logobmp, &logoinfo);
    
}

void pmenu_event(PMENU *menu, int type, int p1, int p2)
{
    int32 tempindex = 0;
    if (type == KY_UP)
    {
        if (p1 == _UP)
        {
            menu->index--;
            if (menu->index < 0)
                menu->index = 3;
            pmenu_draw(menu);
        }
        if (p1 == _DOWN)
        {
            menu->index++;
            if (menu->index > 3)
                menu->index = 0;
            pmenu_draw(menu);
        }

        if (p1 == _SELECT || p1 == _SLEFT)
        {
            switchWindow(1, menu->index);
        }
        if (p1 == _SRIGHT)
        {
            //退出游戏
            mrc_exit();
        }
    }
    if (type == MS_UP)
    {
        if ((tempindex = pmenu_col(menu, p1, p2)) >= 0)
        {
            if (menu->index == tempindex)
            {
                switchWindow(1, menu->index);
            }
            else
            {
                menu->index = tempindex;
                pmenu_draw(menu);
            }
        }
    }
}

void pmenu_free(PMENU *menu)
{
    free(menu->items);
    bitmapFree(menu->logobmp);
    menu->items = NULL;
    menu->index = 0;
}
