#define lsmain "易ui/lishi.dat"
#define lstext "易ui/temp.txt"
int timerf=timercreate();
rects shu;
uint16 *scrf=getscrbuf();
int temp,i,mode,leixing=4,font;
color16 rgb,RGB;
char *gedt=malloc(60);
wxy *dq;

owp qh;
void loadxian(int i)
{xian(i);
if(i<1000)
timerstart(timerf,300,i+1000,"loadxian",0);
else
{timerstop(timerf);
timerstart(timerf,5,i-1000,"xian",1);}}
void xian(int fx)
{if(fx)eventf(fx);
bmp();
if(j)mo(now());
if(j!=3){if(qh.ge)
{sprintf(gedt,"%d/%d",qh.dge,qh.ge);
dtext(gedt,200,0,250,250,250,0,0);}
shuui();
int a=shu.y;
if(a<SCRH-th||a>SCRH)
cdl(0,"菜单",j?"确认":"创建",j?"删除":NULL);
re();}
}

int eventf(uint8 fx)
{
temp+=temp<30?1:0;
int16 a,b;
a=fx==1||fx==4||fx==7||fx==_LEFT?(temp>1?-temp:-1):(fx==3||fx==6||fx==9||fx==_RIGHT?(temp>1?temp:1):0);
b=fx==1||fx==2||fx==3||fx==_UP?(temp>1?-temp:-1):(fx==7||fx==8||fx==9||fx==_DOWN?(temp>1?temp:1):0);
if(j){wxy *obj;obj=qh.now;obj->w+=a;obj->h+=b;}else{shu.x+=a;shu.y+=b;}
if(fx>0&&fx<10||fx>=12&&fx<=15)
return 0;
else
return -1;
}

void shuui()
{
int ww=2;
if(!j)
{zuobiao();
int pig=10;
effsetcon(shu.x-pig/2+1,shu.y,pig,2,-255,-255,-255);
effsetcon(shu.x,shu.y-pig/2+1,2,pig,-255,-255,-255);
if(shu.x>=0)
if(shu.x<SCRW)
if(shu.y>=0)
if(shu.y<SCRH)
if(*(scrf+shu.y*SCRW+shu.x)==0)
{*(scrf+shu.y*SCRW+shu.x)=0xffff;}
}
else
{rects a;wxy *obj=qh.now;memcpy(&a,&obj->x,sizeof(rects));
colorst b;memset(&b,255,3);
int16 i=a.w<0?0:1,i1=a.h<0?0:1;
dline(a.x,a.y-i1,a.x+a.w-i,a.y-i1,b.r,b.g,b.b);
dline(a.x,a.y+a.h,a.x+a.w-i,a.y+a.h,b.r,b.g,b.b);
dline(a.x-i,a.y,a.x-i,a.y+a.h-i1,b.r,b.g,b.b);
dline(a.x+a.w,a.y,a.x+a.w,a.y+a.h-i1,b.r,b.g,b.b);
char *dt=malloc(20);int W,H;
b.g/=2;rects *d=pdrect(&obj->x);memcpy(&a,d,sizeof(rects));free(d);
printwh(dt,a.x,&W,&H);
dtext(dt,scrfx(a.x,0),scrfx(a.y-H,1),b.r,b.g,b.b,0,0);
printwh(dt,a.y,&W,&H);
dtext(dt,scrfx(a.x-W,0),scrfx(a.y,1),b.r,b.b,b.g,0,0);
printwh(dt,a.w,&W,&H);
dtext(dt,scrfx(a.x+a.w,0),scrfx(a.y+a.h-H,1),b.r,b.g,b.b,0,0);
printwh(dt,a.h,&W,&H);
dtext(dt,scrfx(a.x+a.w-W,0),scrfx(a.y+a.h,1),b.r,b.g,b.b,0,0);
free(dt);}
}

void printwh(char *dt,int nn,int *w,int *h)
{sprintf(dt,"%d",nn);
textwh(dt,0,0,w,h);}
void bmp()
{int f=open("易ui/cache.scr",1);read(f,scrf,SCRW*SCRH*2);close(f);}
void loadtu()
{int f=open("易ui/cache.scr",12);write(f,scrf,SCRW*SCRH*2);close(f);}
void zuobiao()
{
char *zb=malloc(64);
sprintf(zb,"x=%d,y=%d",shu.x,shu.y);
int W,H;
textwh(zb,0,0,&W,&H);
effsetcon(0,0,W,H,180,180,180);
dtext(zb,0,0,220,220,220,0,0);
free(zb);
}

void loadzu()
{
wxy *obj;
obj=malloc(sizeof(wxy));
tongbu(obj,1);
wxy *bf=qh.last;
if(bf!=NULL)
{
qh.dge++;
wxy *a=qh.now,*b=a->point.hou;
obj->point.qian=a;
a->point.hou=obj;
if(qh.now!=qh.last){obj->point.hou=b;
b->point.qian=obj;}
else{qh.last=obj;obj->point.hou=NULL;}qh.ge++;
qh.now=obj;}
else
{qh.now=qh.first=qh.last=obj;
qh.dge=qh.ge=1;}
tongbu(obj,0);
qh.now=obj;}
void btu(int *i)
{drect(0,*i,SCRW,1,70,70,70);
drect(*i,0,1,SCRH,70,70,70);
*i+=20;}
void refzu()
{tishi("请稍后",0);
char *name="易ui/pg.bmp";
int len=SCRW*SCRH*2,f=open(name,1);
if(f)read(f,getscrbuf(),len);
else
{cls(25,25,25);
for(int i;i<SCRH;btu(&i));
f=open(name,12);
write(f,getscrbuf(),len);}
close(f);
if(qh.last!=NULL)
{
wxy *a=qh.first;
while(a!=NULL)
{if(now()==a){if(j!=1)mo(a);}else mo(a);
a=a->point.hou;
}
}
loadtu();
}

