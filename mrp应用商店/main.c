#include <mrc_base.h>
#include <mrc_base_i.h>
// #include <mrc_network.h>
#include "buffer.h"
#include "httpparser.h"
#include "filelist.h"
#include "mpc.h"
// int32 SCRW, SCRH;

ListView *list_mrp;
ListView *list_shop;
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

//UTF8转换为Unicode
//参数：UTF8字符串，Unicode缓冲区，缓冲区大小
//返回：缓冲区使用大小
int UTF8ToUni( char *utf8str, char *unistr, int size)
{
    int32 i = 0, u = 0;
    uint8 *utf8, *unicode;

    utf8 = (unsigned char*)utf8str;
    unicode = (unsigned char*)unistr;

    while (utf8[i] && u < size - 2)
    {
        if((utf8[i] & 0x80) == 0)
        {
            // one byte 0...
            unicode[u] = 0;
            unicode[u + 1] = utf8[i++];
        }else if((utf8[i] & 0x20) == 0){
            // two bytes 110... 10...
            unicode[u] = (unsigned char)((utf8[i] & 0x1f) >> 2);
            unicode[u + 1] = (unsigned char)((utf8[i] << 6) & 0xff) | (utf8[i + 1] & 0x3f);
            i += 2;
        }else{
            // three bytes 1110... 10... 10...
            unicode[u] = (unsigned char)((utf8[i] << 4) & 0xff) | ((utf8[i + 1] & 0x3f) >> 2);
            unicode[u + 1] = (unsigned char)((utf8[i + 1] << 6) & 0xff) | (utf8[i + 2] & 0x3f);
            i += 3;
        }

        u += 2;
    }

    unicode[u] = '\0';
    unicode[u + 1] = '\0';

    return u;
}




//显示状态信息
void drawInfo(char *text) {
	char temp[300];
	UTF8ToUni(text,temp,300);
  mrc_drawRect(0, SCRH - 24, SCRW, 24, 255, 255, 255);

  mrc_drawText(temp, 10, SCRH - 24, 33, 33, 33, 1, 1);
  mrc_refreshScreen(0, SCRH - 24, SCRW, 24);
  mrc_printf("drawInfo %s",text);
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
  
  UTF8ToUni(text,temp,300);
  mrc_drawRect(0, 0, SCRW, SCRH - 24, 200, 200, 200);
  mrc_drawTextExVM(temp, 0, 0, rect, colors, 1+2, 1);
//   mrc_drawText(temp,0,0,33,33,33,1,1);
  mrc_refreshScreen(0, 0, SCRW, SCRH - 24);
//   mrc_printf("drawBody %s",text);
}

void http_onProgress(int32 pos){
    char temp[100];
    mrc_sprintf(temp,"获取中 %d",pos);
    // drawInfo(temp);
}

void http_onSuccess(int32 ip, char *data, char *body){
    drawInfo("获取成功");
    drawBody(body);
}

void http_onError(int32 err){
    drawInfo("获取出错");
}

void drawTextUTF(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font){
char temp[100];
if(!is_unicode){
  UTF8ToUni(pcText,temp,100);
  mrc_drawText(temp,x,y,r,g,b,1,font);
}
else{
  mrc_drawText(pcText,x,y,r,g,b,1,font);
}


}

void drawUI(){
  int32 fontw,fonth;
  char temp[100];
  mrc_textWidthHeight("\x5f\x71\0\0", 1, 1, &fontw, &fonth);
  mrc_clearScreen(255,255,255);

  //显示标题
  mrc_drawRect(0,0,SCRW,30, 120,120,120);
  UTF8ToUni("影子mrp列表",temp,100);
  mrc_drawText(temp,10,(30-fonth)/2, 240,240,240,1,1);
  list_draw(list_mrp);
  list_draw(list_shop);
  //绘制底部菜单
  mrc_drawRect(0, SCRH-30, SCRW, 30, 120,120,120);
  drawTextUTF("确定",10,SCRH-30+(30-fonth)/2, 240,240,240,0,1);
  drawTextUTF("退出",SCRW-10 - fontw*2, SCRH-30+(30-fonth)/2, 240,240,240,0,1);
  mrc_refreshScreen(0,0,SCRW,SCRH);
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
  
  list_mrp = list_create(0,30,SCRW, SCRH-30-30);
  list_shop = list_create(0,30,SCRW, SCRH-30-30);
  list_hide(list_shop);
  mrc_clearScreen(255,255,255);
  list_find_txt(list_mrp, "");
  list_draw(list_mrp);
  mrc_refreshScreen(0,0,SCRW,SCRH);
  http_init();
//   void* timerCB = http_onSuccess;
//   test_test jjj = http_onSuccess;
//   HTTP_ONSUCCESS onsuccess = http_onSuccess;
//   HTTP_ONPROGRESS onprogress = http_onProgress;
//   HTTP_ONERROR onerr = http_onError;
  // getHttpData("http://mrp.yzjlb.net/api.php", http_onProgress, http_onSuccess, http_onError);

  return 0;
}





int32 mrc_event(int32 code, int32 param0, int32 param1) {
//   int32 state = 0;
  if(tab_index == 0){
    list_event(list_mrp, code, param0,param1);
  }
  else if(tab_index == 1){
    list_event(list_shop, code,param0,param1);
  }
  if (code == MR_KEY_RELEASE) {
    if(param0 == MR_KEY_SOFTRIGHT){
        mrc_exit();
    }
    else if(param0 == MR_KEY_SELECT || param0 == MR_KEY_SOFTLEFT){
       if(tab_index == 0){
         mrc_printf("%s %s",list_getCurItem(list_mrp)->DisplayName, list_getCurItem(list_mrp)->DisplayName);
         mrc_runMrp(list_getCurItem(list_mrp)->path, "start.mr", NULL);
       }
    }
  } else if (code == MR_MOUSE_UP) {
     //判断是否点底部
     if(param1 > SCRH-30){
       if(param0 < SCRW/2){
         mrc_runMrp(list_getCurItem(list_mrp)->path, "start.mr", NULL);
       }
       else{
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