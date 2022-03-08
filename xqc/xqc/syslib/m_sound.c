#include "haning.h"
#include "mrc_sound.h"


void Lib_SoundInit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_playSoundExInit(Param[0]->Val->LongInteger);
}

void Lib_SoundLoadFile(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_playSoundExLoadFile(Param[0]->Val->Integer,Param[1]->Val->Pointer);
}

void Lib_SoundPlay(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_playSoundEx(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,NULL);
}
void Lib_SoundPause(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_pauseSoundEx(Param[0]->Val->Integer);
}

void Lib_SoundResume(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_resumeSoundEx(Param[0]->Val->Integer);
}

void Lib_SoundStop(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_stopSoundEx(Param[0]->Val->Integer);
}

void Lib_SoundClose(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_closeSoundEx(Param[0]->Val->Integer);
}

void Lib_SetVolume(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_setVolume(Param[0]->Val->Integer);
}

void Lib_GetSoundTotalTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_getSoundTotalTime(Param[0]->Val->Integer,Param[1]->Val->Pointer);
}

void Lib_GetSoundCurTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_getSoundCurTime(Param[0]->Val->Integer,Param[1]->Val->Pointer);
}

void Lib_GetSoundCurTimeMs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_getSoundCurTimeMs(Param[0]->Val->Integer,Param[1]->Val->Pointer);
}

void Lib_SetPlayPos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
//    ReturnValue->Val->Integer = mrc_setPlayPos(Param[0]->Val->Integer,*(T_DSM_AUDIO_POS*)Param[1]->Val->Pointer);
}

void Lib_SetPlayTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
//    ReturnValue->Val->Integer = mrc_setPlayTime(Param[0]->Val->Integer,*(T_DSM_AUDIO_POS*)Param[1]->Val->Pointer);
}

void Lib_GetDeviceState(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
//    ReturnValue->Val->Integer = mrc_getDeviceState(Param[0]->Val->Integer);
}

#define MAX_SOUND_COUNT 16

/* 所有的库函数和他们的原型列表 */
struct LibraryFunction Sound_Functions[MAX_SOUND_COUNT];

int32 sound_init()
{
	Haninit(NULL);

	AddString(Lib_SoundInit,"int soundinit(int);");
	AddString(Lib_SoundLoadFile,"int soundloadfile(int,char*);");
	AddString(Lib_SoundPlay,"int soundplay(int,int,int);");
	AddString(Lib_SoundPause,"int soundpause(int);");
	AddString(Lib_SoundResume,"int soundresume(int);");
	AddString(Lib_SoundStop,"int soundstop(int);");
	AddString(Lib_SoundClose,"int soundclose(int);");
	AddString(Lib_SetVolume,"int setvolume(int);");
	AddString(Lib_GetSoundTotalTime,"int getsoundtotaltime(int,uint8**);");
	AddString(Lib_GetSoundCurTime,"int getsoundcurtime(int,uint8**);");
	AddString(Lib_GetSoundCurTimeMs,"int getsoundcurtimems(int,uint8**);");
	AddString(Lib_SetPlayPos,"int setplaypos(int,void*);");
	AddString(Lib_SetPlayTime,"int setplaytime(int,void*);");
	AddString(Lib_GetDeviceState,"int getdevicestate(int);");

	return hanclose();

}
