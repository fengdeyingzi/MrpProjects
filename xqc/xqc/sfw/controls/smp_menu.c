#include "i18n.h"
#include "smp.h"
#include "smp_menu.h"

/**
 * \brief menu item style
 */
#define MIS_HASSUBMENU		0x0001
#define MIS_DISABLED		0x0002
#define MIS_CHECKED			0x0004 

#define MENU_ITEM_SPACE			2
#define MENU_BORDER				3  //外围边框
#define MENU_SEPERATOR_HEIGHT		3
#define MENU_ITEM_MARGIN			6
#define MENU_STATUS_WIDTH			6
#define MENU_SUBICON_WIDTH		3

#define HAS_SUBMENU(pItem) \
	((pItem->style & MIS_HASSUBMENU) && pItem->su.submenu)

#define GET_SUBMENU(pItem) \
	pItem->su.submenu

#define GET_USRDATA(pItem) \
	pItem->su.userdata

#define HAS_TITLE(pItem) \
	(NULL != pItem->title)

#define HAS_CHECKED(pItem) \
	(pItem->style & MIS_CHECKED)

#define HAS_DISABLED(pItem) \
	(pItem->style & MIS_DISABLED)

#define IS_SEPERATOR(pItem) \
	(pItem->id == SMP_MENU_SEPERATOR)

// DATA TYPES
///////////////////////////////////////////////////////////////////////////////////////////////

//menu item data
typedef struct SMP_MenuItem
{
	WID id; //menuitem id
	WORD style; 
	PCWSTR title; //display string
	
	union{
	DWORD userdata; //userdata, only MIS_HASSUBMENU not set
	struct SMP_MenuItem* submenu; //submenu items, only MIS_HASSUBMENU set
	}su;
	
	struct SMP_MenuItem* next; //next item
}MENUITEM, *PMENUITEM;

//menu data
typedef struct SMP_MenuData
{
	int height; // max menu item height
	int width; // max menu item width
	PMENUITEM highlight; // current selected index from 0

	//menu item list
	PMENUITEM item_list;

	//sub menu window handle
	HMENU hSubMenu;
}MENUDATA, *PMENUDATA;

//global data
static HMENU sMenu;
static MENUITEM sMenuItems[SMP_MENU_MAX_MENUITEM];

// FUNCTION IMPLEMENTS
/////////////////////////////////////////////////////////////////////////////////////////////

VOID SMP_Menu_ClearMenuItems(VOID)
{
	SGL_MEMSET((sMenuItems), 0, sizeof((sMenuItems)));
}


VOID SMP_Menu_SetMenuItem(int index, WID id, PCWSTR str, DWORD data, int next)
{
	if(index < 0 || index >= SMP_MENU_MAX_MENUITEM
		|| next < -1 || next >= SMP_MENU_MAX_MENUITEM)
		return;
		
	sMenuItems[(index)].id = (WID)(id);
	sMenuItems[(index)].title = (str);
	sMenuItems[(index)].su.userdata = (data);
	sMenuItems[(index)].next = ((next)==-1? NULL : &sMenuItems[(next)]);
}


VOID SMP_Menu_SetMenuItem2(int start, const DWORD* items, int size)
{
	int i;
	
	if(start < 0 || start >= SMP_MENU_MAX_MENUITEM
		|| !items || size <= 0 || size > SMP_MENU_MAX_MENUITEM - start)
		return;

	for(i = 0; i < size; i++, start++)
	{
		sMenuItems[start].id = (WID)items[i];
		sMenuItems[start].title = SGL_LoadString(items[i]);
		sMenuItems[start].su.userdata = 0;
		sMenuItems[start].next = (i == size - 1? NULL : &sMenuItems[start+1]);
	}
}


VOID SMP_Menu_SetSubMenu(int index, int sub)
{
	int i;
	if(index < 0 || index >= SMP_MENU_MAX_MENUITEM
		|| sub < 0 || sub >= SMP_MENU_MAX_MENUITEM)
		return;

	for(i=0;i<SMP_MENU_MAX_MENUITEM;i++)
		if(sMenuItems[i].id==index)
		{
			sMenuItems[i].style |= MIS_HASSUBMENU;
			sMenuItems[i].su.submenu = &sMenuItems[(sub)];
			return;
		}
}


VOID SMP_Menu_CheckMenuItem(int index, BOOL check)
{
	if(index < 0 || index >= SMP_MENU_MAX_MENUITEM)
		return;

	if(check)
		sMenuItems[(index)].style |= MIS_CHECKED;
	else
		sMenuItems[(index)].style &= ~MIS_CHECKED;
}


