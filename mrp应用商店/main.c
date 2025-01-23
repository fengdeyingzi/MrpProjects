#include <mrc_base.h>
#include <mrc_base_i.h>
// #include <mrc_network.h>
#include "buffer.h"
#include "cJSON.h"
#include "filelist.h"
#include "httpparser.h"
#include "mpc.h"
#include "url_docode.h"
#include "xl_coding.h"
// int32 SCRW, SCRH;

ListView *list_mrp;
ListView *list_shop;
int shop_page;
int tab_index;

// int _dtext(const char *pcText, int x, int y, int r, int g, int b, int is_unicode, int font) {
//     uint16 *un_text = pcText;
// 	uint32 temp_len = 0;
//     if(!is_unicode){
// 		temp_len = strlen(pcText)*2+2;
// 		un_text = malloc(temp_len);
//         UTF8ToUni(pcText, un_text,temp_len);
//     }
//     int32 len = wstrlen(un_text);
//     int ix = x;
//     int iy = y;
//     uint16 ncolor = MAKERGB565(r, g, b);
//     int fontw, fonth;
//     for (int i = 0; i < len/2; i++) {
//         uint16 id = un_text[i] << 8 | un_text[i] >> 8;
//         // mrc_drawChar(id, ix, iy, ncolor);
//         // _mr_table->mr_getCharBitmap(id, font, &fontw, &fonth);
//         ix += fontw;
//     }
// 	if(!is_unicode){
// 		free(un_text);
// 	}
//     return 0;
// }
extern void http_onProgress(int32 pos);
extern void http_onSuccess(int32 ip, char *data, char *body);
extern void http_onError(int32 err);
extern void drawUI();
extern void drawTextUTF(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);

//搜索mrp utf8编码
void searchMrp(char *text) {
	char url[300];
	char *temp = mrc_malloc(300);
	mrc_memset(temp, 0, 300);
	strcpy(temp, text);
	urlencode(temp);
	mrc_sprintf(url, "http://mrp.yzjlb.net/api.php?s=%s", temp);
	mrc_free(temp);
	http_getData(url, http_onProgress, http_onSuccess, http_onError);
}

//获取mrp列表 1开始
void getMrpList(int page) {
	char url[300];
	mrc_sprintf(url, "http://mrp.yzjlb.net/api.php?page=%d", page);
	http_getData(url, http_onProgress, http_onSuccess, http_onError);
}

//显示状态信息
void drawInfo(char *text) {
	char temp[300];
	int32 fontw, fonth;
	int height = 24;
	UTF8ToUni(text, temp, 300);
	mrc_textWidthHeight(temp, 1, 1, &fontw, &fonth);
	mrc_drawRect(60, SCRH - 30 + (30 - height) / 2, SCRW - 120, height, 255, 255, 255);

	mrc_drawText(temp, (SCRW - fontw) / 2, SCRH - 30 + (30 - fonth) / 2, 33, 33, 33, 1, 1);
	mrc_refreshScreen(0, SCRH - 30, SCRW, 30);
	mrc_printf("drawInfo %s", text);
}

void httpDownloadFile(char *url) {
	mrc_printf("downloadFile %s", url);
	wap(url);
}

void shop_onClidk(int32 data) {
	httpDownloadFile(list_getCurItem(list_shop)->path);
}

void file_onClick(int32 data) {
	mrc_runMrp(list_getCurItem(list_mrp)->path, "start.mr", NULL);
}

void drawBody(char *text) {
	mr_screenRectSt rect;
	mr_colourSt colors;
	char temp[300];
	rect.x = 0;
	rect.y = 0;
	rect.w = SCRW;
	rect.h = SCRH - 24;
	colors.r = 33;
	colors.g = 33;
	colors.b = 33;

	UTF8ToUni(text, temp, 300);
	mrc_drawRect(0, 0, SCRW, SCRH - 24, 200, 200, 200);
	mrc_drawTextExVM(temp, 0, 0, rect, colors, 1 + 2, 1);
	//   mrc_drawText(temp,0,0,33,33,33,1,1);
	mrc_refreshScreen(0, 0, SCRW, SCRH - 24);
	//   mrc_printf("drawBody %s",text);
}

