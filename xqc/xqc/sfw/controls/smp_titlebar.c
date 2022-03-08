#include "smp_titlebar.h"
#include "string.h"
#include "smp.h"
#include "bmp.h"

/*白天模式颜色*/
#define COLOR_001   0x00c4dfe9 //外围边框

int32 timer;     // 刷新本地时间定时器

//标题栏数据结构
typedef struct SMP_TitlebarData
{
	DWORD bmp;       // BMP图片标识
	PCWSTR str;      // 显示的标题文字
	int32 len;       // 字符长度字节数
	int32 width;     // 字符总宽度
	int32 height;    // 字符高度
}LABELDATA, *PLABELDATA;

static VOID UpdateTime(int32 data)
{
	HWND hTitlebar = (HWND)data;
	SGL_UpdateWindow(hTitlebar);
}

//显示本地时间
VOID ShowLocalTime(uint32 color)
{
	int32 width,height;
	int16 x,y;
	PSTR timenow;
	mr_datetime time;

	mrc_getDatetime(&time);
	timenow = (PSTR)MR_MALLOC(6);
	if(time.minute <= 9)  //如果分钟小于9则为0*
		mrc_sprintf(timenow,"%d%s%d",time.hour,":0",time.minute);
	else 
		mrc_sprintf(timenow,"%d%s%d",time.hour,":",time.minute);

	fontwh(timenow,FALSE,MR_FONT_SMALL,&width,&height);
	x = (int16)(SCREEN_WIDTH - width - 5);
	y = DIV((SMP_HEADER_HEIGHT - (int16)height),2);
	gbsez(timenow, x, y, PIXEL888RED(color), PIXEL888GREEN(color), PIXEL888BLUE(color), 0, MR_FONT_SMALL);
	mrc_free(timenow);
}

//设置标题栏内容
VOID SMP_Titlebar_SetContent(HWND hWnd, DWORD bmpID, PCWSTR content)
{
	PLABELDATA pData = _GET_WINDATA(hWnd, PLABELDATA);

	pData->str = content;
	pData->bmp = bmpID;
	
	fontwh((PSTR)pData->str, TRUE, (uint16)SGL_GetSystemFont(), &pData->width, &pData->height);
	pData->len = wstrlen(pData->str);
}

//标题栏窗口事件
LRESULT SMP_Titlebar_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PLABELDATA pData = _GET_WINDATA(hWnd, PLABELDATA);
	
	switch(Msg)
	{
	
	case WM_CREATE:
	{
		pData = (PLABELDATA)SGL_MALLOC(sizeof(LABELDATA));
		if(NULL == pData)
		{
			SGL_TRACE("%s, %d: memory out\n", __FILE__, __LINE__);
			return 1;
		}

		//创建刷新本地时间的定时器
		timer = mrc_timerCreate();
		
		SGL_MEMSET(pData, 0, sizeof(LABELDATA));
		_SET_WINDATA(hWnd, pData);
		break;
	}

	case WM_DESTROY:
	{
		//销毁刷新本地时间定时器
		if(timer != 0)
			mrc_timerDelete(timer);
		//释放控件数据结构
		if(pData)
			SGL_FREE(pData);
		break;
	}

	case WM_SHOW:
		// 定时器，可以传入一个参数，最后一个参数决定是否自动循环定时器
		mrc_timerStart(timer, 30000, (int32)hWnd, UpdateTime, TRUE);
		break;
	case WM_HIDE:
		mrc_timerStop(timer);
		break;

	case WM_PAINT:
	{
		int x=0, y=0, w = 0, h = 0, mid;
		HBITMAP bmp;
		mr_colourSt color = {0,};
		mr_screenRectSt rect;
		HFONT font = SGL_GetSystemFont();
		PCWSTR pContent = pData->str;

		SGL_WindowToScreen(hWnd, &x, &y);
		
		/* 标题文字颜色 */
		_FGCOLOR(hWnd) =  COLOR_lightwhite ;
		color.r = PIXEL888RED(_FGCOLOR(hWnd));
		color.g = PIXEL888GREEN(_FGCOLOR(hWnd));
		color.b = PIXEL888BLUE(_FGCOLOR(hWnd));


		rect.x = (uint16)(x + SMP_ITEM_CONTENT_MARGIN); 
		rect.y = (uint16)y;
		rect.w = (uint16)(_WIDTH(hWnd) - 2*SMP_ITEM_CONTENT_MARGIN); 
		rect.h = (uint16)_HEIGHT(hWnd);

		mid = DIV(_HEIGHT(hWnd),2);
		GAL_DrawHLine(PHYSICALGC, x, y, _WIDTH(hWnd), COLOR_black);
		FillShadeRange1(x, y+1, _WIDTH(hWnd), _HEIGHT(hWnd)-1, 0x00B4B4B4, 0x00181818);

		/* 加载logo图标 */
		if(RESID_INVALID != pData->bmp)
		{
			bmp = SGL_LoadBitmap(pData->bmp, &w, &h);
			if(bmp)
			{
				mrc_bitmapShowEx((uint16 *)bmp, (int16)rect.x, (int16)(y + DIV(_HEIGHT(hWnd)-h, 2)), (int16)w, (int16)w, (int16)h, BM_TRANSPARENT, 0, 0); //显示标题图片
				x += w + SMP_ITEM_CONTENT_MARGIN;
				rect.w -= w + SMP_ITEM_CONTENT_MARGIN;
			}
		}

		SGL_ReleaseBitmap(pData->bmp);

		x += SMP_ITEM_CONTENT_MARGIN;
		rect.x = (int16)x;
		mid = DIV(_HEIGHT(hWnd)-pData->height, 2);

		//显示标题文字
		gbsez2((PSTR)pContent, (int16)x, (int16)(y + mid - 1), rect, color, 1, (uint16)font);
		ShowLocalTime(COLOR_lightwhite);
		break;
	}

	}

	return 0;
}

