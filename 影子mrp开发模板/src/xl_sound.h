#ifndef __SOUND_DEFINED__
#define __SOUND_DEFINED__
#include <mrc_base.h>


//������Ƶ�ļ� ������Ƶ�ļ�id
int32 soundLoad(char *filename);

//������Ƶ�ļ�
void soundPlay(int32 id, int32 isloop);

//ֹͣ����
void soundStop(int32 id);

//�ͷ���Ƶ�ļ�
void soundFree(int32 id);

#endif