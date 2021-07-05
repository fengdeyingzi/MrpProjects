#include <mrc_base.h>
#include <mrc_base_i.h>
// #include <mrc_network.h>
#include "buffer.h"



int32 SCRW, SCRH;
int32 timer;


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
  mrc_drawTextEx(temp, 0, 0, rect, colors, 1+2, 1);
  mrc_drawText(temp,0,0,33,33,33,1,1);
  mrc_refreshScreen(0, 0, SCRW, SCRH - 24);
  mrc_printf("drawBody %s",text);
}

int32 mrc_init() {
  // mrc_drawText("影子俱乐部www.yzjlb.net",0, 0, 230, 230, 230, 0, 1);
  // mrc_drawText("影子论坛bbs.yzjlb.net",0,30,230,230,230,0,1);
  int i = 0;
  mr_screeninfo scrinfo;
  mrc_getScreenInfo(&scrinfo);
  SCRW = scrinfo.width;
  SCRH = scrinfo.height;



  

  return 0;
}





int32 mrc_event(int32 code, int32 param0, int32 param1) {
  int32 state = 0;
  if (code == MR_KEY_RELEASE) {
    if(param0 == MR_KEY_SOFTRIGHT){
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