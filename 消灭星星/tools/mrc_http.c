#include "mrc_http.h"
// ��ǰhttp
HTTP *_http;

void _http_error()
{
}

int32 _get_host_ip_cb(int32 ip) // ����������IP�ص�����
{
    mrc_printf("get_host_ip_cb %d\n", ip);
    if (ip != -1) // ��ȡ�ɹ�
    {
        if (ip != 2)
        {
            _http->ip = ip;
        }
    }
    return 0;
}

int32 _init_network_cb(int32 result) // ���粦�Żص�����
{
    int32 gethostre;
    mrc_printf("init_network_cb %d\n", result);
    switch (result)
    {
    case MR_SUCCESS:
        gethostre = mrc_getHostByName(_http->hostName, _get_host_ip_cb); // ����������IP
        if (gethostre == MR_FAILED)
        {
            // lis_onerror(HTTP_ERROR_HOST);
            return MR_FAILED;
        }
        else if (gethostre == MR_WAITING)
        {
            //   drawInfo("getHostByName�ص�");
            return 0;
        }
        else
        { // ֱ�ӻ�ȡip
            mrc_printf("getHost 3");
            _get_host_ip_cb(gethostre);
        }
        break;
    case MR_FAILED:
        break;
    }
    return 0;
}

void _http_connect_cb(int32 data) // ����״̬�ص���ѯ
{
    HTTP *http = (HTTP *)data;
    http->connect_ok = mrc_getSocketState(http->socket);
    mrc_printf("http_connect_cb:%d", http->connect_ok);
    switch (http->connect_ok)
    {
    case MR_WAITING: // ���ӷ�����
        http->usedTime++;
        if (http->usedTime > 100) // ���ӷ�������ʱ
        {
            _http_error();
        }
        break;

    case MR_SUCCESS:
        mrc_timerStop(http->timer);
        http->usedTime = 0;
        break;

    case MR_FAILED:
    case MR_IGNORE:
        mrc_printf("http connect failed\n");
        break;
    }
}

void mrc_http_connect(HTTP *http) // ��ʼ����
{
    int32 con_state;
    http->socket = mrc_socket(MR_SOCK_STREAM, MR_IPPROTO_TCP); // ����TCP Socket
    mrc_printf("mrc_connect socket %d", http->ip);
    con_state = mrc_connect(http->socket, http->ip, http->port, MR_SOCKET_NONBLOCK); // ����Socket
    
     if (con_state == MR_FAILED) {
    // socket_state = -1;
    // lis_onerror(HTTP_ERROR_CONNECT);
    } else if (con_state == MR_SUCCESS) {
    //   drawInfo("connect���ӳɹ�");
    //   lis_progress(HTTP_PROGRESS_SEND);
    //   socket_state = 2;
    mrc_http_send(http);
    } else if (con_state == MR_WAITING) {  //��ѯ
    //   socket_state = 1;
    http->timer = mrc_timerCreate();
      mrc_timerStart(http->timer, 200, (int32)http, _http_connect_cb, TRUE);
    }
    
    
}

void mrc_http_get_init(char *url, HTTP *http)
{
    char *p;
    char *findPos;
    int32 netre;
    _http = http;

    p = findPos = (char *)strstr(url + 7, "/"); // ����http://�ҵ�һ��/

    // ��ȡ�������Ͷ˿ں�
    p = (char *)strstr(url + 7, ":");
    if (findPos != NULL)
        http->host_Len = findPos - url - 7;
    else
        http->host_Len = strlen(url) - 7;
    if (p != NULL) // ����ж˿ںţ����¼�������������
        http->host_Len = p - url - 7;
    mrc_memset(http->hostName, 0, 128);
    mrc_strncat(http->hostName, url + 7, http->host_Len);

    if (p == NULL)
        http->port = 80;
    else
        http->port = strtoul(p + 1, NULL, 10);

    // ����HTTP����ͷ
    if (findPos == NULL) // ����"http://"���������/���ʾ�Ǿ�����ļ�
        findPos = "/";   // ������ҳ

    sprintf(http->httpHead, "GET %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nConnection: close\r\n\r\n", findPos, http->hostName);
    mrc_printf(http->httpHead);

    http->ip = 0;
    http->timer = 0;
    http->socket = 0;
    http->connect_ok = MR_FAILED;

    http->head_len = 0;
    http->recv_len = 0;
    http->data_len = 0;

    http->data = NULL;

    netre = mrc_initNetwork(_init_network_cb, "cmnet");
    if (netre == MR_SUCCESS)
    {
        _init_network_cb(netre);
    }
}

