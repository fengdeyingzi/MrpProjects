#include "smp.h"
#include "smp_edit.h"
#include "string.h"

#define PWDCHAR			0x2a //'*'
#define SMP_MAXCHARS_PERLINE		80 //max characters displated one line

////////////////////////////////////////////////////////////////////////////////////

typedef struct SMP_EditData
{
	PCWSTR title; //the title of the native input method 
	PWSTR buffer; //the buffer of the content 
	Sint32 size; // the buffer size 
	Sint32 iLineBegin; //the left index of the display text
	Sint32 iLineEnd; // the right index of the display text
	Sint32 iCursor; // the cursor index
	BOOL bShowCursor; // if show the cursor
	int32 hBlinkTimer; // the blink timer
	PCWSTR pIntroduce;  // to introduce what is input
}EDITDATA, *PEDITDATA;

/////////////////////////////////////////////////////////////////////////////////////


#define SMP_EDIT_MOVE_CURSOR_TO_BEGIN(hEdit, pData)  \
	pData->iCursor = pData->iLineBegin = 0; \
	SMP_Edit_AdjustTextLineInfoFromBegin(hEdit)


#define SMP_EDIT_MOVE_CURSOR_TO_END(hEdit, pData) \
	pData->iCursor = pData->iLineEnd = wstrlen(pData->buffer); \
	SMP_Edit_AdjustTextLineInfoFromEnd(hEdit)
	


static VOID SMP_Edit_AdjustTextLineInfoFromBegin(HWND hEdit)
{
	int32 w, h, len, lineWidth = 2*SMP_ITEM_CONTENT_MARGIN;
	HFONT font = SGL_GetSystemFont();
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	UCHAR word[4] = {0, };
	PCWSTR pEnd;

	if(_IS_SET_ANY(hEdit, ES_PWD))
	{
		word[1] = PWDCHAR;
		fontwh((PSTR)word, TRUE, (uint16)font, &w, &h);
		pData->iLineEnd = pData->iLineBegin + DIV(_WIDTH(hEdit) - 2*SMP_ITEM_CONTENT_MARGIN, w)*2;
		len = wstrlen(pData->buffer);
		if(pData->iLineEnd > len) pData->iLineEnd = len;
	}else{
		for(pEnd = pData->buffer + pData->iLineBegin; pEnd[0] || pEnd[1]; pEnd += 2)
		{
			word[0] = pEnd[0]; word[1] = pEnd[1];
			fontwh((PSTR)word, TRUE, (uint16)font, &w, &h);
			if((lineWidth += w) > _WIDTH(hEdit)) break;
		}
		pData->iLineEnd = pEnd - pData->buffer;
	}
}


static VOID SMP_Edit_AdjustTextLineInfoFromEnd(HWND hEdit)
{
	int32 w, h, lineWidth = 2*SMP_ITEM_CONTENT_MARGIN;
	HFONT font = SGL_GetSystemFont();
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	UCHAR word[4] = {0, };
	PCWSTR pBegin;

	if(_IS_SET_ANY(hEdit, ES_PWD))
	{
		word[1] = PWDCHAR;
		fontwh((PSTR)word, TRUE, (uint16)font, &w, &h);
		pData->iLineBegin = pData->iLineEnd - DIV(_WIDTH(hEdit) - 2*SMP_ITEM_CONTENT_MARGIN, w)*2;
		if(pData->iLineBegin < 0) pData->iLineBegin = 0;
	}else{
		for(pBegin = pData->buffer + pData->iLineEnd - 2; pBegin >= pData->buffer; pBegin -= 2)
		{
			word[0] = pBegin[0]; word[1] = pBegin[1];
			fontwh((PSTR)word, TRUE, (uint16)font, &w, &h);
			if((lineWidth += w) > _WIDTH(hEdit)) break;
		}
		pData->iLineBegin = pBegin - pData->buffer + 2;
	}
}

VOID SMP_Edit_AdjustPosEnd(HWND hEdit)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	SMP_EDIT_MOVE_CURSOR_TO_END(hEdit, pData);
}