VOID SMP_Menu_DisableMenuItem(int index, BOOL disable)
{
	if(index < 0 || index >= SMP_MENU_MAX_MENUITEM)
		return;

	if(disable)
		sMenuItems[(index)].style |= MIS_DISABLED;
	else
		sMenuItems[(index)].style &= ~MIS_DISABLED;
}


HMENU SMP_Menu_Popup(WID id, DWORD style, HWND hParent, int x, int y, HWND listener)
{
	if(NULL == sMenu)
	{
		sMenu = SGL_CreateWindow(SMP_Menu_WndProc, 1, 1, 1, 1, id, style, (DWORD)sMenuItems);
	}else{
		_WID(sMenu) = id;
		_STYLE(sMenu) = style;
		_SET_USERDATA(sMenu, sMenuItems);
	}

	_LISTENER(sMenu) = listener;
	SGL_PopupMenu(sMenu, x, y, hParent);
	return sMenu;
}


HMENU SMP_Menu_Popup2(WID id, DWORD style, HWND hParent, int x, int y, const DWORD* items, int size, HWND listener)
{
	SMP_Menu_ClearMenuItems();
	SMP_Menu_SetMenuItem2(0, items, size);
	return SMP_Menu_Popup(id, style, hParent, x, y, listener);
}


HMENU SMP_Menu_PopupFlat(WID id, DWORD style, HWND hParent, int x, int y, int w, HWND listener)
{
	if(NULL == sMenu)
	{
		sMenu = SGL_CreateWindow(SMP_Menu_WndProc, x, y, w, 1, id, style | SMP_MENUS_FLAT, (DWORD)sMenuItems);
	}else{
		_WID(sMenu) = id;
		_STYLE(sMenu) = style | SMP_MENUS_FLAT;
		_WIDTH(sMenu) = w;
		_SET_USERDATA(sMenu, sMenuItems);
	}

	_LISTENER(sMenu) = listener;
	SGL_PopupMenu(sMenu, x, y, hParent);
	return sMenu;
}


static VOID SMP_Menu_AdjustMenuLocation(HMENU hMenu, int width, int height)
{
	int x= _LEFT(hMenu);
	int y= _TOP(hMenu);
	SIZE size;

	SGL_GetScreenSize(&size);
	SGL_WindowToScreen(_PARENT(hMenu), &x, &y);

	if(_IS_SET_ANY(hMenu, SMP_MENUS_FLAT))
		width = _WIDTH(hMenu);

	if(x + width > size.cx)
		x = size.cx - width;

	if(_IS_SET_ANY(hMenu, SMP_MENUS_BOTTOMLEFT))
	{
		y -= height;
		if(y < 0) y = 0;
	}else{
		if(y + height > size.cy - SMP_MENU_BOTTOM_HEIGHT) y = size.cy - height - SMP_MENU_BOTTOM_HEIGHT;
	}

	SGL_ScreenToWindow(_PARENT(hMenu), &x, &y);
	_LEFT(hMenu) = x;
	_TOP(hMenu) = y;
	_WIDTH(hMenu) = width;
	_HEIGHT(hMenu) = height;
}


static VOID SMP_Menu_GetMenuDimension(HMENU hMenu, int* width, int* height)
{
	int32 w, h, itemCount = 0, spCount = 0;
	HFONT font = SGL_GetSystemFont();
	PMENUDATA pMenuData =  _GET_WINDATA(hMenu, PMENUDATA);
	PMENUITEM pItem = _GET_USERDATA(hMenu, PMENUITEM);
	
	for(pMenuData->height = pMenuData->width = 0; pItem; pItem = pItem->next)
	{
		// count seperator menu item count
		if(IS_SEPERATOR(pItem))
		{
			spCount ++;
			continue;
		}

		//count menu items
		++itemCount;

		//test menu item width & height
		if(HAS_TITLE(pItem))
		{
			fontwh((PSTR)pItem->title, TRUE, (Uint16)font, &w, &h);
			if(w > pMenuData->width) pMenuData->width = w;
			if(h > pMenuData->height) pMenuData->height = h;
		}
	}

	//compute the informations
	if(_IS_SET_ANY(hMenu, SMP_MENUS_FLAT))
	{
		pMenuData->width +=  2*SMP_ITEM_CONTENT_MARGIN;
		*width = pMenuData->width;
		pMenuData->height += MENU_ITEM_SPACE;
		*height = itemCount * (pMenuData->height) + spCount * MENU_SEPERATOR_HEIGHT + 2;
	}else{
		pMenuData->width += 4 * MENU_ITEM_MARGIN + 2*MENU_BORDER;
		*width = pMenuData->width;
		pMenuData->height += MENU_ITEM_SPACE;
		*height = itemCount * (pMenuData->height) + spCount * MENU_SEPERATOR_HEIGHT + 2*MENU_BORDER;
	}
}


