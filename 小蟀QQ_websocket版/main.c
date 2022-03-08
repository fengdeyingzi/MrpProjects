//#include "base.h"
#include "mpc.h"
#include "xl_coding.h"
#include "http/httpclient.h"
#include "cJSON.h"
//#include "theme.h"
//#include "draw.h"
//#include "button.h"
//#include "menu.h"
//#include "chat.h"

//#include "act_manager.h"

//#include "main_act.h"
//#include "about_act.h"
//#include "login_act.h"

/*
软件主题
设置主题色等配置，按钮等控件默认使用此配置
*/
//#include "base.h"

mr_datetime time;
int32 wsocket;

typedef enum _STA {
	STA_NORMAL = 0,
	STA_HIGH,
	STA_MAIN,
	STA_HIDE
} STA;

typedef struct theme {
	int32 cls_text;		//普通文字
	int32 cls_text_high;	//高亮文字
	int32 cls_main;		//主配色
	int32 cls_high;		//高亮配色
	int32 cls_normal;	//默认颜色
	int32 cls_bkg;		//背景色
	int8 font_size;		//文本大小
} Theme;

Theme theme_normal;
Theme *theme_current;

#define CLS_TEXT 0x000000
#define CLS_MAIN 0x62a0f4
#define CLS_HIGH 0x75c0f0
#define CLS_NORMAL 0xA0A0A0
#define CLS_BKG  0xf0f0f0

//资源管理器
//管理字符串，图片等需要释放的资源的
typedef enum resType {
	tImg = 0,
	tSnd,
	tTim,
	tStr
} ResType;

//资源
typedef struct res {
	uint16* id;
	char *path;
	ResType type;
} Res;

//资源链
struct {
	Res res[300];
	int num;
} ResList;

