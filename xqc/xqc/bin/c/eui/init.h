

int timerpf=timercreate();
int p0,px,py,timep;
int dialogf;
int c,j;
int tw,th;
char *ptrdt=malloc(512);
int init(void)
{chushhhua();
xian(0);
    return 0;
}

void chushhhua()
{tishi(lang(0,5),1);
mkdir("Ò×ui");
shu.x=SCRW/2;shu.y=SCRH/2;shu.w=shu.h=20;
rgb.r=200;
textwh("¸ß",0,0,&tw,&th);
for(int i;i<20;cddt[i++]=malloc(50));
memset(ptrdt,0,512);
qh.now=NULL;
refzu();
}

int eventmain(int type, int p1, int p2)
{if(KY_DOWN == type)
{
if(p1>0&&p1<17){loadxian(p1);}
switch(p1)
{
case _SELECT:
if(j==1){j=3;if(now()==NULL||now()==qh.last)xian(0);else refzu();loadtu();j=0;xian(0);}
else{loadzu();j=1;
wxy *obj=now();
int nn=obj->leixing;
if(nn==7||nn==8||nn==11||nn==12)
{if(nn==11||nn==12)shuru(lang(0,8),ptrdt,1);else shuru(NULL,ptrdt,1);}else{xian(0);}
}break;
case _SRIGHT:
if(j==1){delzu(now());j=0;xian(0);}
break;
}}
if(KY_UP==type)
{temp=-3;timerstop(timerf);
if(!p1){if(!qhzu(1)){refzu();j=1;xian(0);}}
if(p1==_STAR){if(!qhzu(0)){refzu();j=1;xian(0);}}


if(p1==_SLEFT){c=200;caidan(1);}
if(p1==_POUND){refzu();xian(0);}
}
return 0;}
int eventcd(int type, int p1, int p2)
{if(KY_DOWN == type)
{
switch(p1)
{case _SELECT:

if(c>=200&&c<=220){event(1,_SLEFT,0);return 0;}
if(c==241||c==242){if(colorj){colorst a;rgb24(colortu[ts.x+ts.y*174],&a);ts.r.r=a.r;ts.r.g=a.g;ts.r.b=a.b;ztiaose();}else{dialogf=editcreate(lang(0,7),NULL,1,4);}}
break;

case _SRIGHT:
if(c==200||c==500){c=-1;}
if(c==210||c>=240&&c<260){c=200;caidan(1);}
if(c>210&&c<=230){c=210;caidan(3);cdy=leixing;caidan(2);}
break;

case _UP:
if(c==200||c>=210&&c<=230||c==240||c==250||c==251){cdy--;caidan(0);eventxh(200);}
if(c==241||c==242)
{if(colorj){ts.y=ts.y<1?255/2-1:ts.y-1;ztiaose();}else{void *b=&ts.r;int16 *a=b;a[ts.x-1]--;pdcolor(&a[ts.x-1]);tiaose();}eventxh(20);}
break;

case _DOWN:
if(c==200||c>=210&&c<=230||c==240||c==250||c==251){cdy++;caidan(0);eventxh(200);}
if(c==241||c==242)
{if(colorj){ts.y=ts.y>255/2-1?1:ts.y+1;ztiaose();}else{void *b=&ts.r;int16 *a=b;a[ts.x-1]++;pdcolor(&a[ts.x-1]);tiaose();}eventxh(20);}
break;
case _LEFT:if(c==241||c==242){if(colorj){ts.x=ts.x<1?174:ts.x-1;ztiaose();eventxh(10);}else{ts.x=ts.x<=1?3:ts.x-1;tiaose();}}break;
case _RIGHT:if(c==241||c==242){if(colorj){ts.x=ts.x>174?1:ts.x+1;ztiaose();eventxh(10);}else{ts.x=ts.x>=3?1:1+ts.x;tiaose();}}break;
}
}

if(KY_UP==type)
{
if(p1==_SLEFT){
switch(c)
{case 500:exitapp();break;

case 200:if(cdy==1){c=210;caidan(3);cdy=leixing;caidan(2);break;}if(cdy==2){refzu();c=-1;}if(cdy>=3&&cdy<=5){c+=cdy*10;caidan(1);}if(cdy==6){c=500;tuichu();}break;
case 210:leixing=cdy;c=-1;if(cdy>=6&&cdy<=11){c=210+cdy;caidan(3);cdy=mode+1;caidan(2);}break;
case 216:case 217:case 218:case 219:case 220:case 221:mode=cdy-1;c=-1;break;
case 230:tishi(lang(0,10),1);int a;if(cdy==1)a=baocun();if(cdy==2)a=bjietu();if(cdy==3)a=lishi();if(cdy==4)a=loadlishi();if(a)tishi(lang(0,12),1);else tishi(lang(0,11),1);sleep(300);c=-1;break;
case 240:c=cdy+240;tiaosebc(1);if(colorj)ztiaose();else tiaose();break;
case 241:case 242:tiaosebc(2);c=-1;break;
case 250:c=250+cdy;if(cdy==1)caidan(1);if(cdy==2)bangzhu();if(cdy==3)guanyu();break;
case 251:font=cdy-1;c=-1;break;
}
}
if(p1==_POUND){
if(c==241||c==242){if(colorj){colorj=0;tiaose();}else{colorj=1;if(!jzcolor())ztiaose();else tishi("»æÖÆÊ§°Ü",0);} }}
}
return 0;}

int event(int type,int p1,int p2)
{p0=type;px=p1;py=p2;
if(c>=200&&c<=500)
{eventcd(type,p1,p2);}
if(!c)
{eventmain(type,p1,p2);}
if(type==KY_UP)
if(c==-1)
{c=0;xian(0);}
  
  
    if(type == MR_DIALOG)
    {
        switch(p1)
        {
        case DLG_OK:
char *ptr=editget(dialogf);
uint16 len=wstrlen(ptr)+2;
char *text=malloc(len);
memset(text,0,len);
memset(ptrdt,0,512);
memcpy(text,ptr,len-2);
memcpy(ptrdt,ptr,len-2);
editdel(dialogf);

if(!c){wxy *obj=now();char *a=text;if((obj->mode%2==0)|(obj->leixing>10)){a=uc2(text,0);free(text);}obj->text=a;xian(0);}
if(c==241||c==242)
{void *b=&ts.r;int16 *a=b;a[ts.x-1]=atoi(uc2(text,0));pdcolor(&a[ts.x-1]);tiaose();}
            break;
        case DLG_CANCEL:
            editdel(dialogf);
if(!c){xian(0);}
if(c==241||c==242){tiaose();}
break;
}
}
return 0;
}

int pause(void)
{
    return 0;
}

int resume(void)
{c=0;xian(0);
    return 0;
}

void exitapp(void)
{timerdel(timerpf);
timerdel(timerf);
free(ptrdt);
free(colortu);
for(;qh.now!=NULL;delzu(now()));
exit();
}
void eventxh(int i)
{if(i>999)
event(p0,px,py);
else
timerstart(timerf,i,1000,"eventxh",0);
}