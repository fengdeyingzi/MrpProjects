#include <mrc_base.h>
#include <libini.h>
#include <highlight.h>

extern int32 FONT;
extern mr_colourSt BackCol,FontCol,FocusCol,FocusFontCol,OtherCol;//¸÷ÖÖÑÕÉ«
extern HIGHLIGHTCOR HighLightCol;

uint8 skin_16110(char a)
{
	if(a>='0'&&a<='9')
		return a-'0';
	if(a>='a'&&a<='z')
		return a-'a'+10;
	if(a>='A'&&a<='Z')
		return a-'A'+10;
	return 0;
}
uint8 skin_16210(char *a)
{
	uint8 c=0;
	c=skin_16110(a[0])<<4;
	c=skin_16110(a[1])+c;
	return c;
}
void skin_setcolors(char *a,mr_colourSt *color)
{
	color->r=skin_16210(a);
	color->g=skin_16210(a+2);
	color->b=skin_16210(a+4);
}
void skin_setcolor(char *a,mr_colourSt *color)
{
	if(a==NULL)
		return;
	if(a[0]=='0'&&a[1]=='x')
		skin_setcolors(a+2,color);
}
void skin_rwcolor(INI *pini,const char *key,mr_colourSt *color,int tepy)
{
	char v1[32];
	if(tepy)
	{
		sprintf(v1,"0x%02x%02x%02x",color->r,color->g,color->b);
		ini_append(pini,key,v1);
	}
	else
	{
		ini_get(pini,key,v1);
		skin_setcolor(v1,color);
	}
}
int skin_rw_color(int type)
{
	INI *f;
	char *name="xqc/skin.ini";
	if(type)
	{
		mrc_remove(name);
		f=ini_create(name);
	}else
		f=ini_open(name);
	if(f==NULL)
		return -1;

	skin_rwcolor(f,"EDITOR@BACK",&BackCol,type);
	skin_rwcolor(f,"EDITOR@FONT",&FontCol,type);
	skin_rwcolor(f,"EDITOR@FOCUS",&FocusCol,type);
	skin_rwcolor(f,"EDITOR@FOCUSFONT",&FocusFontCol,type);
	skin_rwcolor(f,"EDITOR@OTHER",&OtherCol,type);


	skin_rwcolor(f,"HIGHLIGHT@KEYWORD",&HighLightCol.keyword,type);
	skin_rwcolor(f,"HIGHLIGHT@ANNOTATION",&HighLightCol.annotation,type);
	skin_rwcolor(f,"HIGHLIGHT@CHARACTER",&HighLightCol.character,type);
	skin_rwcolor(f,"HIGHLIGHT@MACRO",&HighLightCol.macro,type);
	skin_rwcolor(f,"HIGHLIGHT@VARIATE",&HighLightCol.variate,type);
	skin_rwcolor(f,"HIGHLIGHT@FUNCTION",&HighLightCol.function,type);

	if(type)
		ini_save(f);

	ini_close(f);
	return 0;
}
 