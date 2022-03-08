#include <tcc.h>
#include <mrc_exb.h>


int32 l_exb_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_sand", (void*)mrc_sand);
	tcc_add_symbol(s,"mrc_rand", (void*)mrc_rand);
	tcc_add_symbol(s,"mrc_LCDCanSleep", (void*)mrc_LCDCanSleep);
	tcc_add_symbol(s,"mrc_GetDiskFreeSpace32", (void*)mrc_GetDiskFreeSpace32);
	tcc_add_symbol(s,"mrc_strrchr", (void*)mrc_strrchr);
	tcc_add_symbol(s,"mrc_GetMrpInfo", (void*)mrc_GetMrpInfo);
	tcc_add_symbol(s,"mrc_tell", (void*)mrc_tell);
	tcc_add_symbol(s,"mrc_ntohl", (void*)mrc_ntohl);
	tcc_add_symbol(s,"mrc_BCDEncode", (void*)mrc_BCDEncode);
	tcc_add_symbol(s,"mrc_BCDDecode", (void*)mrc_BCDDecode);
	tcc_add_symbol(s,"mrc_getPackNameMr", (void*)mrc_getPackNameMr);
	tcc_add_symbol(s,"mrc_setPackNameMr", (void*)mrc_setPackNameMr);
	tcc_add_symbol(s,"GetCurLanguageID", (void*)GetCurLanguageID);
	tcc_add_symbol(s,"md5MakeFromMem", (void*)md5MakeFromMem);
	tcc_add_symbol(s,"md5MakeFromFileHandle", (void*)md5MakeFromFileHandle);
	tcc_add_symbol(s,"md5MakeFromFile", (void*)md5MakeFromFile);
	tcc_add_symbol(s,"md5ToString", (void*)md5ToString);
	tcc_add_symbol(s,"mrc_motionSensorPowerOn", (void*)mrc_motionSensorPowerOn);
	tcc_add_symbol(s,"mrc_motionSensorPowerOff", (void*)mrc_motionSensorPowerOff);
	tcc_add_symbol(s,"mrc_motionSensorStopListen", (void*)mrc_motionSensorStopListen);
	tcc_add_symbol(s,"mrc_motionSensorStartShakeListen", (void*)mrc_motionSensorStartShakeListen);
	tcc_add_symbol(s,"mrc_motionSensorStartTiltListen", (void*)mrc_motionSensorStartTiltListen);
	tcc_add_symbol(s,"mrc_motionSensorGetRange", (void*)mrc_motionSensorGetRange);
	tcc_add_symbol(s,"mrc_unicodeToGb2312", (void*)mrc_unicodeToGb2312);
	tcc_add_symbol(s,"mrc_voiceRecStart", (void*)mrc_voiceRecStart);
	tcc_add_symbol(s,"mrc_voiceRecStreamProc", (void*)mrc_voiceRecStreamProc);
	tcc_add_symbol(s,"mrc_voiceRecPause", (void*)mrc_voiceRecPause);
	tcc_add_symbol(s,"mrc_voiceRecResume", (void*)mrc_voiceRecResume);
	tcc_add_symbol(s,"mrc_voiceRecStop", (void*)mrc_voiceRecStop);
	tcc_add_symbol(s,"mrc_isTouchSupport", (void*)mrc_isTouchSupport);
	tcc_add_symbol(s,"mrc_writeBigEndianU32", (void*)mrc_writeBigEndianU32);
	tcc_add_symbol(s,"mrc_writeBigEndianU16", (void*)mrc_writeBigEndianU16);
	tcc_add_symbol(s,"readBigEndianUint32", (void*)readBigEndianUint32);
	tcc_add_symbol(s,"readBigEndianUint16", (void*)readBigEndianUint16);
	tcc_add_symbol(s,"readLittleEndianUint32", (void*)readLittleEndianUint32);
	tcc_add_symbol(s,"readLittleEndianUint16", (void*)readLittleEndianUint16);
	tcc_add_symbol(s,"mrc_getphoneInfo", (void*)mrc_getphoneInfo);
	tcc_add_symbol(s,"mrc_getSdkVer", (void*)mrc_getSdkVer);

	return 0;
}