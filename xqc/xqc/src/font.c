#include <mrc_base.h>
#include <font.h>
	int32 sezjb;//¾ä±ú
	char fonttempaz[32*128];
	int32 FONT;

void fontinit()
{
	sezjb=0;
}

int32 openfont(int font)
{
	if(sezjb)
		closefont();
	if(font!=12&&font!=16)
		return 0;
	sezjb=mrc_open(font!=12?"system/gb16.uc2":"system/gb12.uc2",1);
	if(sezjb==0)
		return 0;
	mrc_read(sezjb,&fonttempaz[0],font*font/8*128);

	return sezjb;
}
int32 closefont()
{
	int32 re;
	if (!sezjb)
		return 0;
	re=mrc_close(sezjb);
	sezjb=0;
	return re;
}

int32 fontwh(char* pcText, int is_unicode, uint16 font, int32* w, int32* h)
{
	char *c;
	int32 l=0,wi=0;
	uint16 *p;
	*w= *h =0;
	if(pcText==NULL)
		return MR_FAILED;

	if(!sezjb||(font!=12&&font!=16))
		return mrc_textWidthHeight(pcText,is_unicode,font,w,h);
	if(!is_unicode)
		c=(char *)mrc_c2uVM(pcText,NULL,&l);
	else
		c=pcText;
	if(c==NULL)
		return MR_FAILED;

	p=(uint16*)c;
	*h=font;
	for(;c[wi+1]+c[wi];wi+=2)
		*w+=(c[wi+1]+(c[wi]<<8))<128?font>>1:font;

	if(!is_unicode)
		mrc_freeFileData(c,l);
	return MR_SUCCESS;
}
int gbsez(char *a,int x,int y,int r,int g,int b,int un,int font)
{
	char *pText;
	uint8 hce[32];//ÊÓÎªÐéÄâ»º´æ
	int32 len=0,ww,flen=font*font>>3;
	int xi,yi,ni,li;
	uint16 color=(r>>3<<11)+(g>>2<<5)+(b>>3),temp;
	if(!sezjb||(font!=12&&font!=16))
		return mrc_drawText(a,x,y,r,g,b,un,font);
	if(un)
	{
		pText=a;
		len=mrc_wstrlen(pText);
	}
	else
		pText=(void *)mrc_c2uVM(a,NULL,&len);

	for(li=0;li<len;li+=2)
	{
		temp=pText[li+1]+(pText[li]<<8);
		if(temp<128)
			memcpy(hce,fonttempaz+temp * flen,flen);
		else
		{
			mrc_seek(sezjb,temp * flen,0);
			mrc_read(sezjb,hce,flen);
		}
		ww=temp<128?8:font;

		for(ni=yi = 0; yi < font; yi++,ni=font==12&&temp<128?8*yi:font*yi)
			for(xi = 0; xi < ww; xi++,ni++)
				if(hce[ni >> 3] & (0x80 >> (ni & 7)))
					mrc_drawPoint(x + xi, y + yi, color);
		x+=temp<128?font>>1:font;
	}
	if(!un)
		mrc_freeFileData(pText,len);
	return 0;
}
int gbsezex(char *a,int x,int y,int r,int g,int b,int un,int font)
{
	int32 f=sezjb,re;//¾ä±ú
	openfont(font);
	re=gbsez(a,x,y,r,g,b,un,font);
	closefont();
	sezjb=f;
	return re;
}

int32 gbsez2h(char* pcText, int16 w,int flag,uint16 font)
{
	int32 l,ll=0,x1=0,y1=0,ww,hh,hhn;
	char dt[4];
	uint16 *text;
	if(!sezjb||(font!=12&&font!=16))
		return 0;
	if(!(flag&1))
		text=(void *)mrc_c2uVM(pcText,NULL,&l);
	else
		text=(uint16*)pcText;
	dt[2]=dt[3]=0;
	fontwh(dt,1,font,&ww,&hh);
	if(w<ww)
		return 0;
	hhn=hh;
	while(text[ll])
	{
		if(text[ll]==0x0a0d||text[ll]==0x0a00)
		{
			y1+=hhn;
			x1=0;
			goto tiao;
		}
		memcpy(dt,&text[ll],2);
		fontwh(dt,1,font,&ww,&hh);
		if(x1+ww>w)
		{
			x1=0;
			y1+=hh;
		}
		x1+=ww;
tiao:
		ll++;
	}
	if(!(flag&1))
		mrc_freeFileData(text,l);
	return y1;
}

int32 gbsez2(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font)
{
	int32 l,ll=0,x1=0,y1=0,ww,hh,hhn;
	char dt[4];
	uint16 color,*text;
	if(!sezjb||(font!=12&&font!=16))
		return mrc_drawTextEx(pcText,x,y,rect,colorst,flag,font);
	if(!(flag&1))
		text=(void *)mrc_c2uVM(pcText,NULL,&l);
	else
		text=(uint16*)pcText;
	color=(colorst.r>>3<<11)+(colorst.g>>2<<5)+(colorst.b>>3);

		memcpy(dt,text,2);

	dt[2]=dt[3]=0;
	fontwh(dt,1,font,&ww,&hh);
	if(rect.w<ww||rect.h<hh)
		return 0;
	hhn=hh;
	while(text[ll])
	{
		if((flag&2)==2&&(text[ll]==0x0a0d||text[ll]==0x0a00))
		{
			y1+=hhn;
			x1=x;
			goto tiao;
		}
		memcpy(dt,&text[ll],2);
		fontwh(dt,1,font,&ww,&hh);
		if(y1+hh>rect.h)break;
		if(x1+ww>rect.w)
		{
			if(!(flag&2))
				break;
			x1=0;
			y1+=hh;
		}
		gbsez(dt,x+x1,y+y1,colorst.r,colorst.g,colorst.b,1,font);
		x1+=ww;
tiao:
		ll++;
	}
	if(!(flag&1))
		mrc_freeFileData(text,l);
	return 0;
}
int32 gbsezex2(char* pcText, int16 x, int16 y, int w,int h,int r,int g,int b, int flag, uint16 font)
{
	mr_screenRectSt rect;
	mr_colourSt colorst;
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;
	colorst.r=r;
	colorst.g=g;
	colorst.b=b;
	return gbsez2(pcText,0,0,rect,colorst,flag,font);
}
