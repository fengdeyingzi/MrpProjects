#ifndef _MRC_GUI_H_
#define _MRC_GUI_H_

int snx,n,cdy=1,cdge;
uint16 *colortu=NULL,colorj;
char **cddt=malloc(50*sizeof(char**));
tcolor ts;

void bangzhu()
{
beijing(0,0,SCRW,SCRH);
drawtxt2(lang(0,2),10,th+10,SCRW-20,SCRH,50,120,50,3,0);
ref(0,th,SCRW,SCRH-th*2);
cdt(0,"帮助",NULL);
cdl(0,NULL,NULL,"返回");
}
void guanyu()
{
cdt(0,"关于",NULL);
beijing(0,0,SCRW,SCRH);
effsetcon(0,th,SCRW,SCRH-th*2,-220,-220,-220);
effsetcon(0,th,SCRW,SCRH-th*2,-220,-220,-220);
effsetcon(0,th,SCRW,SCRH-th*2,180,180,180);

drawtxt2(lang(0,1),5,th+5,SCRW,SCRH,124,252,0,3,0);
cdl(0,"访问",NULL,"返回");
int xx;
for(xx=0;xx<SCRW;xx+=2)
{
ref(xx,th,1,SCRH-th*2);
ref(SCRW-xx-1,th,1,SCRH-th*2);
}
}
void cdt(int xx,char *a,char *a1)
{
beijing(xx,0,SCRW,th);
effsetcon(xx,0,SCRW,th,200,200,200);
dtext(a,5+xx,0,0,0,0,0,0);
dtext(a1,SCRW-strw(a1,0)+xx,0,200,200,200,0,0);
if(!xx){ref(0,0,SCRW-xx,th);}
}


void tuichu()
{
effsetcon(0,0,SCRW,SCRH-th,200,200,200);
effsetcon(2,SCRH-th*4,SCRW-4,th*3,150,150,150);
dtext(lang(0,3),SCRW/2-tw*2,SCRH-th*4,100,200,100,1,0);
dline(5,SCRH-th*3-1,SCRW-5,SCRH-th*3-1,100,100,100);
drawtxt2(lang(0,4),5,SCRH-th*3,SCRW-10,th*3,200,100,200,3,0);
cdl(0,"退出",NULL,"返回");
re();
}

void beijing(int16 xx,int16 yy,uint16 ww,uint16 hh)
{drect(xx,yy,ww,hh,120,200,150);}

void caidan(int cn)
{
if(cn%2==0){goto tiao;}
beijing(0,0,SCRW,SCRH);
char *a,*fens="/";
switch(c)
{case 200:a=lang(1,1);
break;
case 210:a=lang(1,2);break;
case 216:a=lang(1,3);break;
case 240:a=lang(1,9);break;
case 250:a=lang(1,10);break;
case 217:case 218:case 219:case 220:case 221:a=lang(1,c-213);break;
case 230:a=lang(1,11);break;
case 251:a=lang(1,12);break;
}
if(a!=NULL){
cdy=1;
int len=strlen(a),alen=0,blen=0;
char *aaa,*bbb,*ccc=malloc(200);
cdge=0;
while(cdge<len)
{
aaa=strstr(a+alen,fens);
if(aaa==NULL)break;
bbb=strstr(aaa+1,fens);
if(bbb==NULL)break;
blen=strlen(aaa+1)-strlen(bbb);
alen+=blen+1;memset(ccc,0,50);
sprintf(cddt[cdge],"%d.%s",cdge+1,memcpy(ccc,aaa+1,blen>40?40:blen));
cdge++;}
free(ccc);

cdl(0,"确定",NULL,"返回");
if(cn==3)return;}
tiao:
cdy=cdy>cdge?1:(cdy<1?cdge:cdy);
int ju=22,xx=20,yy=(SCRH-cdge*ju)/2,ww,hh;uint8 i;

for(i=0;i<cdge;i++)
{
beijing(0,yy+ju*i-2,SCRW,th+4);
if(cdy==i+1){textwh(cddt[i],0,0,&ww,&hh);
drect(xx,yy+ju*i-2,ww+10,hh+4,25,120,200);
if(c>=210&&c<230)cdtu(xx+ww+15,yy+i*ju);}
dtext(cddt[i],xx+(cdy==i+1?5:0),yy+ju*i,180,cdy==i+1?250:50,200,0,0);
}
if(cn==1||cn==2)re();
else
ref(0,yy-2,SCRW,4+ju*cdge);
}
void cdtu(int16 xx,int16 yy)
{wxy *obj=malloc(sizeof(wxy));tongbu(obj,2);obj->x=xx;obj->y=yy;
if(c==210){if(cdy==3)obj->leixing=1+cdy;}
else{obj->leixing=leixing;obj->mode=cdy-1;}
mo(obj);free(obj);
}

