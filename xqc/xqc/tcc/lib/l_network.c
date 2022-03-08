#include <tcc.h>
#include <mrc_network.h >


int32 l_network_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_GPRS_GetStatus", (void*)mrc_GPRS_GetStatus);
	tcc_add_symbol(s,"mrc_GPRS_GetMode", (void*)mrc_GPRS_GetMode);
	tcc_add_symbol(s,"mrc_GPRS_Connect", (void*)mrc_GPRS_Connect);
	tcc_add_symbol(s,"mrc_GPRS_DisConnect", (void*)mrc_GPRS_DisConnect);
	tcc_add_symbol(s,"mrc_Socket_Initialize", (void*)mrc_Socket_Initialize);
	tcc_add_symbol(s,"mrc_Socket_Terminate", (void*)mrc_Socket_Terminate);
	tcc_add_symbol(s,"mrc_Socket_Create", (void*)mrc_Socket_Create);
	tcc_add_symbol(s,"mrc_Socket_Connect", (void*)mrc_Socket_Connect);
	tcc_add_symbol(s,"mrc_Socket_Close", (void*)mrc_Socket_Close);
	tcc_add_symbol(s,"mrc_Socket_Send", (void*)mrc_Socket_Send);
	tcc_add_symbol(s,"mrc_Socket_IsNull", (void*)mrc_Socket_IsNull);
	tcc_add_symbol(s,"mrc_Socket_IsConnected", (void*)mrc_Socket_IsConnected);
	tcc_add_symbol(s,"mrc_Socket_CloseAll", (void*)mrc_Socket_CloseAll);
	tcc_add_symbol(s,"mrc_Http_Initialize", (void*)mrc_Http_Initialize);
	tcc_add_symbol(s,"mrc_Http_Terminate", (void*)mrc_Http_Terminate);
	tcc_add_symbol(s,"mrc_Http_Open", (void*)mrc_Http_Open);
	tcc_add_symbol(s,"mrc_Http_Close", (void*)mrc_Http_Close);
	tcc_add_symbol(s,"mrc_Http_Get", (void*)mrc_Http_Get);
	tcc_add_symbol(s,"mrc_Http_Post", (void*)mrc_Http_Post);
	tcc_add_symbol(s,"mrc_Http_GetResponseCode", (void*)mrc_Http_GetResponseCode);
	tcc_add_symbol(s,"mrc_Http_GetResponseField", (void*)mrc_Http_GetResponseField);
	tcc_add_symbol(s,"mrc_Http_GetResponseHead", (void*)mrc_Http_GetResponseHead);
	tcc_add_symbol(s,"mrc_GetNetWorkVersion", (void*)mrc_GetNetWorkVersion);

	return 0;
}