Res *getRes(char *path, ResType type);
extern void main_act_call(int32 data);
extern void drawTextUTF(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
void drawTextUTFX(char *pcText, int16 x, int16 y, rectst *rt,colorst *co, int is_unicode, uint16 font);
//普通控件，只是图块
typedef struct _View {
	int32 x;
	int32 y;
	int32 w;
	int32 h;
	int id;
	STA sta;
} View;

//文本框，显示文本，u8编码
typedef struct _TextView {
	View view;
	char *text;
	int8 single;		//是否单行
} TextView;

//文本大小
typedef struct _Size {
	int w;
	int h;
} Size;


//文本按钮，显示文本
//文本内存需用户自己管理
typedef struct _TextBtn {
	View view;
	int32 timer;
	mrc_timerCB call;
	char *text;
} TextBtn;


#define MR_SUCCESS  0		//成功
#define MR_FAILED   -1		//失败
#define MR_IGNORE   1		//不关心
#define MR_WAITING  2		//异步(非阻塞)模式

#define  MR_SOCK_STREAM 0	//数据流
#define  MR_SOCK_DGRAM  1	//数据包
#define  MR_IPPROTO_TCP  0	//TCP
#define  MR_IPPROTO_UDP  1	//UDP

#define  MR_SOCKET_BLOCK   0	//阻塞方式（同步方式）
#define  MR_SOCKET_NONBLOCK   1	//非阻塞方式（异步方式）

#define BUFLEN 1024*4

typedef struct _SOC {
	int32 soc;
	int32 con_timer;	//连接timer

	//回调函数
	mrc_timerCB call;		//连接回调
	mrc_timerCB recv;		//接收数据
	mrc_timerCB error;		//错误回调

	int32 use_time;		//超时时间

	char *buff;
} SOC;


typedef struct _Link {
	void *node;

	struct _Link *prev;
	struct _Link *next;
} Link;

typedef struct _LinkD {
	Link *head;
} LinkD;

//为新的链表申请内存
#define LinkAlloc malloc(sizeof(Link))


//消息列表
LinkD msgLink;



typedef enum _ACT_PAGE {
	MAIN_ACT = 0,
	LOGIN_ACT,
	ABOUT_ACT
} ACT_PAGE;

//页面管理器
typedef struct _ACT {
	mrc_timerCB call;
	char *name;
	int32 id;
} ACT;


ACT main_act;
TextBtn mainBtn[3];
//下方三个菜单
TextBtn runBtn, zuBtn;

char *send_msg = NULL;
int32 main_edit ;

ACT login_act;
TextBtn loginBtn[5];
#define LOGIN_BTN_NUM 3

char *login_name = NULL;
// char *login_name1 = NULL;
int32 login_edit;


//输入框，输入文本
typedef struct _EditText {
	View view;
	char *text;
	int8 single;		//是否单行
} EditText;


//标题栏，仅显示无响应事件
//字符内存由使用者保存
typedef struct _title {
	View view;
	char *title;
	char *info;
} Title;

//下方菜单，响应左右软键和中键
//字符内存由使用者保存
typedef struct _Menu {
	View view;
	char *left;
	char *center;
	char *right;
} Menu;


//标题、当前标题
Title _title;
Title *current_title;


//图片框，需手动调用freeImageView
typedef struct _ImageView {
	View view;
	char *path;
	uint16* image;
	int32 iw;
	int32 ih;
} ImageView;



ImageView logo;

ACT about_act;
TextView about;
ImageView about_logo;



ACT *current_act;
int32 act_timer;


char *url;
SOC chat;

int32* filelen;


void *readBitmapFromAssets(char *filename)
{
	int32 len=0;
	return mrc_readFileFromMrp((const char*)filename, &len, 0);
}

void* readBitmap(char *filename)
{
	int32 f;
	void *buf;
	int32 len = 0;

	len = mrc_getLen(filename);
	if(len>0)
	{	
		f= mrc_open(filename,1);

		buf = mrc_malloc(len);
		if(buf==NULL)
		{

		}
		if(f>0)
		{	
			mrc_read(f, buf, len);
			mrc_close(f);
		}	
		//		mrc_free(buf);
		return buf;
	}
	return NULL;
}

void drawBitmapEx(uint16* bmp,int32 x,int32 y,int32 w,int32 h,int32 tx,int32 ty,int32 tw,int32 th)
{
	bmpshowflip(bmp,x,y,tw,tw,th,BM_COPY,tx,ty,0);
}

void bitmapFree(uint16* buf)
{
	if(buf)
		mrc_free(buf);
}


//删除节点
void linkDelLink(Link * temp)
{
	Link *prev = temp->prev;
	Link *next = temp->next;
	if (prev != NULL)
		prev->next = next;
	if (next != NULL)
		next->prev = prev;
	free(temp);
}



//获取结尾
Link *linkGetEnd(Link * head)
{
	Link *temp = head;
	while (temp->next != NULL)
		temp = temp->next;
	return temp;
}

//获取开头
Link *linkGetHead(Link * head)
{
	Link *temp = head;
	while (temp->prev != NULL)
		temp = temp->prev;
	return temp;
}


//后面添加Node
Link *linkAddNode(Link * head, void *node)
{
	Link *temp;
	Link *next;
	printf("linkAddNode\n");
	temp = linkGetEnd(head);
	if (temp->node != NULL) {
		next = LinkAlloc;
		next->node = node;
		next->prev = temp;
		next->next = NULL;
		temp->next = next;
		return next;
	} else {
		temp->node = node;
		return temp;
	}
}


//节点获取链接
Link *nodeGetLink(Link * link, void *node)
{
	Link *temp = linkGetHead(link);
	while (temp->node != node) {
		temp = temp->next;
		if (temp == NULL)
			break;
	}
	return temp;
}

//删除节点
void linkDelNode(Link * link, void *node)
{
	Link *temp = nodeGetLink(link, node);
	linkDelLink(temp);
}


void themeInit()
{
	theme_normal.cls_text = 0x000000;
	theme_normal.cls_text_high = 0xffffff;
	theme_normal.cls_main = 0x62a0f4;
	theme_normal.cls_high = 0x75c0f0;
	theme_normal.cls_normal = 0xC0C0C0;
	theme_normal.cls_bkg = 0xf0f0f0;
	theme_normal.font_size = 0;
}

int8 getThemeFont()
{
	return theme_current->font_size;
}

Theme *getTheme()
{
	return theme_current;
}

int32 getStaCls(STA sta)
{
	int32 rgb = theme_current->cls_normal;
	rgb = sta == STA_MAIN ? theme_current->cls_main : rgb;
	rgb = sta == STA_HIGH ? theme_current->cls_high : rgb;
	return rgb;
}

int8 getClsR(int32 argb)
{
	return argb >> 16 & 0xff;
}

int8 getClsG(int32 argb)
{
	return argb >> 8 & 0xff;
}

int8 getClsB(int32 argb)
{
	return argb >> 0 & 0xff;
}

void getClsSt(int32 argb, colorst * color)
{
	color->r = getClsR(argb);
	color->g = getClsG(argb);
	color->b = getClsB(argb);
}


//载入资源
Res *loadRes(char *path, ResType type)
{
	char *text = NULL;
	Res *res;
	printf("loadRes%d：%s:%s\n", ResList.num - 1, resTypeStr[type], path);


	res = &(ResList.res[ResList.num - 1]);
	switch (type) {
		//图片
case tImg:
	res->type = type;
	res->path = getRes(path, tStr)->path;
	if (res->path[0] == '/')
		res->id = readBitmapFromAssets(path + 1);
	else
		res->id = readBitmapFromAssets(path);
	if (res->id == NULL) {
		//读取sd卡图片
		res->id = readBitmap(path);
		if (res->id == NULL)
			return NULL;
	}
	break;

	//定时器
case tTim:
	res->id = (uint16*)timercreate();
	res->type = type;
	res->path = getRes(path, tStr)->path;
	break;

	//字符串
case tStr:
	text = malloc(sizeof(char) * strlen(path) + sizeof(char));
	memset(text, '\0', strlen(path) + sizeof(char));
	strcpy(text, path);
	res->type = type;
	strcpy(text, path);
	res->path = text;
	res->id = (uint16*) text;
	break;

default:
	break;
	}
	return res;
}


//获取资源
Res *getRes(char *path, ResType type)
{
	Res *res = NULL;
	int i;
	//遍历资源
	if (ResList.num > 0)
		for (i = 0; i < ResList.num; i++) {
			res = &(ResList.res[i]);
			//资源已载入
			if (res->type == type)
				if (strcmp(path, res->path) == 0) {
					return res;
					break;
				}
		}
		//资源未载入
		ResList.num++;
		res = loadRes(path, type);
		if (res == NULL) {
			ResList.num--;
			exit();
		}
		//返回资源
		return res;
}


void freeOneRes(Res * res, ResType type)
{
	Res *res2 = NULL;
	int i,j;
	//遍历资源
	if (ResList.num > 0)
		for (i = 0; i < ResList.num; i++) {
			res2 = &(ResList.res[i]);
			//资源已载入
			if (res->type == type)
				if (strcmp(res->path, res2->path) == 0) {
					printf("释放资源%d:%s\n", i,
						res2->path);
					switch (res2->type) {
						//图片
case tImg:
	bitmapFree(res2->id);
	break;

	//定时器
case tTim:
	timerstop((int32)(res2->id));
	timerdel((int32)(res2->id));
	break;

case tStr:
	return;
	break;

default:
	break;
					}

					for (j = i;
						j < ResList.num - 1; j++) {
							ResList.res[j] =
								ResList.res[j + 1];
					}
					ResList.num--;
					return;
					break;
				}
		}
		//资源未载入
		printf("释放资源失败：%s\n", res2->path);
		//返回资源
		return;
}

//释放资源资源
void freeRes()
{
	//遍历Res
	Res *res;
	int i;
	for (i = 0; i < ResList.num; i++) {
		res = &(ResList.res[i]);
		printf("freeRes%d\n", i);
		switch (res->type) {
			//图片
case tImg:
	bitmapFree(res->id);
	break;

	//定时器
case tTim:
	timerstop((int32)(res->id));
	timerdel((int32)(res->id));
	break;

case tStr:
	free(res->path);
	break;

default:
	break;
		}
	}
}



//获取当前字体大小
void getFontSize(char *str, Size * size)
{
	char *text = ex_coding(str, strlen(str), "utf-8", "UNICODE");
	textwh(text, TRUE, theme_current->font_size, &(size->w), &(size->h));
	free(text);
}

//设置基本属性
void setView(View * v, int32 x, int32 y, int32 w, int32 h)
{
	v->x = x;
	v->y = y;
	v->w = w;
	v->h = h;
	v->sta = STA_MAIN;
	v->id = (int)v;
}

//绘制普通按钮
void drawView(View * v)
{
	int32 rgb = getStaCls(v->sta);
	drect(v->x, v->y, v->w, v->h, getClsR(rgb), getClsG(rgb), getClsB(rgb));
}

//判断点是否在View内
int pointInView(View * v, int x, int y)
{
	if (x > v->x && y > v->y && x < v->x + v->w && y < v->y + v->h)
		return TRUE;
	return FALSE;
}

//普通进度条
typedef struct _Bar {
	View view;
	int8 index;		//进度，百分比
	int32 timer;		//定时器
} Bar;

void setBar(Bar * bar, int32 x, int32 y, int32 w, int32 h)
{
	setView(&(bar->view), x, y, w, h);
	bar->timer = -1;
	bar->index = 100;
}

void setBarIndex(Bar * bar, int8 ind)
{
	bar->index = ind;
}

void drawBar(Bar * bar)
{
	View *v;
	int32 rgb = theme_current->cls_normal;
	int32 argb = theme_current->cls_high;
	v = &(bar->view);
	drect(v->x, v->y, v->w, v->h, getClsR(rgb), getClsG(rgb), getClsB(rgb));
	drect(v->x, v->y, v->w * bar->index / 100, v->h, getClsR(argb),
		getClsG(argb), getClsB(argb));
}



//获取u8字符串绘制高度
int32 getTextHeight(char *u8, int32 w)
{
	Size size;
	uint16 *uni;
	int32 row;
	if (u8 == NULL)
		return 1;


	uni = (void *)ex_coding(u8, strlen(u8), "utf-8", "unicode");
	row = unitextrow(uni, getThemeFont(), w);
	free(uni);

	getFontSize("口", &size);
	return row * (size.h + 2);
}

void setTextView(TextView * tv, int32 x, int32 y, int32 w, int32 h)
{
	View *view = &(tv->view);
	setView(view, x, y, w, h);
	view->sta = STA_NORMAL;
}

//设置文字，内存由用户管理
void setTextViewText(TextView * tv, char *str)
{
	tv->text = str;
}

int32 setTextViewAutoHeight(TextView * tv)
{
	View *view = &(tv->view);
	int32 h = getTextHeight(tv->text, view->w);
	setView(view, view->x, view->y, view->w, h);
	return h;
}

//绘制文本按钮
void drawTextView1(TextView * tv)
{
	Size size;
	View *view;
	int32 rgb;
	rectst rect;
	colorst color;
	// char *gbstr;

	if (tv->text == NULL)
		return;


	view = &(tv->view);
	getFontSize(tv->text, &size);

	rgb = getStaCls(STA_NORMAL);
	drect(view->x, view->y, view->w, view->h, getClsR(rgb), getClsG(rgb),
		getClsB(rgb));

	rgb = theme_current->cls_text;

	rect.x = view->x < 0 ? 0 : view->x;
	rect.y = view->y < 0 ? 0 : view->y;
	rect.w = view->w;
	rect.h = view->h;


	getClsSt(rgb, &color);

	// gbstr = ex_coding(tv->text, strlen(tv->text), "GBK", "GB2312");
	dtextex(tv->text, view->x, view->y, &rect, &color, 2, 0);
	// free(gbstr);
}


//绘制文本按钮
void drawTextView(TextView * tv)
{
	Size size;
	View *view;
	int32 rgb;
	rectst rect;
	colorst color;
	// char *gbstr;

	if (tv->text == NULL)
		return;
	if (tv->view.y + tv->view.h< 0){
		return;
	}else if(tv->view.y>SCRH){
		return;
	}

	view = &(tv->view);
	getFontSize(tv->text, &size);

	rgb = getStaCls(STA_NORMAL);
	drect(view->x, view->y, view->w, view->h, getClsR(rgb), getClsG(rgb),
		getClsB(rgb));

	rgb = theme_current->cls_text;

	rect.x = view->x < 0 ? 0 : view->x;
	rect.y = view->y < 0 ? 0 : view->y;
	rect.w = view->w;
	rect.h = view->h;


	getClsSt(rgb, &color);

	// gbstr = ex_coding(tv->text, strlen(tv->text), "UTF-8", "GB2312");
	drawTextUTFX(tv->text, view->x, view->y, &rect, &color, 0, 0);
	// drawTextUTF(tv->text, )
	// free(gbstr);
}

//u8对话框
int32 u8EditCreate(char *title, char *text, int32 type, int32 max)
{
	char *untitle = ex_coding(title, strlen(title), "UTF-8", "unicode");
	char *untext = ex_coding(text, strlen(text), "UTF-8", "unicode");
	int32 edit = editcreate(untitle, untext, type, max);
	free(untitle);
	free(untext);
	return edit;
}


void setImageView(ImageView * tv, int32 x, int32 y, int32 w, int32 h)
{
	View *view = &(tv->view);
	setView(view, x, y, w, h);
	view->sta = STA_NORMAL;

	tv->path = NULL;
	tv->iw = w;
	tv->ih = h;
}

void setImageViewPath(ImageView * tv, char *path)
{
//	if (tv->path != NULL)
//		free(tv->image);
	tv->image = (uint16*)readBitmapFromAssets(path);
}

void setImageViewShow(ImageView * tv, int32 w, int32 h)
{
	tv->iw = w;
	tv->ih = h;
}

void freeImageView(ImageView *tv)
{
	if (tv->image)
		free(tv->image);
}

void drawImageView(ImageView *tv)
{
	View *view = &(tv->view);
	drawBitmapEx(tv->image, view->x, view->y, view->w, view->h, 0, 0, tv->iw, tv->ih);
}




//获取按钮的View视图
View *getBtnView(TextBtn * btn)
{
	return &(btn->view);
}

void setBtnClick(TextBtn * btn, mrc_timerCB call)
{
	btn->call = call;
}

//按钮点击回调函数
void clickBtnCall(int32 data)
{
	TextBtn *btn = (void *)data;
	View *view = getBtnView(btn);

	timerdel(btn->timer);
	view->sta = STA_MAIN;
	btn->timer = -1;
}


//设置按钮基本属性，文本按钮可强转使用
void setBtn(TextBtn * btn, int32 x, int32 y, int32 w, int32 h)
{
	setView(&(btn->view), x, y, w, h);
	btn->timer = -1;
	btn->call = clickBtnCall;
}

//设置文本按钮文字，内存由用户管理
void setBtnText(TextBtn * btn, char *str)
{
	btn->text = str;
}

int drawText1(char *str, int16 x, int16 y, int32 rgb)
{
	// char *gbstr = ex_coding(str, strlen(str), "GBK", "GB2312");
	int32 result =
		dtext(str, x, y, getClsR(rgb), getClsG(rgb), getClsB(rgb),
		FALSE, theme_current->font_size);
	// free(gbstr);
	return result;
}

int drawText(char *str, int16 x, int16 y, int32 rgb)
{
	// char *gbstr = ex_coding(str, strlen(str), "UTF-8", "GB2312");
	// int32 result =
		drawTextUTF(str, x, y, getClsR(rgb), getClsG(rgb), getClsB(rgb),
		FALSE, theme_current->font_size);
	// free(gbstr);
	return 0;
}


//绘制文本按钮
void drawBtn1(TextBtn * btn)
{
	Size size;
	View *view = &(btn->view);
	// int32 rgb = getStaCls(view->sta);
	getFontSize(btn->text, &size);

	drawView(view);
	drawText(btn->text, view->x + (view->w - size.w) / 2,
		view->y + (view->h - size.h) / 2,
		theme_current->cls_text_high);
}

//绘制文本按钮
void drawBtn(TextBtn * btn)
{
	Size size;
	View *view = &(btn->view);
	// int32 rgb = getStaCls(view->sta);
	getFontSize(btn->text, &size);

	drawView(view);
	drawText(btn->text, view->x + (view->w - size.w) / 2,
		view->y + (view->h - size.h) / 2,
		theme_current->cls_text_high);
}

//触发按钮的点击事件
void clickBtn(TextBtn * btn)
{
	int32 call = (int)btn;
	View *view = &(btn->view);
	if (btn->timer == -1) {
		view->sta = STA_HIGH;
		drawBtn1(btn);
		ref(0, 0, SCRW, SCRH);

		btn->timer = timercreate();
		timerstart(btn->timer, 300, call, btn->call, FALSE);
	}
}





Title *getTitle()
{
	return current_title;
}

void setTitle(int16 h, char *tit, char *info)
{
	View *view = &(current_title->view);
	setView(&(current_title->view), 0, 0, SCRW, h);
	view->h = h;
	current_title->title = tit;
	current_title->info = info;
}

void drawTitle()
{
	Size size;
	View *view = &(current_title->view);
	// int32 rgb = getStaCls(view->sta);
	//drawView(view);
	shaderect(view->x, view->y, view->w, view->h, theme_current->cls_high,
		theme_current->cls_main, 0);

	getFontSize(current_title->title, &size);
	drawText(current_title->title, view->x + 2,
		view->y + (view->h - size.h) / 2,
		theme_current->cls_text_high);

	getFontSize(current_title->info, &size);
	drawText(current_title->info, view->x + (view->w-size.w)/2,
		view->y + (view->h - size.h) / 2,
		theme_current->cls_text_high);

}




//移动消息
void msgRool(int32 rool)
{
	//遍历绘制消息
	View *view;
	TextView *tv;
	Link *temp = msgLink.head;

	//按下键，画面上去，rool<0
	Link *end = linkGetEnd(temp);
	Link *head ;
	if (end != NULL) {
		if (end->node != NULL) {
			tv = end->node;
			view = &(tv->view);
			if (view->y + view->h < SCRW - 30)
				rool = rool < 0 ? 0 : rool;
		}
	} else {
		rool = 0;
	}

	//按上键，画面下去，rool>0
	head = linkGetHead(temp);
	if (head != NULL) {
		if (head->node != NULL) {
			tv = head->node;
			view = &(tv->view);
			if (view->y >= 28)
				rool = rool > 0 ? 0 : rool;
		}
	} else {
		rool = 0;
	}

	if (temp != NULL) {
		if (temp->node != NULL) {
			tv = temp->node;
			view = &(tv->view);
			setTextView(tv, view->x, view->y + rool, view->w,
				view->h);
		}

		while (temp->next != NULL) {
			temp = temp->next;
			if (temp->node != NULL) {
				tv = temp->node;
				view = &(tv->view);
				setTextView(tv, view->x, view->y + rool,
					view->w, view->h);
			}
		}
	}
	main_act_call(0);
}

//释放整个链
void msgFreeAll()
{
	// Link *end;
	TextView *tv;
	Link *temp = msgLink.head;
	if (temp != NULL) {
		while (temp->next != NULL) {
			if (temp->next->node != NULL) {
				tv = temp->next->node;
				free(tv->text);
				free(tv);
				tv = NULL;
				printf("释放内存\n");
			}
			linkDelLink(temp->next);
			printf("删除节点\n");
		}

		if (temp->node != NULL) {
			tv = temp->node;
			free(tv->text);
			free(tv);
			tv = NULL;
			printf("释放内存\n");
		}
		linkDelLink(temp);
		printf("删除节点\n");
	}
}

int32 getIp(int32 a, int32 b, int32 c, int32 d)
{
	return (a << 24) + (b << 16) + (c << 8) + d;
}



void drawUI(char *text){
     mrc_clearScreen(240,240,240);
     drawTextUTF(text, 0,0, 20,20,20,0,1);
     mrc_refreshScreen(0,0,240,320);
}

void drawTextUTF(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font) {
	char temp[100];
	if (!is_unicode) {
		UTF8ToUni(pcText, temp, 100);
		mrc_drawText(temp, x, y, r, g, b, 1, font);
	} else {
		mrc_drawText(pcText, x, y, r, g, b, 1, font);
	}
}

void drawTextUTFX(char *pcText, int16 x, int16 y, rectst *rt,colorst *co, int is_unicode, uint16 font) {
	char temp[512];
	if (!is_unicode) {
		UTF8ToUni(pcText, temp, 512);
		dtextex(temp, x, y, rt,co, 1+2, font);
	} else {
		dtextex(pcText, x, y, rt,co, 1+2, font);
	}
}

void addMsg(char *text){
	TextView *tv;
	Link *link;
	TextView *prev;
	View *view;
	int32 uppos = 0;
//获取到消息，创建TextView
		tv = malloc(sizeof(TextView));
		tv->text = malloc(sizeof(char) * (BUFLEN + 2));
		memset(tv->text, '\0', sizeof(char) * (BUFLEN + 2));
		memcpy(tv->text, text, BUFLEN);
		tv->text[BUFLEN-1] = '0';
		//计算修改TextView
		link = linkGetEnd(msgLink.head);
		if (link != NULL && link->node != NULL) {
			prev = link->node;
			view = &(prev->view);
			setTextView(tv, 2, view->y + view->h + 1, SCRW - 4,
				SCRH - 30 * 2);
			
			setTextViewAutoHeight(tv);

		} else {
			setTextView(tv, 2, 30, SCRW - 4, SCRH - 30 * 2);
			setTextViewAutoHeight(tv);
		}

		//添加到消息节点
		linkAddNode(msgLink.head, tv);
		if(tv->view.y > SCRH - tv->view.h-30){
				uppos = SCRH - tv->view.h - tv->view.y-30;
				if(uppos<-60){
					uppos = -60;
				}
				msgRool(uppos);
			}
}

// socket已连接
 void ws_onOpen(int32 ws){
	//  char temp[BUFLEN];
	 cJSON* jsetname = cJSON_CreateObject();
	//基础属性
	cJSON_AddStringToObject(jsetname, "action", "setname");
	cJSON_AddStringToObject(jsetname, "data", login_name);
	//  mrc_sprintf(temp,  "{\"action\":\"setname\", \"data\":\"%s\"}", login_name);
	 
     mrc_printf("onOpen");
    //  if(!isSend){
	
        http_ws_send(ws,cJSON_Print(jsetname));
        http_ws_send(ws,"{\"action\":\"userlist\"}");
        // isSend = TRUE;
    // }
	cJSON_Delete(jsetname);
 }

 //接收socket消息
 void ws_onMessage(int32 ws, char *msg){
     mrc_printf("onMessage %s",msg);
    //  drawUI(msg);
	if (msg[0] == '{') {
			cJSON* json = cJSON_Parse(msg);
			char* action = cJSON_GetObjectItem(json, "action")->valuestring;
			//接收消息
			if (strcmp("sendmsg", action) == 0) {
				 addMsg(cJSON_GetObjectItem(json, "msg")->valuestring);
			}
			cJSON_Delete(json);
			//加入成功
			// else if (strcmp("join", action) == 0) {
			// 	if (Game.type == 0) {
			// 		Game.type = cJSON_GetObjectItem(json, "board")->valueint;
			// 	}
			// 	toastUTF8("有人加入了对局");
			// }
			//有人下子
			// else if (strcmp("down", action) == 0) {
			// 	x = cJSON_GetObjectItem(json, "x")->valueint;
			// 	y = cJSON_GetObjectItem(json, "y")->valueint;
			// 	type = cJSON_GetObjectItem(json, "type")->valueint;
			// 	board_down(board, x, y, type);

			// 	//AI自动下棋
			// 	if (Game.type != NOONE && Game.type != type) {
			// 		if (Game.useai) {
			// 			int dx, dy;
			// 			get_board_down_ai(board, Game.type, &dx, &dy);
			// 			sprintf(data, "{\"action\":\"down\",\"x\":%d,\"y\":%d,\"type\":%d}", dx, dy, Game.type);
			// 			http_ws_send(websocket, data);
			// 			//board_down(board, dx, dy, Game.type);
			// 		}
			// 	}
			// 	//toastUTF8(msg);
			// }
			//更新状态
			// else if (strcmp("state", action) == 0) {
			// 	txt = cJSON_GetObjectItem(json, "msg")->valuestring;
			// 	Game.state = cJSON_GetObjectItem(json, "state")->valueint;

			// 	//AI自动下棋
			// 	if (Game.state != NOONE && Game.state != END && Game.type != NOONE) {
			// 		if (Game.useai) {
			// 			int dx, dy;
			// 			get_board_down_ai(board, Game.type, &dx, &dy);
			// 			sprintf(data, "{\"action\":\"down\",\"x\":%d,\"y\":%d,\"type\":%d}", dx, dy, Game.type);
			// 			http_ws_send(websocket, data);
			// 			//board_down(board, dx, dy, Game.type);
			// 		}
			// 	}

			// 	char* state_str[] = {
			// 		"WAIT", "WHITE", "BLACK", "END"};
			// 	toastUTF8(state_str[Game.state]);
			// }
			//棋盘布局
			// else if (strcmp("board", action) == 0) {
			// }
			// //有人认输
			// else if (strcmp("tint", action) == 0) {
			// 	txt = cJSON_GetObjectItem(json, "msg")->valuestring;
			// 	toastUTF8(txt);
			// }
			//棋盘清空
			// else if (strcmp("clearboard", action) == 0) {
			// 	memset(board, 0, sizeof(int) * XN * YN);
			// 	txt = cJSON_GetObjectItem(json, "msg")->valuestring;
			// 	toastUTF8(txt);
			// }
			//出错信息
			// else if (strcmp("error", action) == 0) {
			// 	txt = cJSON_GetObjectItem(json, "msg")->valuestring;
			// 	// toast(txt,0);
			// } else if (strcmp("end", action) == 0) {
			// 	Game.type = 0;
			// 	Game.state = NOONE;
			// 	toastUTF8("对局结束");
			// } else {
			// 	//toastUTF8(msg);
			// }
		// 	cJSON_Delete(json);
		// } else {
		// 	if (Game.wait == 1) {
		// 		if (Game.state == NOONE || Game.state == END)
		// 			http_ws_send(websocket, "{\"action\":\"prepare\"}");
		// 		Game.wait = 0;
		// 	}
		// 	//toastUTF8(msg);
		}
		main_act_call(0);
	//  addMsg(msg);
 }

 //socket已关闭
 void ws_onClose(int32 ws){
     mrc_printf("onClose");
     drawUI("onClose");
 }

 //连接出错
 void ws_onError(int32 ws, int err){
     mrc_printf("onError %d",err);
     drawUI("onError");
 }

int32 netWorkCb(int32 result) {
//   net_init_type = result;
  mrc_printf("init network cb %d", result);
  
  return 0;
}


/*
void recvCb(int32 data)
{
	int32 len;
	SOC *chat = (void *)data;
	char *temp = malloc(sizeof(char) * BUFLEN);
	TextView *tv;
	Link *link;
	TextView *prev;
	View *view;

	memset(temp, '\0', sizeof(char) * BUFLEN);
	len = recv(chat->soc, temp, sizeof(char) * BUFLEN);
	if (len > 0) {
		if (chat->buff == NULL)
			chat->buff = malloc(sizeof(char) * BUFLEN);
		memset(chat->buff, '\0', sizeof(char) * BUFLEN);
		memcpy(chat->buff, temp, BUFLEN + 2);
		printf("ReceiveCb ing\n%s\n", chat->buff);

		//获取到消息，创建TextView
		tv = malloc(sizeof(TextView));
		tv->text = malloc(sizeof(char) * (BUFLEN + 2));
		memset(tv->text, '\0', sizeof(char) * (BUFLEN + 2));
		memcpy(tv->text, chat->buff, BUFLEN);

		//计算修改TextView
		link = linkGetEnd(msgLink.head);
		if (link != NULL && link->node != NULL) {
			prev = link->node;
			view = &(prev->view);
			setTextView(tv, 2, view->y + view->h + 1, SCRW - 4,
				SCRH - 30 * 2);
			setTextViewAutoHeight(tv);

		} else {
			setTextView(tv, 2, 30, SCRW - 4, SCRH - 30 * 2);
			setTextViewAutoHeight(tv);
		}

		//添加到消息节点
		linkAddNode(msgLink.head, tv);
	}
	free(temp);
}
*/

void errorCb(int32 data)
{
}


int32 strlen2(char *str)
{
	int32 len = 0;
	if (str != NULL) {
		while (*str++ != '\0')
			len++;
	}
	return len;
}

void drawBkg()
{
	cls(CLS_BKG >> 16 & 0xff, CLS_BKG >> 8 & 0xff, CLS_BKG & 0xff);
}

/*
void connectSoc(SOC * chat, int32 ip, int32 port)	//开始连接
{
	int32 net = (int)chat;
	chat->soc = socket(MR_SOCK_STREAM, MR_IPPROTO_TCP);
	connect(chat->soc, ip, port, MR_SOCKET_NONBLOCK);
	chat->con_timer = timercreate();
	timerstart(chat->con_timer, 200, net, chat->call, TRUE);
	printf("connect:%x:%d", ip, port);
}
*/

//发送数据
int32 socSend(SOC * chat, char *str)
{
	int32 len = strlen2(str);
	char temp[BUFLEN];
	 cJSON* jsend = cJSON_CreateObject();
	mrc_sprintf(temp, "%s: %s", login_name, str);
	//基础属性
	cJSON_AddStringToObject(jsend, "action", "sendmsg");
	cJSON_AddStringToObject(jsend, "msg", temp);
	//  mrc_sprintf(temp,  "{\"action\":\"setname\", \"data\":\"%s\"}", login_name);
	 
     mrc_printf("onOpen");
    //  if(!isSend){
	
        http_ws_send(wsocket, cJSON_Print(jsend));

        // isSend = TRUE;
    // }
	cJSON_Delete(jsend);
	// mrc_sprintf(temp, "{\"action\":\"sendmsg\":\"%s\"}",str);
	// int32 sendlen;
	// printf("socSend:(%d):%s\n", len, str);

	// sendlen = 0;
	// // 发送请求……
	// do 
	// {
	// 	sendlen+=send(chat->soc,str+sendlen,len-sendlen);
	// } while (sendlen < len);
	// http_ws_send(wsocket, temp);

	return len;
}

/*
void connectCb(int32 data)	//连接状态查询回调
{
	char login[100];
	SOC *chat = (void *)data;
	int32 state = getsocketstate(chat->soc);

	switch (state) {
	case MR_WAITING:	// 连接服务器        
		chat->use_time++;
		if (chat->use_time > 100)	// 连接服务器超时
		{
			timerstop(chat->con_timer);
		}
		printf("connect more time\n");
		break;
	case MR_SUCCESS:
		timerstop(chat->con_timer);
		chat->use_time = 0;

		if (login_name != NULL) {
			sprintf(login, "设置昵称 %s", login_name);
			login_name1 =ex_coding(login, wstrlen(login), "GBK", "UTF-8");
			socSend(chat, login_name1);
			free(login_name);
			login_name = NULL;
			free(login_name1);
			login_name1 = NULL;
		}
		//recvCb
		// timerstart(chat->con_timer, 100, data, chat->recv, TRUE);
		printf("connect start ReceiveCb\n");
		break;

	case MR_FAILED:
	case MR_IGNORE:
		timerstop(chat->con_timer);
		printf("connect MR_FAILED\n");
		break;
	}
}*/

void startAct(ACT * act)
{
	if (act_timer == -1)
		act_timer = timercreate();
	timerstop(act_timer);

	current_act = act;
	timerstart(act_timer, 200, 0, act->call, TRUE);
	printf("startAct:%s\n", act->name);
}

void stopAct(ACT* act){
	if(act_timer!=-1 && act_timer!=0){
		timerstop(act_timer);
		act_timer = -1;
	}
	
}

ACT *getCurrentAct()
{
	return current_act;
}

int32 getCurrentActId()
{
	return current_act->id;
}



void main_act_call(int32 data)
{
	char str[100];

	rectst rect;
	colorst color;
	//遍历绘制消息
	TextView *tv;
	Link *temp;

	rect.x = 4;
	rect.y = 30;
	rect.w = SCRW - 8;
	rect.h = SCRH - 30 * 2;
	getClsSt(theme_current->cls_text, &color);

	getdatetime(&time);
	sprintf(str, "%02d:%d", time.hour, time.minute);
	setTitle(28, "小蟀聊聊", str);

	drawBkg();


	temp = msgLink.head;
	if (temp != NULL) {
		if (temp->node != NULL) {
			tv = temp->node;
			drawTextView(tv);
		}
		while (temp->next != NULL) {
			temp = temp->next;
			if (temp->node != NULL) {
				tv = temp->node;
				drawTextView(tv);
			}
		}
	}

	drawTitle();
	drawBtn1(&runBtn);

	ref(0, 0, SCRW, SCRH);
}

void main_btn_call(int32 data)
{
	TextBtn *btn = (void *)data;
	View *view = getBtnView(btn);

	timerdel(btn->timer);
	view->sta = STA_MAIN;
	btn->timer = -1;
/*
	switch (view->id) 
	{
    case runBtn.view.id:
	if (main_edit == -1)
		main_edit = u8EditCreate("发送消息", "", 0, 10);
	break;
	}
*/
	if (view->id==runBtn.view.id)
	{
		if (main_edit == -1)
			main_edit = u8EditCreate("发送消息", "", 0, 10);
	}
}

void main_act_init()
{
	int32 ip;
	main_act.call = main_act_call;
	main_act.name = "首页";
	main_act.id = MAIN_ACT;

	setTitle(28, "小蟀聊天室", "小蟀制作");

	setBtn(&runBtn, 0, SCRH - 28, SCRW, 28);
	setBtnText(&runBtn, "输入内容");
	setBtnClick(&runBtn, main_btn_call);

	msgLink.head = LinkAlloc;
	msgLink.head->node = NULL;
	msgLink.head->prev = NULL;
	msgLink.head->next = NULL;

	ip = getIp(42, 193, 16, 54);
	chat.soc=0;
	// chat.call = connectCb;
	// chat.recv = recvCb;
	chat.error = errorCb;
	chat.buff = NULL;
	// connectSoc(&chat, ip, 8080);
	wsocket = http_ws("http://websocket.yzjlb.net:2022/socket", ws_onOpen,ws_onMessage,ws_onClose,ws_onError);
}

int main_act_event(int type, int p1, int p2)
{
	char *temp;
	View *view;
	if (MS_DOWN == type) {
		view = getBtnView(&runBtn);
		if (pointInView(view, p1, p2))
			clickBtn(&runBtn);
	}

	if (MR_DIALOG == type) {
		if (DLG_OK == p1) {
			temp = (char *)editget(main_edit);
			
			if (send_msg != NULL){
				free(send_msg);
				send_msg = NULL;
			}
				
			send_msg =ex_coding(temp, wstrlen(temp), "unicode", "utf-8");

			

			socSend(&chat, send_msg);
			free(send_msg);
			send_msg = NULL;
		}

		editdel(main_edit);
		main_edit = -1;
		
		
	}

	if (KY_DOWN == type) {
		switch (p1) {
case _2:
case _UP:
	msgRool(24);
	break;

case _8:
case _DOWN:
	msgRool(-24);
	break;
	case _LEFT:
	case _4:
	msgRool(240);
	break;
	case _RIGHT:
	case _6:
	msgRool(-240);
	break;

case _SLEFT:

	break;
case _SRIGHT:
	timerdel(act_timer);
	if (chat.buff != NULL)
		free(chat.buff);
	msgFreeAll();
	printf("退出");
	exit();
	break;
case _1:
	break;
		}
	}
	return 0;
}


//登录页面
void login_act_call(int32 data)
{
	char str[100];
	int i;

	getdatetime(&time);
	sprintf(str, "%02d:%d", time.hour, time.minute);
	setTitle(28, "登录", str);

	drawBkg();
	drawTitle();
	for (i = 0; i < LOGIN_BTN_NUM; i++)
		drawBtn1(&loginBtn[i]);

	drawImageView(&logo);
	ref(0, 0, SCRW, SCRH);
}

void login_act_exit()
{
	freeImageView(&logo);
	printf("退出");
}




int about_act_event(int type, int p1, int p2)
{
	if (KY_DOWN == type) {
		switch (p1) {
case _SRIGHT:
	freeImageView(&about_logo);
	startAct(&login_act);
	break;
case _1:
	break;
		}
	}
	return 0;
}


//登录页面
void about_act_call(int32 data)
{
	char str[100];

	getdatetime(&time);
	sprintf(str, "%02d:%d", time.hour, time.minute);
	setTitle(28, "关于", str);

	drawBkg();
	drawTextView(&about);
	drawImageView(&about_logo);
	drawTitle();

	ref(0, 0, SCRW, SCRH);
}

void about_act_init()
{
	int32 h;
	about_act.call = about_act_call;
	about_act.name = "关于页面";
	about_act.id = ABOUT_ACT;

	setTextView(&about, 0, 28, SCRW, SCRH - 28);
	setTextViewText(&about,
		"小蟀聊聊：\n打造mrp平台专业聊天软件\n\n作者：小蟀\n感谢：无尽时空、风的影子\n\nQQ群：1035494515\n");
	h = setTextViewAutoHeight(&about);

	setImageViewPath(&about_logo, "alipay.bmp");
	setImageView(&about_logo, (SCRW - 100) / 2, h + 38, 100, 100);
	setImageViewShow(&about_logo, 100, 100);
}


void login_btn_call(int32 data)
{
	TextBtn *btn = (void *)data;
	View *view = getBtnView(btn);

	timerdel(btn->timer);
	view->sta = STA_MAIN;
	btn->timer = -1;
/*
	switch (view->id) 
	{
    case loginBtn[0].view.id:
	about_act_init();
	startAct(&about_act);
	//login_act_event(KY_DOWN, _SRIGHT, 0);
	break;

    case loginBtn[1].view.id:

	main_act_init();
	startAct(&main_act);

	login_act_exit();
	break;

    case loginBtn[2].view.id:
	if (login_edit == -1)
		login_edit =
		u8EditCreate("设置您的昵称", "", 0, 10);
	break;
	}
*/
	if (view->id==loginBtn[0].view.id)
	{
		about_act_init();
		startAct(&about_act);
	} 
	else if(view->id==loginBtn[1].view.id)
	{
		main_act_init();
		stopAct(&main_act);
		// startAct(&main_act);
		current_act = &main_act;

		login_act_exit();
	}
	else if(view->id==loginBtn[2].view.id)
	{
		if (login_edit == -1)
			login_edit =
			u8EditCreate("设置您的昵称", "", 0, 10);
	}
}

void login_act_init()
{
	login_act.call = login_act_call;
	login_act.name = "登录页面";
	login_act.id = LOGIN_ACT;

	setTitle(28, "登录", "小蟀制作");

	setBtn(&loginBtn[0], SCRW / 6, SCRH - SCRH / 8, SCRW * 4 / 6, 28);
	setBtnText(&loginBtn[0], "关于");
	setBtnClick(&loginBtn[0], login_btn_call);

	setBtn(&loginBtn[1], SCRW / 6, SCRH - SCRH * 2 / 8, SCRW * 4 / 6, 28);
	setBtnText(&loginBtn[1], "登录");
	setBtnClick(&loginBtn[1], login_btn_call);

	setBtn(&loginBtn[2], SCRW / 6, SCRH - SCRH * 3 / 8, SCRW * 4 / 6, 28);
	setBtnText(&loginBtn[2], "昵称");
	setBtnClick(&loginBtn[2], login_btn_call);

	setImageViewPath(&logo, "logo1.bmp");
	setImageView(&logo, (SCRW-80) / 2, SCRW / 4, SCRW * 2 / 4, SCRW * 2 / 4);
	setImageViewShow(&logo, 80, 90);

}

int login_act_event(int type, int p1, int p2)
{
	char *temp;
	View *view;
	int i;

	if (MS_DOWN == type) {
		for (i = 0; i < LOGIN_BTN_NUM; i++) {
			view = getBtnView(&loginBtn[i]);
			if (pointInView(view, p1, p2))
				clickBtn(&loginBtn[i]);
		}
	}

	if (MR_DIALOG == type) {
		if (DLG_OK == p1) {
			temp = (char *)editget(login_edit);
			if (login_name != NULL){
				free(login_name);
				login_name = NULL;
			}
				
			login_name = ex_coding(temp, wstrlen(temp)+2, "unicode", "UTF-8");
			for(i=0;i<strlen(login_name);i++){
				if(login_name[i] == '\n' || login_name[i]=='\r'){
					login_name[i] = ' ';
				}
			}
		}
		editdel(login_edit);
		login_edit = -1;
	}

	if (KY_DOWN == type) {
		switch (p1) {
case _SRIGHT:
	login_act_exit();

	timerdel(act_timer);
	if (login_name != NULL){
		free(login_name);
		login_name = NULL;
	}
		
	exit();
	break;
case _1:
	break;
		}
	}
	return 0;
}








/*
void drawRect(int16 x, int16 y, int16 w, int16 h, int32 color)
{
	drect(x, y, w, h, color>>16&0xff, color>>8&0xff, color&0xff);
}*/



int32 cbInitNetwork(int32 result)//网络拨号回调函数
{
return 0;
}


int32 init(void)
{
	theme_current = &theme_normal;
	main_edit = -1;
	login_edit = -1;
	act_timer = -1;
	url = "http://42.193.16.54:8080";
	current_title = &_title;

	mpc_init();
	themeInit();
	login_act_init();
	initnetwork(cbInitNetwork,"cmnet");
	startAct(&login_act);
	return 0;
}

int32 event(int32 type, int32 p1, int32 p2)
{
	switch (getCurrentActId()) {
	case MAIN_ACT:
		main_act_event(type, p1, p2);
		break;

	case LOGIN_ACT:
		login_act_event(type, p1, p2);
		break;

	case ABOUT_ACT:
		about_act_event(type, p1, p2);
		break;
	}
	return 0;
}

int32 pause(void)
{
	return 0;
}

int32 resume(void)
{
	return 0;
}

int32 exitapp()
{
	mpc_exit();
	return 0;
}


int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
  mrc_printf("mrc_extRecvAppEvent");
  return 0;
}
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3,
                            int32 p4, int32 p5) {
  mrc_printf("mrc_extRecvAppEventEx");
  return 0;
}