void drawTabBar() {
	int32 fontw, fonth;
	mrc_textWidthHeight("\x5f\x71\0\0", 1, 1, &fontw, &fonth);
	//显示标题
	mrc_drawRect(0, 0, SCRW, 30, 120, 120, 120);

	if (tab_index == 0) {
		mrc_drawRect(2, 2, SCRW / 2 - 4, 30 - 4, 240, 240, 240);
		drawTextUTF("本地mrp", 10, (30 - fonth) / 2, 20, 20, 20, 0, 1);
		drawTextUTF("网络mrp", SCRW / 2 + 10, (30 - fonth) / 2, 240, 240, 240, 0, 1);
	} else {
		mrc_drawRect(2 + SCRW / 2, 2, SCRW / 2 - 4, 30 - 4, 240, 240, 240);
		drawTextUTF("本地mrp", 10, (30 - fonth) / 2, 240, 240, 240, 0, 1);
		drawTextUTF("网络mrp", SCRW / 2 + 10, (30 - fonth) / 2, 20, 20, 20, 0, 1);
	}
}

void switchTab(int index) {
	mrc_printf("switchTab %d", index);
	tab_index = index;
	if (tab_index == 0) {
		list_show(list_mrp);
		list_hide(list_shop);
	} else {
		list_hide(list_mrp);
		list_show(list_shop);
		mrc_printf("switchTab getMrpList");
		if(list_shop->list_len == 0)
		getMrpList(shop_page);
	}
	mrc_printf("switchTab drawUI");
	drawUI();
}

void tabBar_event(int32 type, int32 p0, int32 p1) {
	int32 index = 0;
	mrc_printf("tabBar_event %d %d %d", type, p0, p1);
	if (type == MR_MOUSE_UP) {
		if (p1 < 30) {
			index = p0 / (SCRW / 2);
			switchTab(index);
		}
	}
	if (type == MR_KEY_RELEASE) {
		if (p0 == MR_KEY_1) {
			switchTab(0);
		} else if (p0 == MR_KEY_2) {
			switchTab(1);
		}
	}
}

void http_onProgress(int32 pos) {
	char temp[100];
	mrc_sprintf(temp, "获取中 %d", pos);
	drawInfo(temp);
}
void http_onSuccess(int32 ip, char *data, char *body) {
	cJSON *cjson;
	cJSON *test_arr;
	cJSON *arr_item;
	int arr_size;
	int i;
	char *downloadUrl;
	char *label;
	char *name;
	downloadUrl = mrc_malloc(300);
	drawInfo("获取成功");
	// drawInfo(body);

	//第一步打包JSON字符串
	cjson = cJSON_Parse(body);
	//判断是否打包成功
	if (cjson == NULL) {
		mrc_printf("cjson error…");
	} else {  //打包成功调用cJSON_Print打印输出
		mrc_printf(cJSON_Print(cjson));
	}
	mrc_printf("onSuccess %s", cjson->child->string);
	//获取数组对象
	// test_arr = cJSON_GetObjectItem(cjson, "data");
	test_arr = cjson->child;
	mrc_printf("onSuccess 2");
	//获取数组对象个数便于循环
	arr_size = cJSON_GetArraySize(test_arr);  //return arr_size 2
	mrc_printf("arr_size %d", arr_size);
	//获取test_arr数组对象孩子节点
	arr_item = test_arr->child;	 //子对象
	//循环获取数组下每个字段的值并使用cJSON_Print打印
	list_clear(list_shop);
	for (i = 0; i <= (arr_size - 1) /*0*/; ++i) {
		arr_item = cJSON_GetArrayItem(test_arr, i);
		// mrc_printf((cJSON_GetObjectItem(arr_item, "md5")));
		// mrc_printf(cJSON_Print(arr_item));
		mrc_printf("%s", (cJSON_GetObjectItem(arr_item, "label")->valuestring));
		mrc_printf(cJSON_GetObjectItem(arr_item, "name")->valuestring);

		mrc_sprintf(downloadUrl, "http://mrp.yzjlb.net/download.php?md5=%s", cJSON_GetObjectItem(arr_item, "md5")->valuestring);
		// label = ex_coding(cJSON_GetObjectItem(arr_item, "label")->valuestring,strlen(cJSON_GetObjectItem(arr_item, "label")->valuestring),"UTF-8","GBK");
		// name = ex_coding(cJSON_GetObjectItem(arr_item, "name")->valuestring,strlen(cJSON_GetObjectItem(arr_item, "name")->valuestring),"UTF-8","GBK");
		label = cJSON_GetObjectItem(arr_item, "label")->valuestring;
		name = cJSON_GetObjectItem(arr_item, "name")->valuestring;
		list_addData(list_shop, label, name, downloadUrl);
		// mrc_free(label);
		// mrc_free(name);
		// arr_item = arr_item->next;	//下一个子对象
	}

	//delete cjson 这里只需要释放cjson即可，因为其它的都指向它
	cJSON_Delete(cjson);
	mrc_free(downloadUrl);
	switchTab(1);
}