static PMENUITEM SMP_Menu_GetMenuItemByPosition(PMENUITEM pItem, int height, int x, int y)
{
	for(; pItem; pItem = pItem->next)
	{
		if(IS_SEPERATOR(pItem))
		{
			y -= MENU_SEPERATOR_HEIGHT;
			continue;
		}
		
		y -= height;
		if(y < 0) break;
	}

	return pItem;
}


static VOID SMP_Menu_ShowSubMenu(HMENU hMenu, PMENUDATA pMenuData)
{
	PMENUITEM pItem;
	PMENUDATA pSubMenuData; 
	int y = _TOP(hMenu);
	
	if(pMenuData->hSubMenu)
		_WID(pMenuData->hSubMenu) = _WID(hMenu);
	else	
		pMenuData->hSubMenu = SGL_CreateWindow(SMP_Menu_WndProc, 1, 1, 1, 1, _WID(hMenu), 0, 0);
	_LISTENER(pMenuData->hSubMenu) = _LISTENER(hMenu);

	pSubMenuData = _GET_WINDATA(pMenuData->hSubMenu, PMENUDATA);
	_SET_USERDATA(pMenuData->hSubMenu, GET_SUBMENU(pMenuData->highlight));
	
	for(pItem = _GET_USERDATA(hMenu, PMENUITEM); pItem != pMenuData->highlight; pItem = pItem->next)
		y += IS_SEPERATOR(pItem)? MENU_SEPERATOR_HEIGHT : pMenuData->height;
	
	_LEFT(pMenuData->hSubMenu) = _LEFT(hMenu) + pMenuData->width;
	_TOP(pMenuData->hSubMenu) = y;
	SGL_AddChildWindow(_PARENT(hMenu), pMenuData->hSubMenu);
}


static VOID SMP_Menu_HilightMenuItem(HMENU hMenu, PMENUITEM pItem, BOOL press)
{
	PMENUDATA pMenuData =  _GET_WINDATA(hMenu, PMENUDATA);

	if(pItem && (HAS_DISABLED(pItem) || IS_SEPERATOR(pItem)))
		return;
	
	if(pItem == pMenuData->highlight)
	{
		if(HAS_SUBMENU(pItem) && !SGL_IsWindowVisible(pMenuData->hSubMenu))
			SMP_Menu_ShowSubMenu(hMenu, pMenuData);
		return;
	}

	pMenuData->highlight = pItem;
	if(SGL_IsWindowVisible(pMenuData->hSubMenu))
	{
		HWND hParent = _PARENT(hMenu);
		SGL_SuspendDrawing();
		SGL_RemoveChildWindow(pMenuData->hSubMenu);
		if(press && HAS_SUBMENU(pItem))
			SMP_Menu_ShowSubMenu(hMenu, pMenuData);
		SGL_UnsuspendDrawing();
		SGL_UpdateWindow(hParent);
	}else{
		SGL_UpdateWindow(hMenu);
		if(press && HAS_SUBMENU(pItem))
			SMP_Menu_ShowSubMenu(hMenu, pMenuData);
	}
}


static VOID SMP_Menu_FocusItemNext(HMENU hMenu)
{
	int i;
	PMENUDATA pMenuData =  _GET_WINDATA(hMenu, PMENUDATA);	
	PMENUITEM pItem = pMenuData->highlight;

	/*
	* 两次的循环主要是用来处理循环滚动的效果。在第一次循环
	* 中如果找不到下一个MenuItem。 那么pItem的值最后是NULL，在第
	* 二个循环时发现这个值是NULL将会从新从表头查找。
	*/
	for(i = 0; i < 2; i++)
	{
		for(pItem = pItem? pItem->next : _GET_USERDATA(hMenu, PMENUITEM)
			; pItem && (IS_SEPERATOR(pItem) || HAS_DISABLED(pItem))
			; pItem = pItem->next);

		if(pItem && pItem != pMenuData->highlight) 
		{
			SMP_Menu_HilightMenuItem(hMenu, pItem, FALSE);
			break;
		}
	}	
}


