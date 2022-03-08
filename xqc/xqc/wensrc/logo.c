#include <mrc_base.h>
#include <mrc_bmp.h>
#include <logo.h>
#include <bmp.h>


int32 scrtimer,scrdata;
int scr_x,scr_xi,scr_y,scr_yi,scr_w,scr_h,LOGO_WIDTH,LOGO_HEIGHT;
void *scr_p;
uint16 *scr_wp,*scrbmp;

void logo_xian(int32 data)
{
	if(data==-1)
	{
		scrdata=0;
		mrc_refreshScreen(scr_x,scr_y,scr_w,scr_h);
		mrc_timerStop(scrtimer);
		SGL_ReleaseBitmap(BMP_SCRLOGO);
		mrc_free(scrbmp);
		return;
	}
	if(!data)
	{
		scrdata=1;
		scr_xi=scr_yi=3;
		scr_wp=w_getScreenBuffer();
		scr_p=SGL_LoadBitmap(BMP_SCRLOGO,&scr_w,&scr_h);
		scrbmp=mrc_malloc(LOGO_WIDTH*scr_h*2);
		mrc_timerStart(scrtimer,100,scrdata,logo_xian,1);
	}
	else
	{
		int qx=scr_x,qy=scr_y;
		scr_x+=scr_xi;
		scr_y+=scr_yi;
		scr_xi=scr_x<5?5:(scr_x+scr_w>LOGO_WIDTH-5?-5:scr_xi);
		scr_yi=scr_y<5?5:(scr_y+scr_h>LOGO_HEIGHT-5?-5:scr_yi);
		if(!scrbmp)
			return;
		memcpy(scrbmp,scr_wp+LOGO_WIDTH*scr_y,LOGO_WIDTH*scr_h*2);
		mrc_bitmapShowEx(scr_p,scr_x,scr_y,scr_w,scr_w,scr_h,BM_TRANSPARENT,0,0);
		mrc_refreshScreen(scr_x,scr_y,scr_w,scr_h);
		mrc_refreshScreen(qx,qy,scr_w,scr_h);
		mrc_bitmapShowEx(scrbmp,0,scr_y,(uint16)LOGO_WIDTH,(uint16)LOGO_WIDTH,scr_h,2,0,0);
	}
}
void logo_init()
{
	mr_screeninfo scrinfo;

	mrc_getScreenInfo(&scrinfo);
	LOGO_WIDTH=scrinfo.width;
	LOGO_HEIGHT=scrinfo.height;
	scr_x=scr_y=50;
	scrtimer=mrc_timerCreate();
	scrdata=0;
}
void logo_event()
{
	if(scrdata>0)
		logo_xian(-1);
	else
		mrc_timerStart(scrtimer,60000,scrdata,logo_xian,0);
}

void logo_free()
{
	if(scrdata>0)
		logo_xian(-1);
	mrc_timerDelete(scrtimer);
}
void logo_pause()
{
	mrc_timerStop(scrtimer);
}
void logo_resume()
{
	logo_event();
}