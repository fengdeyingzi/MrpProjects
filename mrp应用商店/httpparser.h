#ifndef __HTTP_H__
#define __HTTP_H__
#include <mrc_base.h>

typedef void (*HTTP_ONPROGRESS)(int32 pos);
typedef void (*HTTP_ONSUCCESS)(int32 ip,char *data, char *body);
typedef void (*HTTP_ONERROR)(int32 code);
typedef void (*test_test)(int32 data);
//数据缓冲区大小
#define BUFFER_MAX 1024*20

enum{
    HTTP_ERROR_NETINIT,
  HTTP_ERROR_HOST,
  HTTP_ERROR_SOCKET, //socket创建失败
  HTTP_ERROR_CONNECT,
  HTTP_ERROR_SEND,
  HTTP_ERROR_RECV
};

enum{
    HTTP_PROGRESS_CONNECT = -2,
    HTTP_PROGRESS_SEND = -1,
    HTTP_PROGRESS_HEAD = 0
};

void http_init(void);
void getHttpData(char *url, HTTP_ONPROGRESS onprogress, HTTP_ONSUCCESS onsuccess, HTTP_ONERROR onerror);
void http_exit(void);

#endif