static VOID SMP_Menu_FocusItemPrev(HMENU hMenu)
{
	int i;
	PMENUDATA pMenuData =  _GET_WINDATA(hMenu, PMENUDATA);	
	PMENUITEM pItem = pMenuData->highlight;
	PMENUITEM pPrev, hili;

	pPrev = pItem = _GET_USERDATA(hMenu, PMENUITEM);
	hili = pMenuData->highlight;

	/*
	* 这里的两次的循环和mnFocusItemNext中的处理是相同的道理。
	*/
	for(i = 0; i < 2; i++)
	{
		while(pItem != hili)
		{
			for(pPrev = pItem, pItem = pItem->next
				; pItem && (IS_SEPERATOR(pItem) || HAS_DISABLED(pItem))
				; pItem = pItem->next);
		}
		
		if(pPrev && !IS_SEPERATOR(pPrev) && !HAS_DISABLED(pPrev) && pPrev != pMenuData->highlight)
		{
			SMP_Menu_HilightMenuItem(hMenu, pPrev, FALSE);
			break;
		}else
			hili = NULL;
	}
	
}


static VOID SMP_Menu_DrawNormal(HWND hMenu)
{
	PMENUDATA pMenuData =  _GET_WINDATA(hMenu, PMENUDATA);

	Uint32 color;
	int x=0, y=0, drawx;
	HFONT font = SGL_GetSystemFont();
	PMENUITEM pItem = _GET_USERDATA(hMenu, PMENUITEM);
	
	if(!pItem) return;
	
	SGL_WindowToScreen(hMenu, &x, &y);
	
	//draw background
	GAL_FillBox(PHYSICALGC, x+MENU_BORDER, y+MENU_BORDER, _WIDTH(hMenu)-2*MENU_BORDER, _HEIGHT(hMenu)-2*MENU_BORDER, 0xf0f0f0);	
	GAL_Rectangle(PHYSICALGC, x, y, _WIDTH(hMenu), _HEIGHT(hMenu), 0xf0f0f0);
	GAL_Rectangle(PHYSICALGC, x+1, y+1, _WIDTH(hMenu)-2, _HEIGHT(hMenu)-2, 0x3179C6);
	
	x += MENU_BORDER;
	y += MENU_BORDER;

	//draw the menu items
	for(; pItem; pItem = pItem->next)
	{
		//draw seperator
		if(IS_SEPERATOR(pItem))
		{
			GAL_DrawHLine(PHYSICALGC, x, y + DIV(MENU_SEPERATOR_HEIGHT, 2), _WIDTH(hMenu) - 2*MENU_BORDER, COLOR_controlhili);	
			y += MENU_SEPERATOR_HEIGHT;
			continue;
		}
		
		//draw high light background
		if(pItem == pMenuData->highlight)
			GAL_FillBox(PHYSICALGC, x, y, _WIDTH(hMenu)-2*MENU_BORDER, pMenuData->height, 0x639ED6);
		
		//draw checked style
		if(HAS_CHECKED(pItem))
		{
			GAL_Line(PHYSICALGC, x, y + DIV(pMenuData->height, 2), x + 3, y + DIV(pMenuData->height*2, 3), COLOR_focus);
			GAL_Line(PHYSICALGC, x + 3, y + DIV(pMenuData->height*2, 3), x + 7, y + DIV(pMenuData->height, 3), COLOR_focus);
		}

		color = HAS_DISABLED(pItem)? COLOR_lightgray :  (pItem == pMenuData->highlight? COLOR_lightwhite : COLOR_black);

		//draw title
		if(HAS_TITLE(pItem))
		{
			gbsez((PSTR)pItem->title, (int16)(x + MENU_ITEM_MARGIN), (int16)(y + DIV(MENU_ITEM_SPACE, 2)), 
				PIXEL888RED(color), PIXEL888GREEN(color), PIXEL888BLUE(color), TRUE, (Uint16)font);
		}
		
		//draw submenu indicator
		if(HAS_SUBMENU(pItem))
		{
			int i;
			drawx = x + _WIDTH(hMenu) - MENU_ITEM_MARGIN - MENU_SUBICON_WIDTH - MENU_BORDER;
			for(i=0; i<4; i++)
			{
				GAL_DrawVLine(PHYSICALGC, drawx+i, y + DIV(pMenuData->height, 2) - 3+i, 7-2*i, color);
			}
			//GAL_DrawVLine(PHYSICALGC, drawx, y + DIV(pMenuData->height, 2) - 3, 7, color);
			//GAL_Line(PHYSICALGC, drawx, y + DIV(pMenuData->height, 2) + 3, drawx + 7, y + DIV(pMenuData->height, 2), color);
			//GAL_Line(PHYSICALGC, drawx, y + DIV(pMenuData->height, 2) - 3, drawx + 7, y + DIV(pMenuData->height, 2), color);
		}
		
		y += pMenuData->height;
	}		
}