void http_onError(int32 err) {
	drawInfo("获取出错");
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

void drawUI() {
	int32 fontw, fonth;
	char temp[100];
	mrc_textWidthHeight("\x5f\x71\0\0", 1, 1, &fontw, &fonth);
	mrc_clearScreen(255, 255, 255);

	//显示标题
	mrc_drawRect(0, 0, SCRW, 30, 120, 120, 120);
	drawTabBar();
	// UTF8ToUni("影子mrp列表", temp, 100);
	// mrc_drawText(temp, 10, (30 - fonth) / 2, 240, 240, 240, 1, 1);
	list_draw(list_mrp);
	list_draw(list_shop);
	//绘制底部菜单
	mrc_drawRect(0, SCRH - 30, SCRW, 30, 120, 120, 120);
	drawTextUTF("确定", 10, SCRH - 30 + (30 - fonth) / 2, 240, 240, 240, 0, 1);
	drawTextUTF("退出", SCRW - 10 - fontw * 2, SCRH - 30 + (30 - fonth) / 2, 240, 240, 240, 0, 1);
	mrc_refreshScreen(0, 0, SCRW, SCRH);
}

int32 mrc_init() {
	// mrc_drawText("影子俱乐部www.yzjlb.net",0, 0, 230, 230, 230, 0, 1);
	// mrc_drawText("影子论坛bbs.yzjlb.net",0,30,230,230,230,0,1);
	//   int i = 0;
	mr_screeninfo scrinfo;
	mrc_getScreenInfo(&scrinfo);
	mpc_init();
	tab_index = 0;
	// SCRW = scrinfo.width;
	// SCRH = scrinfo.height;

	list_mrp = list_create(0, 30, SCRW, SCRH - 30 - 30);
	list_shop = list_create(0, 30, SCRW, SCRH - 30 - 30);
	list_setonclick(list_mrp, (void *)file_onClick);
	list_setonclick(list_shop, (void *)shop_onClidk);
	list_hide(list_shop);
	mrc_clearScreen(255, 255, 255);
	list_find_txt(list_mrp, "");
	list_draw(list_mrp);
	mrc_refreshScreen(0, 0, SCRW, SCRH);
	http_init();
	drawUI();
	shop_page = 1;
	//   void* timerCB = http_onSuccess;
	//   test_test jjj = http_onSuccess;
	//   HTTP_ONSUCCESS onsuccess = http_onSuccess;
	//   HTTP_ONPROGRESS onprogress = http_onProgress;
	//   HTTP_ONERROR onerr = http_onError;
	// http_getData("http://mrp.yzjlb.net/api.php", http_onProgress, http_onSuccess, http_onError);

	return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1) {
	//   int32 state = 0;
	tabBar_event(code, param0, param1);
	if (tab_index == 0) {
		list_event(list_mrp, code, param0, param1);
	} else if (tab_index == 1) {
		list_event(list_shop, code, param0, param1);
	}
	if (code == MR_KEY_RELEASE) {
		if (param0 == MR_KEY_SOFTRIGHT) {
			mrc_exit();
		} else if (param0 == MR_KEY_SELECT || param0 == MR_KEY_SOFTLEFT) {
			if (tab_index == 0) {
				mrc_printf("%s %s", list_getCurItem(list_mrp)->DisplayName, list_getCurItem(list_mrp)->DisplayName);
				file_onClick(0);
			} else {
				shop_onClidk(0);
			}
		}
	} else if (code == MR_MOUSE_UP) {
		//判断是否点底部
		if (param1 > SCRH - 30) {
			if (param0 < SCRW / 2) {
				if (tab_index == 0) {
					mrc_printf("%s %s", list_getCurItem(list_mrp)->DisplayName, list_getCurItem(list_mrp)->DisplayName);
					file_onClick(0);
				} else {
					shop_onClidk(0);
				}
			} else {
				mrc_exit();
			}
		}
	}

	drawUI();
	return 0;
}
int32 mrc_pause() { return 0; }
int32 mrc_resume() { return 0; }
int32 mrc_exitApp() {
	http_exit();
	list_free(list_shop);
	list_free(list_mrp);
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