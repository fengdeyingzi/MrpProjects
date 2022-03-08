#include "smp.h"
#include "smp_list.h"
#include "smp_scrollbar.h"
#include "bmp.h"

#define LIST_SCRBAR_ID		1


// DATA TYPES
///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct SMP_ListData
{
	int rheight;          // �и� ����
	int total;            // �б�����Ŀ��
	int pagestart;        // ��ǰҳ��һ���е�����
	int hilight;          // ������ʾ�е�����
	int pagesize;         // ÿһҳ��ʾ������

	SMP_List_GetTotal fnGetTotalCount;  //��ȡ�������Ļص�����
	SMP_List_GetRowData fnGetRowData;   //��ȡ�����ݵĻص�����
	SMP_List_DrawRow fnDrawRow;
	HWND hScrBar;         // �б���������
}LISTDATA, *PLISTDATA;


/**
 * FUNCTION IMPLEMENTS Ĭ�ϵĻ��й��ܡ�
 * ע�⣺�˺���������������ÿ������
 * r�����л������ݾ���
 * ÿ�θı��б�ĸ����ж��ᴥ�����¼�
 */
VOID SMP_List_DefaultDrawRowCallback(int index, PRECT r, PSMPROWDATA pRowData, int hilight, BOOL flush)
{
	int32 i, w, h;
	mr_screenRectSt rect;
	mr_colourSt color={0,};
	HFONT font = SGL_GetSystemFont();	

	if(System_Theme == THEME_CLASSIC)
	{
		color.r = PIXEL888RED(COLOR_black);
		color.g = PIXEL888GREEN(COLOR_black);
		color.b = PIXEL888BLUE(COLOR_black);
	}else{
		color.r = PIXEL888RED(COLOR_user3);
		color.g = PIXEL888GREEN(COLOR_user3);
		color.b = PIXEL888BLUE(COLOR_user3);
	}

	//���Ƹ�����ʽ
	if(index == hilight) 
	{
		if(System_Theme == THEME_CLASSIC)
		{
			//GAL_Rectangle(PHYSICALGC, r->left, r->top, r->width, r->height, COLOR_focus);				
			//GAL_FillBox(PHYSICALGC, r->left+2, r->top+2, r->width-4, r->height-4, COLOR_magenta);
			GAL_FillBox2(PHYSICALGC, r->left+3, r->top+3, r->width-6, r->height-6, 0x002888D0);
			color.r = PIXEL888RED(COLOR_lightwhite);
			color.g = PIXEL888GREEN(COLOR_lightwhite);
			color.b = PIXEL888BLUE(COLOR_lightwhite);
		}else{
			//GAL_Rectangle(PHYSICALGC, r->left, r->top, r->width, r->height, COLOR_focus);
			//GAL_FillBox(PHYSICALGC, r->left+2, r->top+2, r->width-4, r->height-4, 0x004a4d52);
			GAL_FillBox2(PHYSICALGC, r->left+3, r->top+3, r->width-6, r->height-6, 0x002888D0);
		}
	}

	//����ͼ��
	rect.x = r->left + pRowData->margin, rect.y = r->top, rect.h = r->height;
	if(pRowData->hBmp)
		mrc_bitmapShowEx(pRowData->hBmp, (int16)rect.x, (int16)(rect.y + DIV(rect.h-pRowData->colHeight0, 2)), pRowData->colWidth0, pRowData->colWidth0, pRowData->colHeight0, BM_TRANSPARENT, 0, 0);	

	rect.x += pRowData->colWidth0;
	for(i = 0; i < SMP_LIST_MAX_STR_COL; i++)
	{
		if(pRowData->cols[i].str)
		{
			rect.w = pRowData->cols[i].width;
			fontwh((PSTR)pRowData->cols[i].str, 0, (uint16)font, &w, &h);
			gbsez2((PSTR)pRowData->cols[i].str, rect.x + 5, (int16)(rect.y + DIV(rect.h - h, 2)), rect, color, 0, (Uint16)font);	
		}
		rect.x += pRowData->cols[i].width;
	}	

	if(flush)
		GAL_FlushRegion(PHYSICALGC, r->left, r->top, r->width, r->height);
}

