
#include <mrc_base.h>
#include "mpc.h"
#include "mrc_graphics.h"
#include "mrc_android.h"

typedef struct {
 void* buf;
 int32 buflen; //���������
 int type;
} SOUND_DATA;


//������Ƶ�ļ� ֻ֧��mid/wav/mp3 ������Ƶ�ļ�id
int32 soundLoad(char *filename){
    char *endName;
    int32 len;
    SOUND_DATA *soundData;
    endName = mrc_strrchr(filename, '.');
    if(endName == NULL) return -1;
    soundData = mrc_malloc(sizeof(SOUND_DATA));
    soundData->buf = mrc_readFileFromAssets(filename, &len);
    soundData->buflen = len;
    if(mrc_strcmp(endName,".mid") == 0){
        soundData->type = MR_SOUND_MIDI;
    }
    if(mrc_strcmp(endName,".wav") == 0){
        soundData->type = MR_SOUND_WAV;
    }
    if(mrc_strcmp(endName,".mp3") == 0){
        soundData->type = MR_SOUND_MP3;
    }
    return (int32)soundData;
}

//������Ƶ�ļ�
void soundPlay(int32 id, int32 isloop){
    SOUND_DATA *soundData;
    if(id == -1) return;
    soundData = (SOUND_DATA*)id;
    if(soundData == NULL) return;
    mr_playSoundEx(soundData->type, soundData->buf, soundData->buflen,  isloop);
}

void soundStop(int32 id){
    SOUND_DATA *soundData;
    if(id == -1) return;
    soundData = (SOUND_DATA*)id;
    if(soundData == NULL) return;
    mr_stopSoundEx(soundData->type);
}

//�ͷ���Ƶ�ļ�
void soundFree(int32 id){
    SOUND_DATA *soundData;
    if(id == -1) return;
    soundData = (SOUND_DATA*)id;
    if(soundData == NULL) return;
    mrc_freeFileFromAssets(soundData->buf, soundData->buflen);
    mrc_free(soundData);
}


