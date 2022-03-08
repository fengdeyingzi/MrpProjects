#include "base.h"
#include "net.h"

#define  MR_SOCK_STREAM 0 //������
#define  MR_SOCK_DGRAM  1 //���ݰ�
#define  MR_IPPROTO_TCP  0 //TCP
#define  MR_IPPROTO_UDP  1 //UDP

#define  MR_SOCKET_BLOCK   0       //������ʽ��ͬ����ʽ��
#define  MR_SOCKET_NONBLOCK   1    //��������ʽ���첽��ʽ��

#define BUFLEN 1024*4

int32 usedTime, recevLen, headLen,fileLen;//��ʱ�������������ݵ��ܳ��ȣ�HTTP��Ӧͷ���ȣ��ļ����ݳ���

char hostName[128];
char httpHead[256];
uint16 port;


int32 socketHandle, timerHandle, fileHandle;
char *url;
char *fileName;

char tempData[BUFLEN];

void StopDownload(void)
{
    if (timerHandle)
    {
        timerdel(timerHandle);
        timerHandle = 0;
    }

    if (socketHandle)
    {
        closesocket(socketHandle);
        socketHandle = 0;
    }
    if(fileHandle)
    {
        close(fileHandle);
        fileHandle=0;
    }
    closenetwork();
}
int yy;
void tishi(char *a)
{
	yy=yy>SCRH?0:yy;
	drect(0,yy*20,SCRW,20,0,0,0);
	dtext(a,0,yy*20,255,255,255,0,1);
	yy++;
	ref(0,0,240,320);

}
void ErrorStop(void)
{
    cls(0,0,0);
    dtext("���س���",0,0,255,255,255,0,1);
    ref(0,0,240,320);
    StopDownload();
}
void ReceiveCb(int32 data)//�������ݻص�
{
    int32 len;
    static char RecvBuffer[BUFLEN];

    len = recv(socketHandle, RecvBuffer, sizeof(RecvBuffer));
    if (0 == len)//�ȴ���������
    {
        usedTime++;
        if (usedTime > 200)
        {
            // �������ݳ�ʱ��
            ErrorStop();
        }
    }
    else if (MR_FAILED == len)
    {
        ErrorStop();
    }
    else
    {
        usedTime = 0;
        if (0 == headLen) //�ڽ�������ͷ
        {
            int respond;
            int templen;
            char *p;
            
            templen=recevLen+len;
            if(templen>sizeof(tempData)) 
            {
                ErrorStop();
                return;
            }
            memcpy(&tempData[recevLen],RecvBuffer,len);
            p=strstr(tempData,"\r\n\r\n");
            if (p!=NULL) //ͷ���������
            {
                char*temp=tempData;
                headLen = (int)p-(int)temp;
                headLen += 4;
                // HTTP/1.1 200 OK
                // Content-Length: 81
                respond = (int)strtoul(&tempData[9],NULL,10);//�����Ӧ��
                p = strstr(tempData,"Content-Length: ");//����ļ�����
                if (200 != respond || p==NULL)// ���س���
                {
                    ErrorStop();
                    return;
                }
                fileLen = (int)strtoul(p+strlen("Content-Length: "),NULL,10);
                write(fileHandle,&tempData[headLen],templen-headLen);
            }
        }
        else
        {
            write(fileHandle, RecvBuffer, len);
        }
        recevLen += len;

        if (headLen+fileLen == recevLen) //���ݽ�����
        {
            cls(0,0,0);
            dtext("�������",0,0,255,255,255,0,1);
            ref(0,0,240,320);
            StopDownload();
        }
    }
}

void httpSendGet(void)//������������
{
    int32 dataLen, sendLen = 0;

    dataLen = strlen(httpHead);
	tishi("��������");
    // �������󡭡�
    do{
        sendLen += send(socketHandle, &httpHead[sendLen], dataLen - sendLen);
    } while (sendLen < dataLen);
}


void ConnectCb(int32 data)//����״̬��ѯ�ص�
{
    int32 conState = getsocketstate(socketHandle);

    switch (conState)
    {
    case MR_WAITING:// ���ӷ�����        
        usedTime++;
        if (usedTime > 100)// ���ӷ�������ʱ
        {            
            ErrorStop();
        }
    	break;
    case MR_SUCCESS:
        timerstop(timerHandle);
        httpSendGet();
        usedTime = 0;
        timerstart(timerHandle, 100, 0, "ReceiveCb", 1);
        break;
    case MR_FAILED:
    case MR_IGNORE:
        ErrorStop();
        break;
    }
}

void Connect(int32 ip)//��ʼ����
{
    socketHandle = socket(MR_SOCK_STREAM, MR_IPPROTO_TCP); //����TCP Socket
    connect(socketHandle, ip, port, MR_SOCKET_NONBLOCK); //����Socket
    timerHandle = timercreate();
    timerstart(timerHandle, 200, 0, "ConnectCb", 1);
}

int cbGetHostIp(int32 ip)//����IP�ص�����
{
    if(ip!=-1)//��ȡ�ɹ�
        Connect(ip);
    else
        ErrorStop();
    return 0;
}

int cbInitNetwork(int32 result)  //��ʼ���ص�����
{
    switch(result)
    {
    case MR_SUCCESS:
		tishi("����������IP");
        cbGetHostIp(gethostbyname(hostName, "cbGetHostIp")); //����������IP
        break;
    case MR_FAILED:
        ErrorStop();
        break;
    }
    return 0;
}
void parseUrl(void)
{
    int32 hostNameLen;
    char *p;
    char *findPos;

    //http://xxx.xxx.xxx:xxxx/x.xxx
    p = findPos = (char*)strstr(&url[7], "/");//����http://�ҵ�һ��/
	tishi(p);


    //��ȡ�������Ͷ˿ں�
    p = (char*)strstr(&url[7], ":");
    if (findPos != NULL)
        hostNameLen = findPos - &url[7];
    else
        hostNameLen = strlen(url) - 7;
    if (p != NULL)//����ж˿ںţ����¼�������������
        hostNameLen = p - &url[7];
    strncat(hostName, &url[7], hostNameLen);

    if (p == NULL)
        port = 80;
    else
        port = strtoul(p+1, NULL, 10);

    //����HTTP����ͷ
    if (findPos == NULL)//����"http://"���������/���ʾ�Ǿ�����ļ�
        findPos="/";//������ҳ
    sprintf(httpHead, "GET %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nConnection: close\r\n\r\n", findPos, hostName);
    printf(httpHead);
	tishi(httpHead);

}

void StartDownLoad(void)//��ʼ���أ���ʼ�����磩
{
    usedTime = 0;
    recevLen = 0;
    fileLen = 0;
    remove(fileName);
    fileHandle = open(fileName, 12);
    if(!fileHandle) return;
    parseUrl();
	 tishi("��ʼ���أ���ʼ�����磩");
	 int a=initnetwork("cbInitNetwork", "cmnet");
	 tishi("���ú���ָ��");
    cbInitNetwork(a);
}

int init()
{
     url="http://wap.baidu.com/logo.gif";
	 tishi(url);
     fileName="logo1.gif";
	 tishi(fileName);


//     url="http://img1.pengfu.cn/big/197/407197.gif";
//     fileName="407197.gif";

//    url="http://wap.baidu.com";
//    fileName="baidu.txt";

    StartDownLoad();
    return MR_SUCCESS;
}


int event(int32 code, int32 param0, int32 param1)
{
	exit();
	return MR_SUCCESS;
}

int pause()
{
	return 0;
}

int resume()
{
	return 0;
}