//�б����ÿһ��
static VOID SMP_List_DrawItem(HWND hList, int index)
{
	RECT r;
	int top = 0;
	SMPROWDATA rowData;
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);
	
	_WINRECT(hList, r);	
	SGL_WindowToScreen(_PARENT(hList), &r.left, &r.top);

	top = r.top + SMP_LIST_TOP_MARGIN;
	r.left += SMP_LIST_LEFT_MARGIN;
	r.width -= 2*SMP_LIST_LEFT_MARGIN;
	if(pListData->hScrBar) r.width -= SMP_SCRBAR_WIDTH;
	r.height = pListData->rheight;
	r.top = top + (index - pListData->pagestart)*pListData->rheight;
	
	GAL_FillBox(PHYSICALGC, r.left, r.top, r.width, r.height, _BGCOLOR(hList));
	SGL_MEMSET(&rowData, 0, sizeof(rowData));
	pListData->fnGetRowData(index, &rowData);
	pListData->fnDrawRow(index, &r, &rowData, pListData->hilight, TRUE);
}

//�б�����иı�
static VOID SMP_List_HilightChanged(HWND hList, int from, int to)
{
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);
	pListData->hilight = to;
	SMP_List_DrawItem(hList, from);
	SMP_List_DrawItem(hList, to);
	SGL_NotifyParent(hList, SMP_LISTN_HILICHANGED, to);
}

//���»���ÿһ��
VOID SMP_List_SetDrawRowCallback(HWND hList, SMP_List_DrawRow fnDrawRow)
{
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);
	pListData->fnDrawRow = fnDrawRow;
}

//����ÿһ������
VOID SMP_List_SetDataProvider(HWND hList, SMP_List_GetTotal fnGetTotal, SMP_List_GetRowData fnGetRowData)
{
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);

	if(pListData)
	{
		pListData->fnGetTotalCount = fnGetTotal;
		pListData->fnGetRowData = fnGetRowData;
	}
}

//�б������
static VOID SMP_List_HilightRow(HWND hList, PLISTDATA pListData, int index)
{
	if(pListData->hilight != index 
		&& index >= 0 
		&& index < pListData->total)
	{
		pListData->hilight = index;

		if(index >= pListData->pagestart + pListData->pagesize)
			pListData->pagestart = index - pListData->pagesize + 1;
		else if(index < pListData->pagestart)
			pListData->pagestart = index;
		
		if(pListData->hScrBar) SMP_Scrollbar_SetValue(pListData->hScrBar, pListData->pagestart, FALSE);
		SGL_UpdateWindow(hList);
		SGL_NotifyParent(hList, SMP_LISTN_HILICHANGED, index);
	}
}

//ͨ������ı������
VOID SMP_List_HilightByIndex(HWND hList, int index)
{
	PLISTDATA pListData;
	if(!IS_NORMAL_WINDOW(hList)) return;

	pListData = _GET_WINDATA(hList, PLISTDATA);
	SMP_List_HilightRow(hList, pListData, index);
}

//��ȡ��ǰ�����е�����
int SMP_List_GetHilightIndex(HWND hList)
{
	PLISTDATA pListData;
	if(!IS_NORMAL_WINDOW(hList)) 
		return 0;

	pListData = _GET_WINDATA(hList, PLISTDATA);
	return pListData->hilight;
}

//��ȡ�б�ҳ����Ϣ
VOID SMP_List_GetPageInfo(HWND hList, int* pagestart, int* pagesize)
{
	PLISTDATA pListData;
	if(!IS_NORMAL_WINDOW(hList)) 
		return;
	
	pListData = _GET_WINDATA(hList, PLISTDATA);
	if(pagestart) *pagestart = pListData->pagestart;
	if(pagesize) *pagesize = pListData->pagesize;
}

//�б��Ϸ�ҳ
VOID SMP_List_PageUp(HWND hList)
{
	int oldHili;
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);		
	if(pListData->pagestart <= 0)
		return;

	pListData->pagestart -= pListData->pagesize;
	if(pListData->pagestart < 0)
		pListData->pagestart = 0;

	oldHili = pListData->hilight;
	if(pListData->hilight >= pListData->pagestart + pListData->pagesize)
		pListData->hilight = pListData->pagestart + pListData->pagesize - 1;

	if(pListData->hScrBar)  SMP_Scrollbar_SetValue(pListData->hScrBar, pListData->pagestart, FALSE);
	SGL_UpdateWindow(hList);
	if(oldHili != pListData->hilight)
		SGL_NotifyParent(hList, SMP_LISTN_HILICHANGED, pListData->hilight);
}

