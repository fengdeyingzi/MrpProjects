#include <mrc_base.h>
#include <mrc_base_i.h>
#include <mrc_network.h>

#include "buffer.h"

extern void drawInfo(char *text);
extern void drawBody(char *text);
extern int32 onHttpCb(int32 ip) ;
extern int32 netWorkCb(int32 result);
extern void print_ip(int32 ip);
/*
void test(){
        int i=0;
int n=0;
for(n=0;i<255;n++)
for(i=0;i<255;i++){
mrc_clearScreen(i,i,i);
//mrc_drawText("\x98\xce\x76\x84\x5f\x71\x5b\x50\x0\x0", 0, 30, 30,30,30, 1, 1);
mrc_refreshScreen(0,0,240,320);
}
}
*/
int32 SCRW, SCRH;
int32 timer;
char temp[30];
int32 channel;
int32 ip;
int32 socket;  //一个socket连接
int32 net_init_type;
STRING_BUFFER *buffer_http;
int32 socket_state;  // socket状态 0 gethost 1 connect 2 send 3 recv 4 ok -1
                     // 错误
char *send_buf = NULL;
int32 send_len = 0;
int local_ip = 0x7f000001;

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

void http_logoc(int32 data) {
  char buf[100];
  int32 con_state = 0;
  int32 temp_int = 0;
  char *temp_str;
  int32 content_len = 0;
  if (socket_state == 1) {  // connect
    //  MR_SUCCESS ：  连接成功
    //   MR_FAILED  ：连接失败
    //   MR_WAITING ：连接中
    //   MR_IGNORE  ：不支持该功能
    con_state = mrc_getSocketState(socket);
    if (con_state == MR_SUCCESS) {
      socket_state = 2;
    }
  } else if (socket_state == 2) {  // send
    temp_int = mrc_send(socket, send_buf+send_len, mrc_strlen(send_buf)-send_len);
    if (temp_int >= 0) {
      send_len += temp_int;
      if (send_len == mrc_strlen(send_buf)) {
        socket_state = 3;
      }
	  mrc_sprintf(buf, "发送数据中 %d %d",socket, send_len);
      drawInfo(buf);
    } else {
      drawInfo("send发送失败");
    }
  } else if (socket_state == 3) {  // recv
    temp_int = mrc_recv(socket, buf, sizeof(buf));
    if (temp_int >= 0) {
		
    mrc_printf(buffer_http->data);
    
      buffer_appendx(buffer_http, buf, temp_int);
      mrc_sprintf(buf,"接收数据中 %d %d %d",socket,buffer_http->len,mrc_strlen(buffer_http->data));
      drawInfo(buf);
		//判断header是否完成
		if(mrc_strstr(buffer_http->data, "\r\n\r\n")){
			//读取头完成
			temp_str = mrc_strstr(buffer_http->header, "Content-Length");
      mrc_printf("!=null");
			if(temp_str != NULL){
        mrc_printf("getline_value begin");
				temp_str = getline_value(temp_str);
        mrc_printf("getline_value %s",temp_str);
				content_len = mrc_atoi(temp_str);
        mrc_printf("atoi %d",content_len);
				mrc_free(temp_str);
				temp_int = isContentSuccess(buffer_http->data,content_len);
				if(temp_int){
					mrc_closeSocket(socket);
					buffer_http->body = mrc_strstr(buffer_http->data, "\r\n\r\n");
					buffer_http->body += 4;
          mrc_printf("http获取成功 %s", buffer_http->body);
					drawInfo("http获取成功");
					drawBody(buffer_http->body);
          socket_state = -1;
				}
			}
		}
    } else {
      drawInfo("接收数据失败");
      socket_state = -1;
    }
  }
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

  timer = mrc_timerCreate();
  buffer_http = buffer_create();
  mrc_timerStart(timer, 1000, 1000, http_logoc, 1);
  for (i = 0; i < 1; i++) {
    mrc_clearScreen(30, 30, 30);
    mrc_drawText("1 initNetWork", 10, 10, 240, 240, 240, 0, 1);
    mrc_drawText("2 getHostByName", 10, 40, 240, 240, 240, 0, 1);
    mrc_drawText("3 socket", 10, 70, 240, 240, 240, 0, 1);
    mrc_drawText("4 connect", 10, 100, 240, 240, 240, 0, 1);
    mrc_drawText("5 send", 10, 130, 240, 240, 240, 0, 1);
    mrc_drawText("6 recv", 10, 160, 240, 240, 240, 0, 1);
    mrc_drawText("7 getSocketState", 10, 190, 240, 240, 240, 0, 1);
    mrc_drawText("8 test http get ", 10, 210, 240, 240, 240, 0, 1);

    mrc_refreshScreen(0, 0, 240, 320);
    // mrc_sleep(100);
  }

  net_init_type = mrc_initNetwork(netWorkCb, "cmnet");

  return 0;
}

int32 netWorkCb(int32 result) {
  net_init_type = result;
  mrc_printf("init network cb %d", result);
  return 0;
}

int32 getHostCb(int32 data) {
  char temp[300];
  mrc_sprintf(temp, "gethost cb %d", data);
  ip = data;
  mrc_drawText(temp, 10, 200, 240, 240, 240, 0, 1);
  mrc_refreshScreen(0, 0, 240, 320);
  return 0;
}

//判断是否已将头信息读取完
int32 isReadHeadSuccess(char *buf) {
  return mrc_strstr(buf, "\r\n\r\n") != NULL;
}