void _http_receive_cb(int32 data) // �������ݻص�
{
    int32 len;
    HTTP *http = (HTTP *)data;
    static char RecvBuffer[HTTP_BUF_LEN];

    len = mrc_recv(http->socket, RecvBuffer, sizeof(RecvBuffer));
    mrc_printf("http_receive_cb:%d", len);
    if (0 == len) // �ȴ���������
    {
        http->usedTime++;
        if (http->usedTime > 200)
        {
            _http_error(); // �������ݳ�ʱ��
        }
    }
    else if (MR_FAILED == len)
    {
        _http_error();
    }
    else
    {
        http->usedTime = 0;
        http->recv_len += len;

        if (0 == http->head_len) // �ڽ�������ͷ
        {
            int respond;
            char *p;

            p = mrc_strstr(RecvBuffer, "\r\n\r\n");
            if (p != NULL) // ͷ���������
            {
                char *temp = RecvBuffer;
                http->head_len = (int)p - (int)temp;
                http->head_len += 4;

                respond = (int)mrc_strtoul(&RecvBuffer[9], NULL, 10); // �����Ӧ��
                if (200 != respond)                                   // ���س���
                {
                    _http_error();
                    return;
                }

                p = mrc_strstr(RecvBuffer, "Content-Length: "); // ����ļ�����
                if (p == NULL)
                {
                    p = mrc_strstr(&RecvBuffer[http->head_len], "\r\n\r\n");
                    if (p)
                    {
                        http->data_len = (int)p - (int)temp;
                        http->data_len = http->data_len - http->head_len + 4;
                    }
                }
                else
                {
                    http->data_len = (int)mrc_strtoul(p + mrc_strlen("Content-Length: "), NULL, 10);
                }
                http->data = (char *)malloc(sizeof(char) * http->data_len);
                mrc_memcpy(http->data, &RecvBuffer[http->head_len], http->recv_len - http->head_len);
            }
        }
        else
        {
            mrc_memcpy(http->data + http->recv_len, RecvBuffer, len);
        }

        if (http->head_len + http->data_len == http->recv_len) // ���ݽ�����
        {
            if (http->cb != NULL)
                http->cb();
            mrc_timerStop(http->timer);
            http->usedTime = 0;
        }
    }
}

void mrc_http_set_recvcb(HTTP *http, HTTP_OVER_CB cb)
{
    http->cb = cb;
}

int32 mrc_http_send(HTTP *http)
{
    int32 dataLen, sendLen = 0;
    if (http->connect_ok == MR_SUCCESS)
    {
        dataLen = mrc_strlen(http->httpHead);
        do
        {
            sendLen += mrc_send(http->socket, &http->httpHead[sendLen], dataLen - sendLen);
        } while (sendLen < dataLen);

        mrc_timerStart(http->timer, 500, (int32)http, _http_receive_cb, TRUE);
        return MR_SUCCESS;
    }
    return http->connect_ok;
}

void mrc_http_free(HTTP *http)
{
    if (http->timer)
    {
        mrc_timerDelete(http->timer);
        http->timer = 0;
    }

    if (http->socket)
    {
        mrc_closeSocket(http->socket);
        http->socket = 0;
    }

    if (http->data)
    {
        mrc_free(http->data);
    }
    mrc_closeNetwork();
}