void re()
{
ref(0,0,SCRW,SCRH);
}
int strw(char *dts,int fonts)
{
int ww,hh;
textwh(dts,0,fonts,&ww,&hh);
return ww;
}



int printscr(char *name)
{
uint16 *scrf=getscrbuf();
int ff=open(name,12);
write(ff,scrf,SCRW*SCRH*2);
return close(ff);
}

void cdl(int xx,char *cdl1,char *cdl2,char *cdl3)
{
effsetcon(xx,SCRH-th,SCRW,20,180,180,180);
dtext(cdl1,xx,SCRH-th,200,200,200,0,0);
dtext(cdl2,SCRW/2-strw(cdl2,0)/2+xx,SCRH-th,200,500,200,0,0);
dtext(cdl3,SCRW-strw(cdl3,0)+xx,SCRH-th,200,200,200,0,0);
ref(0,SCRH-th,SCRW-xx,th);
}

char *uc2(char *dts,int nn)
{
char *dts2;int ll;
if(nn)
{dts2=(void *)c2u(dts,NULL,&ll);}
else
{int ll2=wstrlen(dts);dts2=malloc(ll2);
u2c(dts,ll2,&dts2,&ll);}
return *&dts2;
}

void drawtxt2(char *dt,int16 xx,int16 yy,int16 ww,int16 hh,uint8 rr,uint8 gg,uint8 bb,uint16 flad,uint16 fonts)
{
colorst *a=malloc(sizeof(colorst));
a->r=rr;a->g=gg;a->b=bb;
rectst *b=malloc(sizeof(rectst));
b->x=xx;b->y=yy;
b->w=ww;b->h=hh;
dtextex(dt,xx,yy,b,a,flad,fonts);
free(a);free(b);}


void shuru(char *bt,char *dt,uint8 n)
{char *text=dt;
if(!n)text=uc2(dt,1);
dialogf=editcreate(bt==NULL?lang(0,6):bt,text,0,255);
}


