#include <mrc_base.h>
#include <font.h>
int32 pp1,pp2,kuangge;
extern int SCRW,SCRH;
int kuangh;
#define  churect(x,y,w,h)   pp1>x&&pp2>y&&pp1<x+w&&pp2<y+h
typedef struct{
	char *dt;
	int16 id;
	int16 x;
	int16 y;
	int16 w;
	int16 h;
}kuangn;
kuangn kuangp[7];
void kuang(int id)
{
	mrc_drawRect(kuangp[id].x,kuangp[id].y,kuangp[id].w,kuangp[id].h,100,200,200);
	gbsez(kuangp[id].dt,kuangp[id].x,kuangp[id].y,0,0,0,0,0);
}
void nowkuang(char *a,int16 x,int16 y,int16 w,int16 h,int16 id)
{
	kuangp[kuangge].id=id;
	kuangp[kuangge].x=x;
	kuangp[kuangge].y=y;
	kuangp[kuangge].w=w;
	kuangp[kuangge].h=h;
	kuangp[kuangge].dt=a;
	kuangge++;
}

void huakuan()
{
	int i;
	mrc_drawRect(0,SCRH,SCRW,kuangh,150,150,150);
	for(i=0;i<kuangge;i++)
		kuang(i);
	mrc_refreshScreen(0,SCRH,SCRW,kuangh);
}
int pdkuang()
{
	int i;
	huakuan();
	for(i=0;i<kuangge;i++)
		if(churect(kuangp[i].x,kuangp[i].y,kuangp[i].w,kuangp[i].h))
			return i;
	return -1;
}
void kuanginit()
{
	kuangh=80;
	nowkuang("×ó¼ü",0,SCRH-20,50,20,0);
	nowkuang("ÓÒ¼ü",SCRW-50,SCRH-20,50,20,1);
	nowkuang("ÉÏ",SCRW/2-20,SCRH-kuangh+10,30,20,3);
	nowkuang("ÏÂ",SCRW/2-20,SCRH-kuangh+50,30,20,4);
	nowkuang("×ó",SCRW/2-50,SCRH-kuangh+30,30,20,5);
	nowkuang("ÓÒ",SCRW/2+15,SCRH-kuangh+30,30,20,6);
	nowkuang("ÖÐ",SCRW/2-15,SCRH-kuangh+30,20,20,7);
	SCRH-=kuangh;
}

int32 eventan(int32 *meg,int32 *p1,int32 *p2)
{
	pp1=*p1;pp2=*p2;
	if(*p2<SCRH)
		return 0;
	if(*meg==MR_MOUSE_UP||*meg==MR_MOUSE_DOWN)
	{
		*meg-=2;
		switch(pdkuang())
		{
		case  0:
			*p1=MR_KEY_SOFTLEFT;
			break;
		case  1:
			*p1=MR_KEY_SOFTRIGHT;
			break;
		case  2:
			*p1=MR_KEY_UP;
			break;
		case  3:
			*p1=MR_KEY_DOWN;
			break;
		case  4:
			*p1=MR_KEY_LEFT;
			break;
		case  5:
			*p1=MR_KEY_RIGHT;
			break;
		case  6:
			*p1=MR_KEY_SELECT;
			break;
		case  7:
			*p1=MR_KEY_SOFTRIGHT;
			break;
		}
	}
	return 0;
}