int qhzu(int nn)
{if(qh.last!=NULL)
{wxy *a=qh.now,*b=nn?a->point.hou:a->point.qian;
if(b!=NULL)
{qh.now=b;qh.dge+=nn?1:-1;}
}else
return -1;
return 0;}

void delzu(wxy *obj)
{if(qh.last==NULL||obj==NULL)return;
wxy *a=obj->point.qian,*b=obj->point.hou;
if(obj==qh.last){if(obj==qh.first){qh.now=qh.last=qh.first=NULL;qh.ge=qh.dge=0;}else{qh.now=qh.last=obj->point.qian;a->point.hou=NULL;qh.ge--;qh.dge=qh.ge;}}
else
{
if(obj!=qh.first){a->point.hou=obj->point.hou;b->point.qian=obj->point.qian;}else{b->point.qian=NULL;qh.first=b;}
qh.now=b;
qh.ge--;}
free(obj);}

void tongbu(wxy *obj,uint8 nn)
{if(nn){memset(obj,0,sizeof(wxy));
if(nn==1)
{obj->leixing=leixing;
obj->x=shu.x;
obj->y=shu.y;
obj->w=shu.w;
obj->h=shu.h;
obj->text=NULL;
obj->font=font;}
else
{obj->leixing=cdy;
obj->w=obj->h=th;
obj->text=NULL;}
obj->mode=mode;
rgbss(&obj->rgb,&rgb);
rgbss(&obj->RGB,&RGB);
}
else
{shu.x=obj->x;shu.y=obj->y;shu.w=obj->w;shu.h=obj->h;mode=obj->mode;}
}

void rgbss(void *a,void *b)
{memcpy(a,b,sizeof(color16));}

void *pdrect(void *b)
{rects *a=malloc(sizeof(rects));memcpy(a,b,sizeof(rects));
if(a->w<0){a->x+=a->w;a->w=-a->w;}
if(a->h<0){a->y+=a->h;a->h=-a->h;}
return a;}

int scrfx(int a,int b)
{if(a<0)return 0;
else{if(a>(b?SCRH-30:SCRW-30)){int W,H;char *dt=malloc(15);sprintf(dt,"%d",a);textwh(dt,0,0,&W,&H);free(dt);
return b?SCRH-H:SCRW-W;}else return a;}
}

wxy *now()
{return qh.now;};

void tishi(char *a,int n)
{int W,H;textwh(a,n,0,&W,&H);
int X=(SCRW-W)/2,Y=(SCRH-H)/2;
effsetcon(X,Y,W,H,150,150,150);
dtext(a,X,Y,200,200,200,n,0);
ref(X,Y,W,H);
}
int rgb16(int16 r0,int16 g0,int16 b0)
{return (r0>>3<<11)+(g0>>2<<5)+(b0>>3);}

void rgb24(uint16 n,colorst *r)
{uint8 m;
m=n>>8&0xf8;
r->r=m|(m>>5);
m=n>>3&0xfe;
r->g=m|(m>>6);
m=n<<3&0xf8;
r->b=m|(m>>5);}

void pdcolor(int16 *a)
{*a=*a>255?-255:(*a<-255?255:*a);}

int bjietu()
{int a=j;j=3;
xian(0);j=a;
return printscr2("c/易ui/我的UI.bmp",0,0,SCRW,SCRH);}

int lishi()
{
wxy *obj=qh.first;
if(obj==NULL)return -1;
remove(lsmain);remove(lstext);
int f=open(lsmain,12),f1=open(lstext,12);
if(!f|!f1){close(f);close(f1);return -1;}
int len=sizeof(wxy),tlen;
char *text;void *a;
while(obj!=NULL)
{text=obj->text;tlen=0;
if(text!=NULL){if(obj->mode%2)tlen=wstrlen(text)+2;else tlen=strlen(text)+2;write(f1,text,tlen);}
obj->text=(char *)tlen;
write(f,obj,len);
obj->text=text;
obj=obj->point.hou;}
close(f);close(f1);
return 0;}

int loadlishi()
{int s=sizeof(wxy),len=getlen(lsmain)/s,f=open(lsmain,1),f1;
if(!f||len<=0)return -1;
long tlen;
for(;qh.now!=NULL;delzu(now()));
f1=open(lstext,1);
char *text;
wxy *obj=malloc(s),*o;
for(int i;i<len;i++)
{read(f,obj,s);
text=obj->text;
loadzu();
o=qh.now;
if(text!=NULL){tlen=(int)text;o->text=malloc(tlen);read(f1,o->text,tlen);}
memcpy(&o->x,&obj->x,24);}
free(obj);
close(f);close(f1);
refzu();
return 0;}