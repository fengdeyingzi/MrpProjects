#include <mrc_base.h>
#include <mrc_base_i.h>
// #include <mrc_network.h>

#include "platEx.h"

extern void drawInfo(char *text);
extern void drawBody(char *text);



int32 SCRW, SCRH;



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

void logoc(int32 data) {
  mrc_clearScreen(30, 50, 30);
  mrc_refreshScreen(0, 0, 240, 320);
}

int32 mrc_drawTextUTF(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font){
  char *temp = malloc(mrc_strlen(pcText)+2);
    if(is_unicode){
      mrc_drawText(pcText, x,y, r,g,b, is_unicode,font);
    }
    else{
      UTF8ToUni(pcText,temp,mrc_strlen(pcText)+2);
      mrc_drawText(temp, x,y, r,g,b, 1,font);
    }
    free(temp);
  	return 0;
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


int32 mrc_init() {
  int i = 0;
  mr_screeninfo scrinfo;
  mrc_getScreenInfo(&scrinfo);
  SCRW = scrinfo.width;
  SCRH = scrinfo.height;

  
  
  for (i = 0; i < 1; i++) {
    mrc_clearScreen(30, 30, 30);
    mrc_drawTextUTF("1 led背光开", 10, 10, 240, 240, 240, 0, 1);
    mrc_drawTextUTF("2 led背光关", 10, 40, 240, 240, 240, 0, 1);
    mrc_drawTextUTF("3 获取短信中心号", 10, 70, 240, 240, 240, 0, 1);
    mrc_drawTextUTF("4 检测是否触屏", 10, 100, 240, 240, 240, 0, 1);
    mrc_drawTextUTF("5 获得手机语言", 10, 130, 240, 240, 240, 0, 1);
    mrc_drawTextUTF("6 查询t卡状态", 10, 160, 240, 240, 240, 0, 1);
    mrc_drawTextUTF("7 获得当前情景模式", 10, 190, 240, 240, 240, 0, 1);
    mrc_drawTextUTF("8 设置播放音量 3", 10, 210, 240, 240, 240, 0, 1);

    mrc_refreshScreen(0, 0, 240, 320);
    // mrc_sleep(100);
  }

  

  return 0;
}








int32 mrc_event(int32 code, int32 param0, int32 param1) {
  int re = 0;
  char temp[200];
  if (code == MR_KEY_RELEASE) {
    if (param0 == MR_KEY_DOWN) {
      mrc_clearScreen(24, 230, 30);

      mrc_refreshScreen(0, 0, 240, 320);
      mrc_refreshScreen(0, 0, 240, 320);
    } else if (param0 == MR_KEY_UP) {
      mrc_clearScreen(24, 30, 230);
      mrc_drawText("\x5f\x71\x5b\x50\x0\x0", 0, 0, 20, 20, 20, 1, 1);
      mrc_refreshScreen(0, 0, 240, 320);
    } else if (param0 == MR_KEY_LEFT) {
      mrc_clearScreen(230, 230, 230);
      mrc_drawLine(0, 0, 200, 200, 30, 30, 30);
      mrc_refreshScreen(0, 0, 240, 320);
    } else if (param0 == MR_KEY_RIGHT) {
      mrc_clearScreen(140, 32, 87);
      mrc_drawRect(0, 0, 30, 30, 240, 240, 240);
      mrc_refreshScreen(0, 0, 240, 320);
    } else if (param0 == MR_KEY_0) {
      mrc_clearScreen(24, 230, 30);
      mrc_clearScreen(24, 230, 30);
      mrc_refreshScreen(0, 0, 240, 320);

    } else if (param0 == MR_KEY_1) {
      ledSet(1);
      drawInfo("背光已开");
    } else if (param0 == MR_KEY_2) {
      ledSet(0);
      drawInfo("背光已关");
    } else if (param0 == MR_KEY_3) {  // socket
      getSmsCenter();
    } else if (param0 == MR_KEY_4) {
      re = isTouchScreen();
      mrc_sprintf(temp, "是否触屏:%d", re);
      drawInfo((char*)temp);
    } else if (param0 == MR_KEY_5) {  // send
      re = getLanguage();
      mrc_sprintf(temp, "语言:%d", re);
      drawInfo((char*)temp);
    } else if (param0 == MR_KEY_6) {  // recev
      re = getTSDCardState();
      mrc_sprintf(temp, "T卡状态:%d", re);
      drawInfo((char*)temp);
    } else if (param0 == MR_KEY_7) {
      re = getModel();
      mrc_sprintf(temp, "情景模式:%d", re);
      drawInfo((char*)temp);
    } else if (param0 == MR_KEY_8) {
		re = setVolue(5);
      mrc_sprintf(temp, "设置音量:%d", re);
      drawInfo((char*)temp);
    } else if (param0 == MR_KEY_SOFTRIGHT) {
      mrc_exit();
    }
  } else if (code == MR_MOUSE_UP) {
    mrc_exit();
  }
  return 0;
}
int32 mrc_pause() { return 0; }
int32 mrc_resume() { return 0; }
int32 mrc_exitApp() {
  

 
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