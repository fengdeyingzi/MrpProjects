#include <base.h>
#include <tcc.h>
#include <mrc_base_i.h>

mr_screeninfo base_scrinfo;
int32 SCRWH(int type)
{
	if(!base_scrinfo.width)
	{
		mrc_getScreenInfo(&base_scrinfo);
	}
	switch(type)
	{
	case 0:
		return base_scrinfo.width;
	case 1:
		return base_scrinfo.height;
	case 2:
		return base_scrinfo.bit;
	}
	return 0;
}
int32 dtextex(char* pcText, int16 x, int16 y, rectst *rect, colorst *color, int flag, uint16 font)
{
	mr_screenRectSt rects;
	mr_colourSt colors;
	memcpy(&rects,rect,sizeof(mr_screenRectSt));
	memcpy(&colors,color,sizeof(mr_colourSt));
	mrc_drawTextEx(pcText,x,y,rects,colors,flag,font);
}

int32 dtextright(char* pcText, int16 x, int16 y, rectst *rect, colorst *color, int flag, uint16 font)
{
	mr_screenRectSt rects;
	mr_colourSt colors;
	memcpy(&rects,rect,sizeof(mr_screenRectSt));
	memcpy(&colors,color,sizeof(mr_colourSt));
	mrc_drawTextRight(pcText,x,y,rects,colors,flag,font);
}
int32 timerstart (int32 t, int32 time, int32 data, char *f, int32 loop)
{
	mrc_timerCB fs;
	fs=(void(*)(int32))tcc_get_symbol(GetMainsf(),f);
	if(fs!=NULL)
		mrc_timerStart(t,time,data,fs,loop);
}

int DrawIMG(char* filename, int x,int y)
{
	PMRAPP_IMAGE_WH p;
	MRAPP_IMAGE_ORIGIN_T t_imgsizeInfo;
	T_DRAW_DIRECT_REQ input;

	//获取图片尺寸信息
	t_imgsizeInfo.src = (char*)filename;
	t_imgsizeInfo.len = mrc_strlen(filename);
	t_imgsizeInfo.src_type = SRC_NAME;
	mrc_getImageInfo(&t_imgsizeInfo, (uint8**)&p);

	//设置传到移植层得参数
	input.src_type = 0;
	input.ox = x;
	input.oy = y;
	input.w = p->width;
	input.h = p->height;
	input.src = (char*)filename;
	input.src_len = mrc_strlen(filename);

	return mrc_platEx(3010, (uint8*)&input, sizeof(T_DRAW_DIRECT_REQ), NULL, NULL, NULL);
}

enum
{
	SHADE_UPDOWN,		//从上到下
	SHADE_LEFTRIGHT,	//从左到右
	SHADE_DOWNUP,		//从下到上
	SHADE_RIGHTLEFT		//从右到左
};
void ShadeRect(int x, int y, int w, int h, int AR, int AG, int AB, int BR, int BG, int BB, int mode)
{
	int16 i,j,t;

	BR-=AR; BG-=AG; BB-=AB;
	switch(mode)
	{
	case SHADE_UPDOWN:
		t=x+w-1;
		for(i=0;     i<h;     i++)
			mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*i/h),(uint8)(AG+BG*i/h),(uint8)(AB+BB*i/h));
		return;
	case SHADE_DOWNUP:
		t=x+w-1;
		for(i=h-1,j=0;    i>=0;    i--,j++)
			mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*j/h),(uint8)(AG+BG*j/h),(uint8)(AB+BB*j/h));
		return;
	case SHADE_LEFTRIGHT:
		t=y+h-1;
		for(i=0;     i<w;     i++)
			mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*i/w),(uint8)(AG+BG*i/w),(uint8)(AB+BB*i/w));
		return;
	case SHADE_RIGHTLEFT:
		t=y+h-1;
		for(i=w-1,j=0;    i>=0;    i--,j++)
			mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*j/w),(uint8)(AG+BG*j/w),(uint8)(AB+BB*j/w));
		return;
	}
}
void shaderect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode)
{
	//RGBA+(RGBB-RGBA)*N/Step
	int32 AR,AG,AB;
	int32 BR,BG,BB;

	AR = (uint8)(pixelA >> 16) & 0xff;
	AG = (uint8)(pixelA >> 8) & 0xff;
	AB = (uint8)(pixelA) & 0xff;

	BR = (uint8)((pixelB >> 16) & 0xff);
	BG = (uint8)((pixelB >> 8) & 0xff);
	BB = (uint8)((pixelB) & 0xff);
	ShadeRect(x,y,w,h,AR,AG,AB,BR,BG,BB,mode);

}