static VOID SMP_Menu_DrawFlat(HWND hMenu)
{
	PMENUDATA pMenuData =  _GET_WINDATA(hMenu, PMENUDATA);

	int x=0, y=0;
	HFONT font = SGL_GetSystemFont();
	PMENUITEM pItem = _GET_USERDATA(hMenu, PMENUITEM);
	mr_screenRectSt rect;
	mr_colourSt color;
	Uint32 tmp;
	
	if(!pItem) return;
	
	SGL_WindowToScreen(hMenu, &x, &y);
	
	//draw background
	GAL_Rectangle(PHYSICALGC, x, y, _WIDTH(hMenu), _HEIGHT(hMenu), COLOR_black);

	++y; //skip the top margin
	rect.x = (uint16)(x + SMP_ITEM_CONTENT_MARGIN), rect.w = (uint16)(_WIDTH(hMenu)-2*SMP_ITEM_CONTENT_MARGIN), rect.h = (uint16)pMenuData->height;
	
	//draw the menu items
	for(; pItem; pItem = pItem->next)
	{
		//draw seperator
		if(IS_SEPERATOR(pItem))
		{
			GAL_DrawHLine(PHYSICALGC, x + MENU_ITEM_SPACE, y + DIV(MENU_SEPERATOR_HEIGHT, 2), _WIDTH(hMenu) - 2*MENU_ITEM_SPACE, COLOR_controlhili);	
			y += MENU_SEPERATOR_HEIGHT;
			continue;
		}
		
		//draw high light background
		if(pItem == pMenuData->highlight)
			GAL_FillBox(PHYSICALGC, x+1, y, _WIDTH(hMenu)-2, pMenuData->height, COLOR_controlhili);
		
		tmp = HAS_DISABLED(pItem)? COLOR_lightgray :  (pItem == pMenuData->highlight? COLOR_lightwhite : COLOR_black);
		color.r = PIXEL888RED(tmp), color.g = PIXEL888GREEN(tmp), color.b = PIXEL888BLUE(tmp);

		//draw title
		rect.y = (uint16)y;
		gbsez2((PSTR)pItem->title, rect.x, (int16)(rect.y + DIV(MENU_ITEM_SPACE, 2)), rect, color, TRUE, (Uint16)font);
		
		y += pMenuData->height;
	}		
}


