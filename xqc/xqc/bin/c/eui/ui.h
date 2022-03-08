
uint8 *zdyy=malloc(20);
memset(zdyy,0,20);
void mo(wxy *b)
{rects *a=pdrect(&b->x);
switch(b->leixing)
{case 1:
b->w=b->h=1;
dpointex(b->x,b->y,b->rgb.r,b->rgb.g,b->rgb.b);
break;
case 2:
dline(b->x,b->y,b->x+b->w,b->y+b->h,b->rgb.r,b->rgb.g,b->rgb.b);
break;
case 3:
b->x=b->y=b->w=b->h=0;
cls(b->rgb.r,b->rgb.g,b->rgb.b);
break;
case 4:
drect(a->x,a->y,a->w,a->h,b->rgb.r,b->rgb.g,b->rgb.b);
break;
case 5:
effsetcon(a->x,a->y,a->w,a->h,b->rgb.r,b->rgb.g,b->rgb.b);
break;
case 6:
int p,q;color16 *m=&b->rgb;p=m->r*0x10000+m->g*0x100+m->b;m=&b->RGB;q=m->r*0x10000+m->g*0x100+m->b;
shaderect(a->x,a->y,a->w,a->h,p,q,b->mode);
break;
case 7:
int W,H;
textwh(b->text,b->mode,b->font,&W,&H);
b->w=W;b->h=H;
dtext(b->text,b->x,b->y,b->rgb.r,b->rgb.g,b->rgb.b,b->mode,b->font);
break;
case 8:
drawtxt2(b->text,a->x,a->y,a->w,a->h,b->rgb.r,b->rgb.g,b->rgb.b,b->mode,b->font);
break;
case 9:
dsanjiao(b->x,b->y,b->w,b->h,b->rgb.r,b->rgb.g,b->rgb.b,b->mode);
break;
case 10:
b->w=b->h;
dyuan(a->x,a->y,a->w,b->rgb.r,b->rgb.g,b->rgb.b,b->mode);break;
case 11:
bmp565(b->text,a->x,a->y,a->w,a->h,b->mode);
break;
case 12:b->w=b->h=0;if(getlen(b->text))break;img(b->text,b->x,b->y);
break;
}free(a);
}
void dsanjiao(int16 xx,int16 yy,int16 ww,int16 hh,uint8 rr,uint8 gg,uint8 bb,uint8 nn)
{hh--;
if(!nn){
dline(xx+ww/2,yy,xx,yy+hh,rr,gg,bb);
dline(xx+ww/2,yy,xx+ww,yy+hh,rr,gg,bb);
dline(xx,yy+hh,xx+ww,yy+hh,rr,gg,bb);}
else{
if(ww<0){xx+=ww;ww=-ww;}
for(int a=0;a<ww;a++)
dline(xx+ww/2,yy,xx+a,yy+hh,rr,gg,bb);}
}
void dyuan(int16 xx,int16 yy,int16 hh,uint8 rr,uint8 gg,uint8 bb,uint8 nn)
{
int r0=hh/2,x1=-1,y1=r0,g1,d1,d2,e1,f1,a,b1;
uint16 t=rgb16(rr,gg,bb);
while(x1<=y1)
{
x1++;g1=y1-1;
f1=x1*x1-r0*r0;
d1=y1*y1+f1;d2=g1*g1+f1;
e1=d1*d1;
f1=d2*d2;
if(e1>f1)
y1--;
a=xx+r0;b1=yy+r0;
if(!nn){dpoint(x1+a,y1+b1,t);
dpoint(a-x1,b1-y1,t);
dpoint(x1+a,b1-y1,t);
dpoint(a-x1,y1+b1,t);
dpoint(y1+a,x1+b1,t);
dpoint(a-y1,b1-x1,t);
dpoint(y1+a,b1-x1,t);dpoint(a-y1,x1+b1,t);}
else{
dline(x1+a,y1+b1,a+x1,b1-y1,rr,gg,bb);
dline(a-x1,b1-y1,a-x1,y1+b1,rr,gg,bb);
dline(y1+a,x1+b1,a+y1,b1-x1,rr,gg,bb);
dline(a-y1,b1-x1,a-y1,x1+b1,rr,gg,bb);
}}
}
void dsrect(int16 xx,int16 yy,int16 ww,int16 hh,uint8 rr,uint8 gg,uint8 bb,uint8 r1,uint8 g1,uint8 b1,uint8 nn)
{
int p=rr*0x10000+gg*0x100+bb,q=r1*0x10000+g1*0x100+b1;
shaderect(xx,yy,ww,hh,p,q,nn);}
void bmp565(char *name,int x,int y,int w,int h2,int nn)
{int len=getlen(name);
if(len<0||!w||!h2)return;
int h;
if(h2){h=len/2/w;if(h2<h)h=h2;if(!h)h=1;}
len=w*h*2>len?len:w*h*2;
int f=open(name,1);
if(!f){return;}
uint16 *p=malloc(len);
if(p==NULL){close(f);return;}
read(f,p,len);
close(f);

bmpshowflip(p,x,y,w,w,h,nn,0,0,*p);
free(p);}

