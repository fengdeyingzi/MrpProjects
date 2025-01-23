/*
  按钮控件 v1.1
　by：风的影子

  更新记录：
  细节修改
  修复点击事件event错误
  
  
影子俱乐部网站：www.yzjlb.net
手机C论坛：bbs.yzjlb.net
手机C交流QQ群：370468001
*/

//按钮结构体

typedef struct 
{
 char *name; //控件名字
int id; //按钮id
int type;//按钮状态，按下1，释放0，隐藏-1
int isshow; //是否显示 不显示时将不会执行点击事件
int isdown; //是否按下
char *text; //按钮文字
int textx; //文字x坐标
int texty; //文字y坐标
int x; //按钮位置x
int y; //按钮位置y
int w; //按钮宽度
int h; //按钮高度
int timer; //回调定时器
int draw_mode; //刷新模式
char *onclick; //函数回调
}button;
int id=0;

//新增按钮
//参数分别是 按钮文字 按钮x坐标 y坐标 宽度 高度 
void* btn_create(char *text,int x,int y,int w,int h)
{
int fw,fh;
 button *btn;
btn=malloc(sizeof(button));
memset(btn,0,sizeof(button));
btn->name="Button";
btn->text=text;
btn->x=x; 
btn->y=y;
btn->w=w;
btn->h=h;
btn->type=0;
btn->id=-1;id++;
textwh(text,0,1,&fw,&fh);
btn->textx=x+(w-fw)/2;
btn->texty=y+(h-fh)/2;
btn->timer = timercreate();
btn->isdown=0;
btn->isshow = 1;
return btn;
}

//设置按钮id
void btn_setid(button *btn,int id)
{
 btn->id=id;
}

//获取按钮id
int btn_getid(button *btn)
{
 return btn->id;
}

//显示按钮 参数：按钮结构体指针
void btn_draw(button *btn)
{
 if(btn->isshow)
 {
  if(btn->isdown)
  {
   //绘制一个矩形
drect(btn->x,btn->y,btn->w,btn->h,68,168,240);
drect(btn->x+4,btn->y+4,btn->w-8,btn->h-8,230,238,245);
  //绘制文字
dtext(btn->text,btn->textx,btn->texty,50,50,50,0,1);
  }
  else
  {
      //绘制一个矩形
drect(btn->x,btn->y,btn->w,btn->h,168,168,168);
drect(btn->x+4,btn->y+4,btn->w-8,btn->h-8,230,230,230);
  //绘制文字
dtext(btn->text,btn->textx,btn->texty,50,50,50,0,1);
  }
 }
}

//设置按钮文字 参数：按钮结构体指针 新的按钮文字
void btn_settext(button *btn,char *text)
{
btn->text=text;
button_draw(btn);
ref(btn->x,btn->y,btn->w,btn->h);
}

//设置定时器刷新模式 0非定时器刷新 1定时器刷新
void btn_setref(button *btn,int mode)
{
 btn->draw_mode=mode;
}


//显示按钮
int btn_show(button *btn)
{
 btn->isshow=1;
 btn_draw(btn);
 return 0;
}

//隐藏按钮
int btn_hide(button *btn)
{
 btn->isshow = 0;
 btn->isdown = 0;
 
 return 0;
}

//按钮是否显示
int btn_isshow(button *btn)
{
 return btn->isshow;
 
}

//设置点击回调函数 由于手机C不支持函数指针，所以采用char*，函数原型为定时器的回调函数void click_ok(int id)
void btn_setonclick(button *btn, char *click)
{
 btn->onclick = click;
}

//按钮event事件
void btn_event(button *btn,int type,int p1,int p2)
{
 
 if(type==MS_DOWN)
 {
 if(btn_impact(btn,p1,p2))
 {
  btn->isdown=1;
  if(btn->draw_mode == 0)
  {
  btn_draw(btn);
  ref(btn->x,btn->y,btn->w,btn->h);
  }
  //printf("down");
 }
 }
 if(type==MS_UP)
 {
  //if(btn_impact(btn,p1,p2))
  {
   if(btn->isdown)
   {
    btn->isdown=0;
    if(btn->draw_mode == 0)
    {
    btn_draw(btn);
    ref(btn->x,btn->y,btn->w,btn->h);
    }
    if(btn->onclick!=NULL)
    {
    // printf("click");
    
     timerstart(btn->timer, 10, btn->id, btn->onclick, 0);
    }
   }
  }
 }
 
}


//判断按钮点击事件 参数：按钮指针 触屏x坐标 触屏y坐标
//返回值：TRUE(1)成功 FALSE(0)失败
int btn_impact(button *btn,int x,int y)
{
 if(btn->isshow)
 {
  if(x>btn->x && x<btn->x+btn->w && y>btn->y && y<btn->y+btn->h)
  return 1;
 }

return 0;
}

//释放按钮内存
void btn_free(button *btn)
{
 free(btn);
}


