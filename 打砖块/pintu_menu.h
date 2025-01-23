#ifndef _PMENU_H_
#define _PMENU_H_

typedef struct
{
    char *text; //�˵�����
    int x;
    int y;
    int w;
    int h;
} PMENU_ITEM;

typedef struct
{
    PMENU_ITEM *items;
    int32 logobmp;
    int index; //��ǰѡ����
} PMENU;
void pmenu_init(PMENU *menu);
void pmenu_draw(PMENU *menu);
void pmenu_event(PMENU *menu, int type, int p1, int p2);
void pmenu_free(PMENU *menu);

#endif
