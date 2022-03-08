#include <tcc.h>
#include <mrc_sound.h>


int32 l_sound_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_playSoundExInit", (void*)mrc_playSoundExInit);
	tcc_add_symbol(s,"mrc_playSoundExLoad", (void*)mrc_playSoundExLoad);
	tcc_add_symbol(s,"mrc_playSoundExLoadFile", (void*)mrc_playSoundExLoadFile);
	tcc_add_symbol(s,"mrc_playSoundEx", (void*)mrc_playSoundEx);
	tcc_add_symbol(s,"mrc_pauseSoundEx", (void*)mrc_pauseSoundEx);
	tcc_add_symbol(s,"mrc_resumeSoundEx", (void*)mrc_resumeSoundEx);
	tcc_add_symbol(s,"mrc_stopSoundEx", (void*)mrc_stopSoundEx);
	tcc_add_symbol(s,"mrc_closeSoundEx", (void*)mrc_closeSoundEx);
	tcc_add_symbol(s,"mrc_setVolume", (void*)mrc_setVolume);
	tcc_add_symbol(s,"mrc_getSoundTotalTime", (void*)mrc_getSoundTotalTime);
	tcc_add_symbol(s,"mrc_getSoundCurTime", (void*)mrc_getSoundCurTime);
	tcc_add_symbol(s,"mrc_getSoundCurTimeMs", (void*)mrc_getSoundCurTimeMs);

	return 0;
}