LRESULT SMP_Menu_WndProc(HMENU hMenu, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PMENUDATA pMenuData =  _GET_WINDATA(hMenu, PMENUDATA);

	switch(Msg)
	{
		
	case WM_CREATE:
	{
		pMenuData = SGL_MALLOC(sizeof(MENUDATA));
		if(!pMenuData){
			SGL_TRACE("%s, %d: memory out\n", __FILE__, __LINE__);
			return 1;
		}
		
		SGL_MEMSET(pMenuData, 0, sizeof(MENUDATA));
		_SET_WINDATA(hMenu, pMenuData);
		return 0;
	}
		
	case WM_DESTROY:
	{
		if(!pMenuData) return 0;
		
		//if have sub menu, destroy it
		if(pMenuData->hSubMenu)
		{
			_SET_USERDATA(pMenuData->hSubMenu, 0); //should clear it first
			SGL_DestroyWindow(pMenuData->hSubMenu);
		}
		
		SGL_FREE(pMenuData);
		return 0;
	}
	
	case WM_SHOW:
	{
		int w, h;
		pMenuData->highlight = 0; 
		SMP_Menu_GetMenuDimension(hMenu, &w, &h);
		SMP_Menu_AdjustMenuLocation(hMenu, w, h);
		SMP_Menu_FocusItemNext(hMenu);		
		return 0;
	}
	
	case WM_HIDE:
	{
		//if sub menu visible close it
		if(SGL_IsWindowVisible(pMenuData->hSubMenu))
			SGL_RemoveChildWindow(pMenuData->hSubMenu);
		
		_CLR_STYLE(hMenu, WS_PRESSED);
		_CLR_STYLE(hMenu, WS_KEYDOWN);
		return 0;
	}
		
	case WM_PAINT:
	{
		if(_IS_SET_ANY(hMenu, SMP_MENUS_FLAT))
			SMP_Menu_DrawFlat(hMenu);
		else
			SMP_Menu_DrawNormal(hMenu);
		return 0;
	}
			
	case WM_MOUSEDOWN:
	case WM_MOUSEUP:		
	{
		PMENUITEM pItem;
		int top = _IS_SET_ANY(hMenu, SMP_MENUS_FLAT)? 1 : MENU_BORDER;
		int left = _IS_SET_ANY(hMenu, SMP_MENUS_FLAT)? SMP_ITEM_CONTENT_MARGIN : MENU_BORDER;
			
		if((int)wParam < left || (int)wParam > _WIDTH(hMenu)-left
			|| (int)lParam < top || (int)lParam > _HEIGHT(hMenu)-top)
			return 0;

		if(NULL == (pItem = SMP_Menu_GetMenuItemByPosition(_GET_USERDATA(hMenu, PMENUITEM), 
			pMenuData->height, wParam - left, lParam - top)))
			return 0;

		if(WM_MOUSEDOWN == Msg)
		{
			_SET_STYLE(hMenu, WS_PRESSED);
			SMP_Menu_HilightMenuItem(hMenu, pItem, TRUE);
		}else if(_IS_SET_ANY(hMenu, WS_PRESSED) && pItem == pMenuData->highlight && !HAS_SUBMENU(pItem)){
			SGL_SelectMenuItem(hMenu, pItem->id, pItem->su.userdata);
			_CLR_STYLE(hMenu, WS_PRESSED);
		}
		
		return 0;
	}

	case WM_KEYDOWN:
	case WM_KEYDOWNREPEAT:
	{		
		//pass events to subment
		if(SGL_IsWindowVisible(pMenuData->hSubMenu))
			return SGL_SendMessage(pMenuData->hSubMenu, Msg, wParam, lParam);
		
		//_SET_STYLE(hMenu, WS_KEYDOWN);
		if(wParam == MR_KEY_DOWN)
			SMP_Menu_FocusItemNext(hMenu);
		else if(wParam == MR_KEY_UP)
			SMP_Menu_FocusItemPrev(hMenu);
		return 0;
	}

	case WM_KEYUP:
	{
		if(wParam == MR_KEY_SOFTRIGHT)
		{
#if 1 //close the whole menu
			return 1;
#else //close the menu step by step
			if(!SGL_IsWindowVisible(pMenuData->hSubMenu))
				return 1; //JUST CLOSE ITSELF
			SGL_RemoveChildWindow(pMenuData->hSubMenu);
			return 0;
#endif
		}

		//左键一级级关闭菜单（子菜单）
		if(wParam == MR_KEY_LEFT) 
		{
			if(!SGL_IsWindowVisible(pMenuData->hSubMenu))
				return 1; //JUST CLOSE ITSELF
			SGL_RemoveChildWindow(pMenuData->hSubMenu);
				return 0;
		}

		//pass events to subment
		if(SGL_IsWindowVisible(pMenuData->hSubMenu))
			return SGL_SendMessage(pMenuData->hSubMenu, Msg, wParam, lParam);

		/*
		* for the simple toolbar will generate a single WM_KEYUP from click event
		* so these two code will simple menu can not cowork with simple toolbar
		*/
		//if(!_IS_SET_ANY(hMenu, WS_KEYDOWN)) return 0;
		//_CLR_STYLE(hMenu, WS_KEYDOWN);

		if(wParam == MR_KEY_SELECT || wParam == MR_KEY_SOFTLEFT || wParam == MR_KEY_RIGHT || wParam == MR_KEY_5)
		{	
			//user select a menu item
			if(pMenuData->highlight)
			{
				if(!HAS_SUBMENU(pMenuData->highlight))
					SGL_SelectMenuItem(hMenu, pMenuData->highlight->id, pMenuData->highlight->su.userdata);					
				else if(!SGL_IsWindowVisible(pMenuData->hSubMenu))
					SMP_Menu_ShowSubMenu(hMenu, pMenuData);
			}
		}

		return 0;
	}

	case WM_GETSUBMENU:
	{
		return (LRESULT)pMenuData->hSubMenu;
	}
	
	}
	
	return 0;
}