static VOID SMP_Edit_InsertCharAtCursor(HWND hEdit, CHAR c)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	int len = wstrlen(pData->buffer);
		
	if(len >= pData->size - 2)
		return;

	SGL_MEMMOVE(pData->buffer + pData->iCursor + 2, pData->buffer + pData->iCursor , len - pData->iCursor + 2);
	pData->buffer[pData->iCursor] = 0;
	pData->buffer[pData->iCursor + 1] = c;

	pData->iCursor += 2;
	pData->iLineEnd += 2;
	SMP_Edit_AdjustTextLineInfoFromEnd(hEdit);

	pData->bShowCursor = TRUE;
	SGL_UpdateWindow(hEdit);
	SGL_NotifyParent(hEdit, SMP_EDITN_TEXTCHANGED, hEdit);
}


static BOOL SMP_Edit_RemoveCharAtCursor(HWND hEdit)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	int len;
		
	if(pData->iCursor < 2)
		return FALSE;

	len = wstrlen(pData->buffer);
	SGL_MEMMOVE(pData->buffer + pData->iCursor - 2, pData->buffer + pData->iCursor , len - pData->iCursor + 2);

	pData->iCursor -= 2;
	pData->iLineEnd -= 2;
	SMP_Edit_AdjustTextLineInfoFromEnd(hEdit);

	pData->bShowCursor = TRUE;
	SGL_UpdateWindow(hEdit);
	SGL_NotifyParent(hEdit, SMP_EDITN_TEXTCHANGED, hEdit);	
	return TRUE;
}


VOID SMP_Edit_SetInfo(HWND hEdit, PCWSTR title, PWSTR text, int maxsize)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	
	pData->title = title;
	pData->buffer = text;
	pData->size = maxsize;
}

void SMP_Edit_SetIntroduceText(HWND hEdit, PCWSTR pIntroduce)
{
    PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);

    pData->pIntroduce = pIntroduce;
}


BOOL SMP_Edit_GetInfo(HWND hEdit, PCWSTR* title, PWSTR* text, int* maxsize)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	
	if(title) *title = pData->title;
	if(text) *text = pData->buffer;
	if(maxsize) *maxsize = pData->size;
	return TRUE;
}


PWSTR SMP_Edit_GetText(HWND hEdit)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	return pData->buffer;	
}


VOID SMP_Edit_SetText(HWND hEdit, PCWSTR pstr, BOOL redraw, BOOL notify)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	wstrncpy(pData->buffer, pstr, pData->size);

    if(_IS_SET_ALL(hEdit, SMP_EDITS_TRIM_CR_LF))
    {
        trimCRLF(pData->buffer);
    }

    if(_IS_SET_ALL(hEdit, SMP_EDITS_TRIM_SPACE))
    {
        trim((PSTR)pData->buffer);
    }
    
//	if(SGL_IsWindowVisible(hEdit))
	{
		if(_IS_SET_ANY(hEdit, WS_FOCUSED))
		{
			SMP_EDIT_MOVE_CURSOR_TO_END(hEdit, pData);
		}else{
			SMP_EDIT_MOVE_CURSOR_TO_BEGIN(hEdit, pData);
		}
	}
	
	if(redraw)
		SGL_UpdateWindow(hEdit);

	if(notify)
		SGL_NotifyParent(hEdit, SMP_EDITN_TEXTCHANGED, hEdit);
}


static VOID SMP_Edit_BlinkCallback(int32 data)
{
	HWND hEdit = (HWND)data;
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
    if(SGL_IsSuspendDrawing())
    {
        return;
    }
    
	if (NULL != SGL_GetModalWindow() || NULL != SGL_GetPopupMenu())
	{
	//	SMP_Edit_StopBlink(hEdit, FALSE);
		return;
	}
	pData->bShowCursor = !pData->bShowCursor;
	SGL_UpdateWindow(hEdit);
}


VOID SMP_Edit_StartBlink(HWND hEdit, BOOL update)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	
	if(!pData->hBlinkTimer)
		pData->hBlinkTimer = mrc_timerCreate();

	pData->bShowCursor = TRUE;
	mrc_timerStart(pData->hBlinkTimer, SMP_EDIT_BLINK_INTERVAL, (int32)hEdit, SMP_Edit_BlinkCallback, TRUE);
	if(update) SGL_UpdateWindow(hEdit);
}


