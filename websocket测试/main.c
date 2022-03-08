#include <mrc_base.h>
#include "http/httpclient.h"
#include "xl_coding.h"

int32 wsocket;
extern void drawTextUTF(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);

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

// socket已连接
 void ws_onOpen(int32 ws){
     mrc_printf("onOpen");
    //  if(!isSend){
        http_ws_send(ws, "{\"action\":\"setname\", \"data\":\"test\"}");
        // isSend = TRUE;
    // }
 }

 //接收socket消息
 void ws_onMessage(int32 ws, char *msg){
     mrc_printf("onMessage %s",msg);
     drawUI(msg);
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

int32 mrc_init(void)
{
    mrc_initNetwork(netWorkCb, "cmnet");
    wsocket = http_ws("http://websocket.yzjlb.net:2022/socket", ws_onOpen, ws_onMessage, ws_onClose, ws_onError);
	return 0; //momo_init();
}

int32 mrc_event(int32 type,int32 p1,int32 p2){
    if(type == MR_KEY_UP){
        if(p1 == MR_KEY_SOFTRIGHT){
            mrc_exit();
        }
    }else if(type == MR_MOUSE_UP){
        mrc_exit();
    }
    return 0;
}

int32 mrc_pause(){
    return 0;
}

int32 mrc_resume(){
    return 0;
}

int32 mrc_exitApp(){
    http_ws_exit(wsocket);
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