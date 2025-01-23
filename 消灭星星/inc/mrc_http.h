#ifndef MRC_HTTP_H
#define MRC_HTTP_H

#include "mrc_base.h"
#define HTTP_BUF_LEN 1024 * 4

typedef void (*HTTP_OVER_CB)(); // 回调函数
typedef struct _HTTP
{
    int32 socket;       //连接信息
    int32 timer;
    int32 connect_ok;
    int32 usedTime;

    int32 ip;           //地址信息
    uint16 port;
    char hostName[128];
    int32 host_Len;
    char httpHead[256];

    int32 recv_len;     //数据信息
    int32 head_len;
    int32 data_len;
    char *data;
    HTTP_OVER_CB cb;
} HTTP;

#include "mrc_http.h"
// 当前http
extern HTTP *_http;

//内部函数，请勿调用
void _http_error(void);
int32 _get_host_ip_cb(int32 ip);
int32 _init_network_cb(int32 result);
void _http_connect_cb(int32 data);
void _http_receive_cb(int32 data);

//接口函数

//初始化连接信息，初始化变量，必须调用
extern void mrc_http_get_init(char *url, HTTP *http);
//设置数据回调函数
extern void mrc_http_set_recvcb(HTTP *http, HTTP_OVER_CB cb);
//开始连接服务器
extern void mrc_http_connect(HTTP *http);
//发送get数据
extern int32 mrc_http_send(HTTP *http);
//释放http数据
extern void mrc_http_free(HTTP *http);

#endif