#define dou ","

int baocun()
{wxy *obj=qh.first;
if(obj==NULL)return -1;
char *name="c/Ò×ui/ÎÒµÄUI.c";
remove(name);
int f=open(name,12);
if(!f)return -1;
char *a;
int len;
writes(f,lang(2,21));
writes(f,lang(2,22));
while(obj!=NULL)
{a=lang(2,obj->leixing);
if(obj->leixing==2){obj->w+=obj->x;obj->h+=obj->y;}
len=-(strlen(strstr(a,"="))-strlen(a));
char *text=malloc(len+2);
memset(text,0,len+2);
memcpy(text,a,len);
writes(f,text);free(text);
canshu(f,a+len+1,obj);
if(obj->leixing==2){obj->w-=obj->x;obj->h-=obj->y;}
zdyy[obj->leixing]=1;
obj=obj->point.hou;
}
for(len=23;len<26;writes(f,lang(2,len++)));
for(len=0;len<20;len++)
{if(zdyy[len])
switch(len)
{case 6:writes(f,lang(2,15));break;
case 8:case 9:writes(f,lang(2,8+len));break;
case 10:writes(f,lang(2,18));writes(f,lang(2,19));writes(f,lang(2,20));break;
case 11:writes(f,lang(2,13));writes(f,lang(2,14));break;}
}
memset(zdyy,0,20);
return close(f);
}

void canshu(int f,char *a,wxy *obj)
{uint8 l=*"0";char *m=malloc(50);
int len=strlen(a);
writes(f,"(");
rects *A;
if((obj->leixing==2)|(obj->leixing==9)){A=malloc(sizeof(rects));memcpy(A,&obj->x,sizeof(rects));}
else{A=pdrect(&obj->x);}
int ww=A->w,hh=A->h;
for(int i,o=0;i<len;i++)
{if(a[i]-l==1)
{
if(i&&o)
{writes(f,dou);}
o++;
switch(i)
{case 0:write(f,"\x22",1);if(obj->mode%2){char *dt=unidt(obj->text);writes(f,dt);free(dt);}else{writes(f,obj->text);}write(f,"\x22",1);break;
case 1:sprintf(m,"%d,%d",obj->x,obj->y);writes(f,m);break;
case 2:sprintf(m,"%d,%d",ww,hh);writes(f,m);break;
case 3:sprintf(m,"%d,%d,%d",obj->rgb.r,obj->rgb.g,obj->rgb.b);writes(f,m);break;
case 4:sprintf(m,"%d,%d,%d",obj->RGB.r,obj->RGB.g,obj->RGB.b);writes(f,m);break;
case 5:sprintf(m,"%d",obj->mode);writes(f,m);break;
case 6:sprintf(m,"%d",obj->font);writes(f,m);break;}
}
free(A);}
writes(f,");\n");
free(m);
}
void writes(int f,char *a)
{write(f,a,strlen(a));}

char *unidt(char *text)
{int len=wstrlen(text)+3;
char *dt=malloc(len*4);
memset(dt,0,len*4);
char m[6];
for(int i=0;i<len-1;i++)
{sprintf(m,"\\x%02x",text[i]);
memcpy(dt+i*4,m,4);}
return dt;}