//�б��·�ҳ
VOID SMP_List_PageDown(HWND hList)
{
	int oldHili;	
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);		
	int maxstart = pListData->total - pListData->pagesize;
	if(pListData->pagestart >= maxstart)
		return;
	
	pListData->pagestart += pListData->pagesize;
	if(pListData->pagestart > maxstart)
		pListData->pagestart = maxstart;
	
	oldHili = pListData->hilight;
	if(pListData->hilight < pListData->pagestart)
		pListData->hilight = pListData->pagestart;
	
	if(pListData->hScrBar) SMP_Scrollbar_SetValue(pListData->hScrBar, pListData->pagestart, FALSE);
	SGL_UpdateWindow(hList);
	if(oldHili != pListData->hilight)
		SGL_NotifyParent(hList, SMP_LISTN_HILICHANGED, pListData->hilight);
}

//�б��Ϸ���
VOID SMP_List_LineUp(HWND hList)
{
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);	

	if(pListData->hilight > 0) 
	{
		if(pListData->hilight > pListData->pagestart)
			SMP_List_HilightChanged(hList, pListData->hilight, pListData->hilight-1);
		else
			SMP_List_HilightRow(hList, pListData, pListData->hilight - 1);
	}else
		SMP_List_HilightRow(hList, pListData, pListData->total - 1);
}

//�б��·���
VOID SMP_List_LineDown(HWND hList)
{
	int step = 1;
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);
	
	if(pListData->hilight < pListData->total - 1) 
	{
		if(pListData->hilight + step < pListData->pagestart + pListData->pagesize)
			SMP_List_HilightChanged(hList, pListData->hilight, pListData->hilight + step);
		else
			SMP_List_HilightRow(hList, pListData, pListData->hilight + step);
	}else
		SMP_List_HilightRow(hList, pListData, 0);		
}

//ˢ���б�
VOID SMP_List_Refresh(HWND hList)
{
	int maxpagestart;
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);
	pListData->total = pListData->fnGetTotalCount();

	if(pListData->hScrBar)
	{
		_LEFT(pListData->hScrBar) = _WIDTH(hList) - SMP_SCRBAR_WIDTH;
		_TOP(pListData->hScrBar) = 0;
		_WIDTH(pListData->hScrBar) = SMP_SCRBAR_WIDTH;
		_HEIGHT(pListData->hScrBar) = _HEIGHT(hList);
	}
	pListData->pagesize = DIV(_HEIGHT(hList) - SMP_LIST_TOP_MARGIN, SMP_LIST_ITEM_HEIGHT);
	pListData->rheight = SMP_LIST_ITEM_HEIGHT;

	if(pListData->hilight > pListData->total-1) 
		pListData->hilight = MAX(pListData->total - 1, 0);
	
	if(pListData->hScrBar) SMP_Scrollbar_SetInfo(pListData->hScrBar, 0, MAX(pListData->total - 1, 0), pListData->pagesize);
	
	maxpagestart = MAX(pListData->total - pListData->pagesize, 0);
	if(pListData->pagestart > maxpagestart)
	{
		pListData->pagestart = maxpagestart;
		if(pListData->hScrBar) SMP_Scrollbar_SetValue(pListData->hScrBar, pListData->pagestart, FALSE);		
	}
}

//�������б�
VOID SMP_List_Reset(HWND hList)
{
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);
	if(pListData)
	{
		 pListData->rheight = 0; 
		 pListData->total = 0; 
		 pListData->pagestart = 0; 
		 pListData->hilight = 0; 
		 pListData->pagesize = 0; 
	}
}