void tiaose()
{beijing(0,0,SCRW,SCRH);
int16 xx,yy=50,ww=5,hh=150;int W,H;
void *se=&ts.r;int16 *b=se,a;
char *dt=malloc(15);
for(int8 i=1;i<4;i++)
{dsrect(i*SCRW/4,yy,ww,hh/2,i==1?255:0,i==2?255:0,i==3?255:0,0,0,0,2);
dsrect(i*SCRW/4,yy+hh/2,ww,hh/2,i==1?255:0,i==2?255:0,i==3?255:0,0,0,0,2);
printwh(dt,b[i-1],&W,&H);
tsk(dt,i*SCRW/4+ww/2,yy+hh+5);}
free(dt);a=b[ts.x-1];
dtext("←",ts.x*SCRW/4+ww,yy+hh/2-th/2+(a<0?-(-a)*hh/2/255:a*hh/2/255),0,0,0,0,0);

drect(SCRW/2-25,SCRH-80,50,50,ts.r.r,ts.r.g,ts.r.b);
cdt(0,c==241?"前景色":"背景色","#键真彩色");
cdl(0,"确认","输入","返回");
re();
}
void tiaosebc(uint8 n)
{if(c!=241&&c!=242){return;}
color16 *a,*b;wxy *obj=now();
if(n==1){a=&ts.r;if(c==241)b=j?&obj->rgb:&rgb;
if(c==242)b=j?&obj->RGB:&RGB;
if(!colorj)ts.x=1;}
if(n==2)
{b=&ts.r;
if(c==241)a=j?&obj->rgb:&rgb;
if(c==242)a=j?&obj->RGB:&RGB;
}
memcpy(a,b,sizeof(color16));
}
void tsk(char *dt,int xx,int yy)
{int ww,hh,r=220,g=10,b=200;
textwh(dt,0,0,&ww,&hh);
ww/=2;

dtext(dt,xx-ww,yy+1,r,g,b,0,0);
}
void ztiaose()
{beijing(0,0,SCRW,SCRH);
if(colortu==NULL){tishi("显示失败！",0);colorj=0;tiaose();return;}else{bmpshowflip(colortu,5,25,174,174,255/2,0,0,0,0);}
colorst r;
rgb24(colortu[ts.x+174*ts.y],&r);
drect(ts.x-1,19+ts.y,12,12,255,255,255);
drect(ts.x,20+ts.y,10,10,r.r,r.g,r.b);
drect(SCRW/2-25,SCRH-80,50,50,ts.r.r,ts.r.g,ts.r.b);
char *dt=malloc(256);
sprintf(dt,"r=\r%d\ng=\r%d\nb=\r%d\n16进制:%02x%x%x",r.r,r.g,r.b,r.r,r.g,r.b);
drawtxt2(dt,10,30+255/2,SCRW,200,0,0,0,2,0);
free(dt);
cdt(0,"真彩色","#键标准模式");
cdl(0,"确认","选择","返回");
re();
}

int jzcolor()
{char *name="易ui/color.dat";
if(getlen(name)<0)
{zcolor();if(printscr2(name,0,0,174,255/2)){return -1;}}
if(colortu==NULL){int f=open(name,1),len=255/2*174*2;colortu=malloc(len);read(f,colortu,len);return close(f);}
return 0;}

void zcolor()
{tishi("正在努力绘制，第一次较慢",0);
int i,y=0;
for(i=255;i>0;i-=2)
{
shade(0,y,29,i,0,0,i,i,0);
shade(29,y,29,i,i,0,0,i,0);
shade(58,y,29,0,i,0,0,i,i);
shade(87,y,29,0,i,i,0,0,i);
shade(116,y,29,0,0,i,i,0,i);
shade(145,y,29,i,0,i,i,0,0);
ref(0,y,SCRW,1);
y++;
}
}
void shade(int x,int y,int w,int r1,int g1,int b1,int r2,int g2,int b2)
{
int i=0,r=r2-r1,g=g2-g1,b=b2-b1;
while(i<=w)
{
dpointex(x+i,y,r1+r*i/w,g1+g*i/w,b1+b*i/w);
i++;}
}

int printscr2(char *name,int xx,int yy,int ww,int hh)
{
remove(name);
if((xx+ww)>SCRW){ww=SCRW-xx;if(ww<0){ww=0;return -1;}}
if((yy+hh)>SCRH){hh=SCRH-yy;if(yy<0){yy=0;}}if(hh<0){return -1;}
uint16 *scrf=getscrbuf(),*scrf2=scrf+SCRW*yy+xx;
int ff=open(name,12);
if(!ff){return -1;}
if(!xx&&ww==SCRW)
write(ff,scrf+yy*SCRW,hh*SCRW*2);
else
{
for(int a=yy;a<hh;a++)
{
write(ff,scrf2,ww*2);
scrf2+=SCRW;
}
}
return close(ff);
}

