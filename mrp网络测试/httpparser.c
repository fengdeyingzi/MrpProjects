#ifndef __HTTP__
#define __HTTP__
#include <mrc_base.h>
#include <mrc_base_i.h>

#include "buffer.h"
#include "httpparser.h"




int32 net_init_type;
char *send_buf = NULL;
int32 send_len = 0;
int local_ip = 0x7f000001;
int32 socket_state;  // socket状态 0 gethost 1 connect 2 send 3 recv 4 ok -1
int32 socket;  //一个socket连接
STRING_BUFFER *buffer_http;
HTTP_ONPROGRESS lis_progress;
HTTP_ONSUCCESS lis_onsuccess;
HTTP_ONERROR lis_onerror;
int32 timer_http;

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
  lis_progress(HTTP_PROGRESS_SEND);
    temp_int = mrc_send(socket, send_buf+send_len, mrc_strlen(send_buf)-send_len);
    if (temp_int >= 0) {
      send_len += temp_int;
      if (send_len == mrc_strlen(send_buf)) {
        socket_state = 3;
      }
	  mrc_sprintf(buf, "发送数据中 %d %d",socket, send_len);
    //   drawInfo(buf);
    
    } else {
    //   drawInfo("send发送失败");
    lis_onerror(HTTP_ERROR_SEND);
    }
  } else if (socket_state == 3) {  // recv
    
    temp_int = mrc_recv(socket, buf, sizeof(buf));
    if (temp_int >= 0) {
		
    // mrc_printf(buffer_http->data);
    
      buffer_appendx(buffer_http, buf, temp_int);
    //   mrc_sprintf(buf,"接收数据中 %d %d %d",socket,buffer_http->len,mrc_strlen(buffer_http->data));
    //   drawInfo(buf);
    
		//判断header是否完成
		if(mrc_strstr(buffer_http->data, "\r\n\r\n")){
            buffer_http->body = mrc_strstr(buffer_http->data, "\r\n\r\n")+4;
			//读取头完成
			temp_str = mrc_strstr(buffer_http->header, "Content-Length");
      mrc_printf("!=null");
			if(temp_str != NULL && (mrc_strstr(temp_str,"\r\n")!=NULL)){
        mrc_printf("getline_value begin");
				temp_str = getline_value(temp_str);
        mrc_printf("getline_value %s",temp_str);
				content_len = mrc_atoi(temp_str);
                buffer_http->body_len = content_len;
        mrc_printf("atoi %d",content_len);
				mrc_free(temp_str);
                
				temp_int = isContentSuccess(buffer_http->data,content_len);
				if(temp_int){
					mrc_closeSocket(socket);
					buffer_http->body = mrc_strstr(buffer_http->data, "\r\n\r\n");
					buffer_http->body += 4;
          mrc_printf("http获取成功 %s", buffer_http->body);
					// drawInfo("http获取成功");
                    lis_onsuccess(local_ip, buffer_http->data, buffer_http->body);
					// drawBody(buffer_http->body);
          socket_state = -1;
				}
                else{
                    lis_progress(100*((buffer_http->len - (buffer_http->body - buffer_http->data)))/buffer_http->body_len);
                    mrc_printf("headlen %d bodylen %d content_len %d",(buffer_http->body - buffer_http->data), (buffer_http->len - (buffer_http->body - buffer_http->data)), buffer_http->body_len );
                }
			}
		}
        else{
lis_progress(HTTP_PROGRESS_HEAD);
        }
    } else {
    //   drawInfo("接收数据失败");
      lis_onerror(HTTP_ERROR_RECV);
      socket_state = -1;
    }
  }
}

void print_ip(int32 ip) {
  unsigned char *data = (unsigned char *)&ip;
  mrc_printf("ip:%d.%d.%d.%d", data[3], data[2], data[1], data[0]);
}

int32 onHttpCb(int32 ip) {
  int32 con_state = 0;
  mrc_printf("onHttpCb %d",ip);
  // ip = local_ip;
  local_ip = ip;
  print_ip(ip);
  if(socket != 0){
      mrc_closeSocket(socket);
  }
  socket = mrc_socket(MR_SOCK_STREAM, MR_IPPROTO_TCP);
  if (socket == MR_FAILED) {
    // drawInfo("socket创建失败");
    lis_onerror(HTTP_ERROR_SOCKET);
  } else {
      lis_progress(HTTP_PROGRESS_CONNECT);
    con_state = mrc_connect(socket, ip, 80, MR_SOCKET_BLOCK);
    if (con_state == MR_FAILED) {
    //   drawInfo("connect连接失败");
    lis_onerror(HTTP_ERROR_CONNECT);
    } else if (con_state == MR_SUCCESS) {
    //   drawInfo("connect连接成功");
      lis_progress(HTTP_PROGRESS_SEND);
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
    if(i == len-1){
        start = -1;
        goto out_road;
    }
    i++;
  }
out_road:
  if (start > 0 && end > 0) {
    mrc_memcpy(host, url + start, len);
    return host;
  }
  else{
      mrc_memcpy(host,"/",1);
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
  
  mrc_printf("getHost 1");
  mrc_memset(send_buf, '\0', 255);
  mrc_printf("host=%s road=%s",host,road);
  mrc_sprintf(send_buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n\r\n", road, host);
  mrc_free(road);
  mrc_free(host);
  mrc_printf("getHost 2");
  if (re == MR_FAILED) {
	//   drawInfo("getHostByName失败");
    lis_onerror(HTTP_ERROR_HOST);
    return MR_FAILED;
  } else if (re == MR_WAITING) {
	//   drawInfo("getHostByName回调");
    return 0;
  } else {  //直接获取ip
  mrc_printf("getHost 3");
    cb(re);
  }
  return 0;
}

int32 netWorkCb(int32 result) {
  net_init_type = result;
  mrc_printf("init network cb %d", result);
  return 0;
}

//初始化
void http_init(void){
    socket = 0;
    net_init_type = mrc_initNetwork(netWorkCb, "cmnet");
    timer_http = mrc_timerCreate();
    mrc_timerStart(timer_http, 100, 666, http_logoc, 1);
    send_buf = (char *)mrc_malloc(255);
    buffer_http = buffer_create();
}

//获取http数据 参数：url 获取进度 加载成功 加载失败
//进度 -2 connect -1 send 0 接收head成功 0+ 接收body 
void getHttpData(char *url, HTTP_ONPROGRESS onprogress, HTTP_ONSUCCESS onsuccess, HTTP_ONERROR onerror){
    lis_progress = onprogress;
    lis_onsuccess = onsuccess;
    lis_onerror = onerror;
    send_len = 0;
     if (net_init_type != MR_SUCCESS) {
    // drawInfo("net未初始化成功");
    lis_onerror(HTTP_ERROR_NETINIT);
  } else {
    getHost(url, onHttpCb);
  }
}

//释放定时器 释放内存
void http_exit(void){
    if(timer_http){
        mrc_timerStop(timer_http);
        mrc_timerDelete(timer_http);
        timer_http = 0;
    }
    if(buffer_http){
        buffer_free(buffer_http);
        buffer_http = NULL;
    }
    if(send_buf){
        mrc_free(send_buf);
        send_buf = NULL;
    }
    mrc_closeNetwork();
}


#endif