//�б����¼�
LRESULT SMP_List_WndProc(HWND hList, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PLISTDATA pListData = _GET_WINDATA(hList, PLISTDATA);
	switch(Msg)
	{

	case WM_CREATE:
	{
		//request memory and initialize members
		//�����ڴ沢��ʼ����Ա����
		pListData = (PLISTDATA)SGL_MALLOC(sizeof(LISTDATA));
		if(!pListData){
			SGL_TRACE("%s, %d: memory out\n", __FILE__, __LINE__);
			return 1;
		}
		
		SGL_MEMSET(pListData, 0, sizeof(LISTDATA));	
		pListData->fnDrawRow = SMP_List_DefaultDrawRowCallback;
		_SET_WINDATA(hList, pListData);		
		//create scrollbar
		//����������
		if(!_IS_SET_ANY(hList, SMP_LISTS_NOSCRBAR))
		{
			pListData->hScrBar = SGL_CreateWindow(SMP_Scrollbar_WndProc, 
				0, 0, SMP_SCRBAR_WIDTH, _HEIGHT(hList), 
				LIST_SCRBAR_ID, 0, 0);

			_BGCOLOR(pListData->hScrBar) = COLOR_lightwhite;
			_FGCOLOR(pListData->hScrBar) = 0x00c5dae2;

			SGL_AddChildWindow(hList, pListData->hScrBar);
		}
		return 0;
	}

	case WM_DESTROY:
	{
		if(pListData) SGL_FREE(pListData);
		return 0;
	}

	case WM_SHOW: //ˢ���б�
	{
		_BGCOLOR(pListData->hScrBar) = COLOR_lightwhite;
		_FGCOLOR(pListData->hScrBar) = COLOR_magenta;
		SMP_List_Refresh(hList);
		return 0;
	}

	case WM_PAINT:
	{
		RECT r;
		int32 from, to;
		SMPROWDATA rowData;
		
		_WINRECT(hList, r);
		SGL_WindowToScreen(_PARENT(hList), &r.left, &r.top);

		r.top += SMP_LIST_TOP_MARGIN;
		r.left += SMP_LIST_LEFT_MARGIN;
		r.width -= pListData->hScrBar? SMP_SCRBAR_WIDTH + 2*SMP_LIST_LEFT_MARGIN : 2*SMP_LIST_LEFT_MARGIN;
		r.height = pListData->rheight;
		
		to = pListData->total - pListData->pagestart;
		if(pListData->pagesize < to) to = pListData->pagesize;

		for(from = 0; from < to; from++, r.top += pListData->rheight)
		{
			SGL_MEMSET(&rowData, 0, sizeof(SMPROWDATA));
			pListData->fnGetRowData(from + pListData->pagestart, &rowData);
			pListData->fnDrawRow(pListData->pagestart + from, &r, &rowData, pListData->hilight, FALSE);
		}
		return 0;
	}

	case WM_MOUSEDOWN: //������ʾ�������
	{
		int row = DIV(lParam-SMP_LIST_TOP_MARGIN, pListData->rheight) + pListData->pagestart;
		if(row >= 0 && row < pListData->total && row < pListData->pagestart + pListData->pagesize)
		{			
			if(row != pListData->hilight )
				SMP_List_HilightChanged(hList, pListData->hilight, row);
		}
		return 0;
	}

	case WM_MOUSEUP:  //֪ͨ�������б���
	{
		int row = DIV(lParam-SMP_LIST_TOP_MARGIN, pListData->rheight) + pListData->pagestart;
		if(row >= 0 && row  <pListData->total && row < pListData->pagestart + pListData->pagesize)
		{
			if(row == pListData->hilight)
				SGL_NotifyParent(hList, SMP_LISTN_CLICKED, pListData->hilight);
		}
		return 0;
	}

	case WM_KEYDOWN:  //�����б��з�ҳ
	case WM_KEYDOWNREPEAT:
	{
		switch(wParam)
		{
		case MR_KEY_DOWN:
			SMP_List_LineDown(hList);
			return 1;
		case MR_KEY_UP:
			SMP_List_LineUp(hList);
			return 1;
		case MR_KEY_LEFT:
			SMP_List_PageUp(hList);
			return 1;
		case MR_KEY_RIGHT:
			SMP_List_PageDown(hList);
			return 1;
		}
		return 0;
	}
	
	case WM_KEYUP:
	{
		if( (wParam == MR_KEY_SELECT) || (wParam == MR_KEY_5) )
		{
			SGL_NotifyParent(hList, SMP_LISTN_SELECTED, pListData->hilight);
			return 1;
		}
		return 0;
	}

	case WM_COMMAND:  //��Ҫ�Ǵ��������λ�õı仯
	{
		WORD id = LOWORD(wParam);
		WORD code = HIWORD(wParam);
		
		if(LIST_SCRBAR_ID == id && code == SMP_SCRBARN_VALUECHANGED)
		{
			RECT r = {0,};
			int oldHi = pListData->hilight;

			if(pListData->pagestart == (int)lParam)
				return 0;

			pListData->pagestart = (int)lParam;
			if(pListData->hilight < pListData->pagestart)
				pListData->hilight = pListData->pagestart;
			else if(pListData->hilight >= pListData->pagestart + pListData->pagesize)
				pListData->hilight = pListData->pagestart + pListData->pagesize - 1;

			//just avoid to repaint the scrollbar
			//ֻΪ�˷�ֹ���»��ƹ�����
			r.width = _WIDTH(hList) - SMP_SCRBAR_WIDTH;
			r.height = _HEIGHT(hList);
			SGL_UpdateWindowEx(hList, &r);
			if(oldHi != pListData->hilight)
				SGL_NotifyParent(hList, SMP_LISTN_HILICHANGED, pListData->hilight);
		}
		return 0;
	}

	}

	return 0;
}