int32 onHttpCb(int32 ip) {
  int32 con_state = 0;
  mrc_printf("onHttpCb %d",ip);
  // ip = local_ip;
  print_ip(ip);
  socket = mrc_socket(MR_SOCK_STREAM, MR_IPPROTO_TCP);
  if (socket == MR_FAILED) {
    drawInfo("socket创建失败");
  } else {
    con_state = mrc_connect(socket, ip, 80, MR_SOCKET_BLOCK);
    if (con_state == MR_FAILED) {
      drawInfo("connect连接失败");
    } else if (con_state == MR_SUCCESS) {
      drawInfo("connect连接成功");
      socket_state = 2;
    } else if (con_state == MR_WAITING) {  //轮询
      socket_state = 1;
    }
  }
  //发送socket

  //接受数据

  //
  return 0;
}

//通过url获取host 需free
char *getUrlHost(char *url) {
  int len = mrc_strlen(url);
  char *host = mrc_malloc(len);
  int i = 0;
  int type = 0;
  int start = 0;
  int end = len;
  mrc_memset(host, '\0', len);
  while (url[i]) {
    switch (type) {
      case 0:
        if (url[i] == ':') {
          type = 1;
        }
        break;
      case 1:
        if (url[i] == '/') {
          type = 2;
        } else {
          goto out_host;
        }
        break;
      case 2:
        if (url[i] == '/') {
          start = i + 1;
          type = 3;
        } else {
          goto out_host;
        }
        break;
      case 3:
        if (url[i] == '/') {
          end = i;
        }
        break;
      default:
        break;
    }

    i++;
  }
out_host:
  if (start > 0 && end > 0) {
    mrc_memcpy(host, url + start, end - start);
    return host;
  }
  mrc_free(host);
  return NULL;
}

//通过url获取路由 需free
char *getUrlRoad(char *url) {
  int len = mrc_strlen(url);
  char *host = mrc_malloc(len);
  int i = 0;
  int type = 0;
  int start = 0;
  int end = len;
  mrc_memset(host, '\0', len);
  while (url[i]) {
    switch (type) {
      case 0:
        if (url[i] == ':') {
          type = 1;
        }
        break;
      case 1:
        if (url[i] == '/') {
          type = 2;
        } else {
          goto out_road;
        }
        break;
      case 2:
        if (url[i] == '/') {
          type = 3;
        } else {
          goto out_road;
        }
        break;
      case 3:
        if (url[i] == '/') {
          start = i;
          type = 4;
          goto out_road;
        }
        break;
      case 4:

        break;
      default:
        break;
    }

    i++;
  }
out_road:
  if (start > 0 && end > 0) {
    mrc_memcpy(host, url + start, len);
    return host;
  }
  mrc_free(host);
  return NULL;
}

//获取ip地址 并回调
int32 getHost(char *url, MR_GET_HOST_CB cb) {
  int32 re = 0;
  char *host = getUrlHost(url);
  char *road = getUrlRoad(url);
  re = mrc_getHostByName((const char *)host, cb);
  send_buf = (char *)mrc_malloc(255);
  mrc_printf("getHost 1");
  mrc_memset(send_buf, '\0', 255);
  mrc_printf("host=%s road=%s",host,road);
  mrc_sprintf(send_buf, "GET %s HTTP/1.1\r\nHost:%s\r\n\r\n\r\n", road, host);
  mrc_free(road);
  mrc_free(host);
  mrc_printf("getHost 2");
  if (re == MR_FAILED) {
	  drawInfo("getHostByName失败");
    return MR_FAILED;
  } else if (re == MR_WAITING) {
	  drawInfo("getHostByName回调");
    return 0;
  } else {  //直接获取ip
  mrc_printf("getHost 3");
    cb(re);
  }
  return 0;
}

//获取http数据
void getHttpData(char *url) {
  if (net_init_type != MR_SUCCESS) {
    drawInfo("net未初始化成功");
  } else {
    getHost(url, onHttpCb);
  }
}

void print_ip(int32 ip) {
  unsigned char *data = (unsigned char *)&ip;
  mrc_printf("ip:%d.%d.%d.%d", data[3], data[2], data[1], data[0]);
}

int32 mrc_event(int32 code, int32 param0, int32 param1) {
  int32 state = 0;
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
      net_init_type = mrc_initNetwork(netWorkCb, "cmnet");
    } else if (param0 == MR_KEY_2) {
      mrc_getHostByName("app.yzjlb.net", getHostCb);
      // mrc_getHostByName("app.yzjlb.net", NULL);
    } else if (param0 == MR_KEY_3) {  // socket
      socket = mrc_socket(MR_SOCK_STREAM, MR_IPPROTO_TCP);
    } else if (param0 == MR_KEY_4) {
      mrc_connect(socket, ip, 80, MR_SOCKET_BLOCK);
    } else if (param0 == MR_KEY_5) {  // send
      mrc_sprintf(temp, "test\n");
      mrc_send(socket, (const char *)&temp, 1);
    } else if (param0 == MR_KEY_6) {  // recev
      mrc_recv(socket, NULL, 1);
    } else if (param0 == MR_KEY_7) {
      state = mrc_getSocketState(socket);
    } else if (param0 == MR_KEY_8) {
		getHttpData("http://app.yzjlb.net/index.php");
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
  mrc_timerDelete(timer);
  if (buffer_http != NULL) {
    buffer_free(buffer_http);
    buffer_http = NULL;
  }

  mrc_closeNetwork();
  if (send_buf != NULL) {
    mrc_free(send_buf);
    send_buf = NULL;
  }
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