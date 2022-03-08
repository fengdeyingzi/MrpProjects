#include <mrc_base.h>
#include <mrc_bmp.h>
#include <mrc_base_i.h>

void *filenei(const char *name,uint32 *Len)
{
	int32 len=0;
	void *p=NULL,*P=NULL;
	Len[0]=0;
	p=mrc_readFileFromMrp(name, &len, 0);
	if(p==NULL)return NULL;
	P=mrc_malloc(len+2);
	if(P==NULL)return NULL;
	P=memcpy(memset(P,0,len+2),p,len);
	mrc_freeFileData(p,len);
	*Len=len;
	return P;
}

void bmpsuo(uint16 *tuf,int x,int y,uint16 ws,uint16 ww,uint16 hh,uint16 w,uint16 h,uint16 rop,uint16 xs,uint16 ys)
{
	int ix,iy;
	mr_screeninfo pt;
	mrc_getScreenInfo(&pt);
	for(iy=0;iy<h+(y<0?y:0);iy++)
		if(iy+y>=pt.height)
			break;
		else
			for(ix=0;ix<w+(x<0?x:0);ix++)
				if(ix+x>pt.width)
					break;
				else
					mrc_bitmapShowExTrans(tuf+ww*ix/w+(iy*hh/h+ys+(y<0?-y:0))*ws+xs+(x<0?-x:0),(x<0?0:x)+ix,iy+(y<0?0:y),ws,1,1,rop,0,0,*tuf);
}

int32 runmrps(char *name)
{
	char *qd;
	if(mrc_fileState("xqc/mtk.txt")==MR_IS_FILE)
		qd="start.mr";
	else
		qd="cfunction.ext";
	return mrc_runMrp(name,qd,NULL);
}
int32 rundata;
void PicocRun(int32 data)
{

	if(!data)
	{
		uint16 *p;uint32 len,i=1200;
		int w=320,h=214;
		mr_screeninfo pt;
		mrc_getScreenInfo(&pt);
		p=filenei("logo.bmp",&len);
		if(p==NULL)
			i=1;
		rundata=data=mrc_timerCreate();
		mrc_timerStart(data,i,data,PicocRun,0);
		mrc_clearScreen(255,255,255);
		bmpsuo(p,0,(pt.height-h*pt.width/w)/2,w,w,h,pt.width,h*pt.width/w,2,0,0);
		mrc_refreshScreen(0,0,pt.width,pt.height);
		mrc_free(p);
	}
	else
	{
		mrc_timerDelete(data);
		runmrps("xqc/editor.mrp");
	}
}

int32 mrc_init(void)
{
	PicocRun(0);
	return MR_SUCCESS;
}



int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	PicocRun(rundata);
	return MR_SUCCESS;
}

int32 mrc_pause(void)
{
	return 0;
}

int32 mrc_resume(void)
{
	return 0;
}


int32 mrc_exitApp(void)
{
	return 0;
}
