#ifndef MRC_HTTP_H
#define MRC_HTTP_H

#include "mrc_base.h"
#define HTTP_BUF_LEN 1024 * 4

typedef void (*HTTP_OVER_CB)(); // �ص�����
typedef struct _HTTP
{
    int32 socket;       //������Ϣ
    int32 timer;
    int32 connect_ok;
    int32 usedTime;

    int32 ip;           //��ַ��Ϣ
    uint16 port;
    char hostName[128];
    int32 host_Len;
    char httpHead[256];

    int32 recv_len;     //������Ϣ
    int32 head_len;
    int32 data_len;
    char *data;
    HTTP_OVER_CB cb;
} HTTP;

#include "mrc_http.h"
// ��ǰhttp
extern HTTP *_http;

//�ڲ��������������
void _http_error(void);
int32 _get_host_ip_cb(int32 ip);
int32 _init_network_cb(int32 result);
void _http_connect_cb(int32 data);
void _http_receive_cb(int32 data);

//�ӿں���

//��ʼ��������Ϣ����ʼ���������������
extern void mrc_http_get_init(char *url, HTTP *http);
//�������ݻص�����
extern void mrc_http_set_recvcb(HTTP *http, HTTP_OVER_CB cb);
//��ʼ���ӷ�����
extern void mrc_http_connect(HTTP *http);
//����get����
extern int32 mrc_http_send(HTTP *http);
//�ͷ�http����
extern void mrc_http_free(HTTP *http);

#endif
