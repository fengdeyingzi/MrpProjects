#ifndef __SOUND_DEFINED__
#define __SOUND_DEFINED__
#include <mrc_base.h>


//加载音频文件 返回音频文件id
int32 soundLoad(char *filename);

//播放音频文件
void soundPlay(int32 id, int32 isloop);

//停止播放
void soundStop(int32 id);

//释放音频文件
void soundFree(int32 id);

#endif