VOID SMP_Edit_SetCursorPosition(HWND hEdit, int pos, BOOL update)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);

	if(pos == pData->iCursor || pos < 0 || pos > wstrlen(pData->buffer))
		return;
	
	if(pos < pData->iLineBegin)
	{
		pData->iLineBegin = pos;
		SMP_Edit_AdjustTextLineInfoFromBegin(hEdit);
	}else if(pos > pData->iLineEnd){
		pData->iLineEnd = pos;
		SMP_Edit_AdjustTextLineInfoFromEnd(hEdit);
	}
	
	pData->iCursor = pos;
	if(update)
	{
		pData->bShowCursor = TRUE;
		SGL_UpdateWindow(hEdit);
	}
}


VOID SMP_Edit_StopBlink(HWND hEdit, BOOL update)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	if(pData->hBlinkTimer)
		mrc_timerStop(pData->hBlinkTimer);

	pData->bShowCursor = FALSE;
	if(update) SGL_UpdateWindow(hEdit);
}


LRESULT SMP_Edit_WndProc(HWND hEdit, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PEDITDATA pData = _GET_WINDATA(hEdit, PEDITDATA);
	
	switch(Msg) 
	{

	case WM_CREATE:
	{
		pData = (PEDITDATA)SGL_MALLOC(sizeof(EDITDATA));
		if(NULL == pData){
			SGL_TRACE("%s, %d: memory out\n", __FILE__, __LINE__);
			return 1;
		}

		SGL_MEMSET(pData, 0, sizeof(EDITDATA));
		_SET_WINDATA(hEdit, pData);
		break;
	}

	case WM_DESTROY:
	{
		if(pData){
			if(pData->hBlinkTimer) mrc_timerDelete(pData->hBlinkTimer);
			SGL_FREE(pData);
		}
		break;
	}

	case WM_SHOW:
	{
		//SMP_EDIT_MOVE_CURSOR_TO_BEGIN(hEdit, pData);
		if(_IS_SET_ANY(hEdit, WS_FOCUSED))
		{
			SMP_EDIT_MOVE_CURSOR_TO_END(hEdit, pData);
		}else{
			SMP_EDIT_MOVE_CURSOR_TO_BEGIN(hEdit, pData);
		}
		break;
	}
	
	case WM_PAINT:
	{
		int x = _LEFT(hEdit);
		int y = _TOP(hEdit);
		UCHAR textline[SMP_MAXCHARS_PERLINE] = {0,};
		int32 w, h, i;			

		SGL_WindowToScreen(_PARENT(hEdit), &x, &y);
		GAL_Rectangle(PHYSICALGC, x, y, _WIDTH(hEdit), _HEIGHT(hEdit), COLOR_black);
		GAL_DrawVLine(PHYSICALGC, x, y, _HEIGHT(hEdit)-2, 0x00BBBBBB);
		GAL_DrawHLine(PHYSICALGC, x, y, _WIDTH(hEdit), 0x00BBBBBB);
		GAL_DrawHLine(PHYSICALGC, x, y+_HEIGHT(hEdit)-2, _WIDTH(hEdit), 0x00BBBBBB);
		GAL_DrawVLine(PHYSICALGC, x+_WIDTH(hEdit)-1, y, _HEIGHT(hEdit)-1, 0x00BBBBBB);

		//get the display text into buffer textline
		if(_IS_SET_ANY(hEdit, ES_PWD))
		{
			for(i = pData->iLineEnd - pData->iLineBegin - 1; i > 0; i-=2)
				textline[i] = PWDCHAR;
		}else{
			wstrncpy(textline, pData->buffer + pData->iLineBegin, pData->iLineEnd - pData->iLineBegin + 2);
		}	

		//adjust the x position by the align
		fontwh((PSTR)textline, TRUE, (uint16)SGL_GetSystemFont(), &w, &h);
		if(_IS_SET_ANY(hEdit, SMP_EDITS_HRIGHT) && w < _WIDTH(hEdit) - 2*SMP_ITEM_CONTENT_MARGIN 
			&& !pData->buffer[pData->iLineEnd] && !pData->buffer[pData->iLineEnd+1])
			x = x + _WIDTH(hEdit) - SMP_ITEM_CONTENT_MARGIN - w;
		else
			x += SMP_ITEM_CONTENT_MARGIN;

		//show the text
		gbsez((PSTR)textline, (int16)x, (int16)(y + DIV(_HEIGHT(hEdit) - h, 2)), 
			PIXEL888RED(_FGCOLOR(hEdit)), PIXEL888GREEN(_FGCOLOR(hEdit)), PIXEL888BLUE(_FGCOLOR(hEdit)), 1, (uint16)SGL_GetSystemFont());

        //show introduce text        
        if((0 == wstrlen(pData->buffer)) && pData->pIntroduce)
        {
            fontwh((PSTR)pData->pIntroduce, TRUE, (uint16)SGL_GetSystemFont(), &w, &h);
            gbsez((PSTR)pData->pIntroduce, (int16)x, (int16)(y + DIV(_HEIGHT(hEdit) - h, 2)), 
			PIXEL888RED(_FGCOLOR(hEdit)), PIXEL888GREEN(_FGCOLOR(hEdit)), PIXEL888BLUE(_FGCOLOR(hEdit)), 1, (uint16)SGL_GetSystemFont());
        }
		//show the cursor
		if(_IS_SET_ANY(hEdit, WS_FOCUSED) && pData->bShowCursor)
		{
			PWSTR pCursor = textline + pData->iCursor - pData->iLineBegin;
			pCursor[0] = pCursor[1] = 0;
			fontwh((PSTR)textline, TRUE, (uint16)SGL_GetSystemFont(), &w, &h);
			GAL_DrawVLine(PHYSICALGC, (int16)(x + w), y+2, _HEIGHT(hEdit)-4, _FGCOLOR(hEdit));
		}
		return 0;
	}

	case WM_KEYDOWN:
	case WM_KEYDOWNREPEAT:
	{
		switch(wParam)
		{
		case MR_KEY_RIGHT:
			SMP_Edit_SetCursorPosition(hEdit, pData->iCursor + 2, TRUE);
			return 1;
		case MR_KEY_LEFT:
			SMP_Edit_SetCursorPosition(hEdit, pData->iCursor - 2, TRUE);
			return 1;
		case MR_KEY_POUND:
			return SMP_Edit_RemoveCharAtCursor(hEdit);
		case MR_KEY_0: case MR_KEY_1: case MR_KEY_2: case MR_KEY_3: case MR_KEY_4:
		case MR_KEY_5: case MR_KEY_6: case MR_KEY_7: case MR_KEY_8: case MR_KEY_9:
			{
				SMP_Edit_InsertCharAtCursor(hEdit, (CHAR)('0' + wParam - MR_KEY_0));
			}

			return 1;
		}
		break;
	}

	case WM_KEYUP:
	{
		switch(wParam)
		{
		case MR_KEY_SELECT:
			SGL_TextInput(hEdit);
			return 1;
		}
		break;
	}
	
	case WM_MOUSEUP:
	{
		if (_IS_SET_ANY(hEdit, WS_TABSTOP)) 
		{
			SGL_TextInput(hEdit);
		}

		break;
	}

	case WM_SETFOCUS:
	{
		SMP_EDIT_MOVE_CURSOR_TO_END(hEdit, pData);
		SMP_Edit_StartBlink(hEdit, TRUE);
		break;
	}
	
	case WM_KILLFOCUS:
	{	
		SMP_EDIT_MOVE_CURSOR_TO_BEGIN(hEdit, pData);
		SMP_Edit_StopBlink(hEdit, TRUE);
		break;
	}

	case WM_GETEDITINFO:
	{
		SGL_EditInfo* info = (SGL_EditInfo*)wParam;
		info->title = pData->title;
		info->buffer = pData->buffer;
		info->size = pData->size;
		break;
	}

	case WM_SETEDITTEXT:
	{
		SMP_Edit_SetText(hEdit, (PCWSTR)wParam, FALSE, TRUE);
		break;
	}

	}

	return 0;
}


