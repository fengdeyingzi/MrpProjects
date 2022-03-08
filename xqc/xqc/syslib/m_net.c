#include "haning.h"
#include "ext.h"
#include "mrc_network.h"

char getHostCB[32];
char initNetworkCB[32];
char Source[256];

int32 GET_HOST_CB(int32 ip)
{
	FuncRet=ip;//FuncRet是内部符号_ret
    if(CheckBaseCall(getHostCB,&IntType,1))//检查函数是否符合要求
    {
        ProgramFail(NULL, "无法使用%s",getHostCB);
    }
    mrc_sprintf(Source,"_ret=%s(_ret);",getHostCB);
    PicocParse(" ", Source, strlen(Source), TRUE, TRUE, FALSE);
	return FuncRet;
}

void Lib_GetHostByName(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_strcpy(getHostCB,Param[1]->Val->Pointer);
    ReturnValue->Val->LongInteger = mrc_getHostByName(Param[0]->Val->Pointer, GET_HOST_CB);
}

int32 INIT_NETWORK_CB(int32 result)
{
	FuncRet=result;//FuncRet是内部符号_ret
    if(CheckBaseCall(initNetworkCB,&IntType,1))//检查函数是否符合要求
    {
        ProgramFail(NULL, "无法使用%s",initNetworkCB);
    }
    mrc_sprintf(Source,"_ret=%s(_ret);",initNetworkCB);
    PicocParse(" ", Source, strlen(Source), TRUE, TRUE, FALSE);
    return FuncRet;
}
void Lib_InitNetwork(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	mrc_strcpy(initNetworkCB,Param[0]->Val->Pointer);
    ReturnValue->Val->LongInteger = mrc_initNetwork(INIT_NETWORK_CB, Param[1]->Val->Pointer);
}

void Lib_GetNetworkID(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_getNetworkID();
}

void Lib_CloseNetwork(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger =mrc_closeNetwork();
}

void Lib_Socket(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_socket(Param[0]->Val->LongInteger, Param[1]->Val->LongInteger);
}
void Lib_Connect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_connect(Param[0]->Val->LongInteger, Param[1]->Val->LongInteger,
        Param[2]->Val->UnsignedShortInteger, Param[3]->Val->LongInteger);
}
void Lib_GetSocketState(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_getSocketState(Param[0]->Val->LongInteger);
}
void Lib_CloseSocket(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_closeSocket(Param[0]->Val->LongInteger);
}

void Lib_Recv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_recv(Param[0]->Val->LongInteger, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}
void Lib_Send(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_send(Param[0]->Val->LongInteger, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void Lib_Recvfrom(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_recvfrom(Param[0]->Val->LongInteger, Param[1]->Val->Pointer, Param[2]->Val->Integer
        ,Param[3]->Val->Pointer, Param[4]->Val->Pointer);
}
void Lib_Sendto(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_sendto(Param[0]->Val->LongInteger, Param[1]->Val->Pointer, Param[2]->Val->Integer
        ,Param[3]->Val->LongInteger, Param[4]->Val->UnsignedShortInteger);
}

#define  MAX_NET_COUNT 13
struct LibraryFunction mrc_net_Functions[MAX_NET_COUNT];

int32 net_init()
{
	Haninit(NULL);

	AddString(Lib_InitNetwork,"int initnetwork(char*,char*);");

    AddString(Lib_Recv,"int recv(int,char*,int);");

    AddString(Lib_Send,"int send(int,char*,int);");

	AddString(Lib_Sendto,"int sendto(int,char*,int,int,uint16);");

	AddString(Lib_Connect,"int connect(int,int,int,int);");

    AddString(Lib_GetSocketState,"int getsocketstate(int);");

    AddString(Lib_CloseSocket,"int closesocket(int);");

    AddString(Lib_GetNetworkID,"int getnetworkid();");

    AddString(Lib_Recvfrom,"int recvfrom(int,char*,int,int*,uint16*);");

    AddString(Lib_Socket,"int socket(int,int);");

    AddString(Lib_CloseNetwork,"int closenetwork();");

    AddString(Lib_GetHostByName,"int gethostbyname(char*,char*);");


	return hanclose();

}
