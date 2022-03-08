#include <tcc.h>
#include <mrc_base_i.h>


int32 l_base_i_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_sleep", (void*)mrc_sleep);
	tcc_add_symbol(s,"mrc_getSysInfo", (void*)mrc_getSysInfo);
	tcc_add_symbol(s,"mrc_sendSms", (void*)mrc_sendSms);
	tcc_add_symbol(s,"mrc_call", (void*)mrc_call);
	tcc_add_symbol(s,"mrc_connectWAP", (void*)mrc_connectWAP);
	tcc_add_symbol(s,"mrc_plat", (void*)mrc_plat);
	tcc_add_symbol(s,"mrc_platEx", (void*)mrc_platEx);
	tcc_add_symbol(s,"mrc_getFixMrp", (void*)mrc_getFixMrp);
	tcc_add_symbol(s,"mrc_getFixMrpByName", (void*)mrc_getFixMrpByName);
	tcc_add_symbol(s,"mrc_encode", (void*)mrc_encode);
	tcc_add_symbol(s,"mrc_setReloadFile", (void*)mrc_setReloadFile);
	tcc_add_symbol(s,"mrc_getReloadFile", (void*)mrc_getReloadFile);
	tcc_add_symbol(s,"mrc_loadSmsCfg", (void*)mrc_loadSmsCfg);
	tcc_add_symbol(s,"mrc_readSmsCfg", (void*)mrc_readSmsCfg);
	tcc_add_symbol(s,"mrc_setSmsCfg", (void*)mrc_setSmsCfg);
	tcc_add_symbol(s,"mrc_saveSmsCfg", (void*)mrc_saveSmsCfg);
	tcc_add_symbol(s,"mrc_getMrParam", (void*)mrc_getMrParam);
	tcc_add_symbol(s,"mrc_getMrEntry", (void*)mrc_getMrEntry);
	tcc_add_symbol(s,"mrc_malloc8", (void*)mrc_malloc8);
	tcc_add_symbol(s,"mrc_free8", (void*)mrc_free8);
	tcc_add_symbol(s,"mrc_exitImmediately", (void*)mrc_exitImmediately);
	tcc_add_symbol(s,"mrc_getAppInfo", (void*)mrc_getAppInfo);
	tcc_add_symbol(s,"mrc_isZip", (void*)mrc_isZip);
	tcc_add_symbol(s,"mrc_unzip", (void*)mrc_unzip);
	tcc_add_symbol(s,"mrc_unzipVM", (void*)mrc_unzipVM);
	tcc_add_symbol(s,"mrc_getExtGlobalMem", (void*)mrc_getExtGlobalMem);
	tcc_add_symbol(s,"mrc_checkMrp", (void*)mrc_checkMrp);
	tcc_add_symbol(s,"mrc_timerInit", (void*)mrc_timerInit);
	/*******************************C¥˙¬Îπ‹¿Ì**********************************/
	tcc_add_symbol(s,"mrc_extRecvAppEvent", (void*)mrc_extRecvAppEvent);
	tcc_add_symbol(s,"mrc_extRecvAppEventEx", (void*)mrc_extRecvAppEventEx);
	tcc_add_symbol(s,"mrc_extLoad", (void*)mrc_extLoad);
	tcc_add_symbol(s,"mrc_extFree", (void*)mrc_extFree);
	tcc_add_symbol(s,"mrc_extFreeTimer", (void*)mrc_extFreeTimer);
	tcc_add_symbol(s,"mrc_extSetMode", (void*)mrc_extSetMode);
	tcc_add_symbol(s,"mrc_extInit", (void*)mrc_extInit);
	tcc_add_symbol(s,"mrc_extEvent", (void*)mrc_extEvent);
	tcc_add_symbol(s,"mrc_extPause", (void*)mrc_extPause);
	tcc_add_symbol(s,"mrc_extResume", (void*)mrc_extResume);
	tcc_add_symbol(s,"mrc_extSendAppEvent", (void*)mrc_extSendAppEvent);
	tcc_add_symbol(s,"mrc_extSendAppEventEx", (void*)mrc_extSendAppEventEx);
	tcc_add_symbol(s,"mrc_extMpsFpCall", (void*)mrc_extMpsFpCall);
	tcc_add_symbol(s,"mrc_extInitMainModule", (void*)mrc_extInitMainModule);

	return 0;
}