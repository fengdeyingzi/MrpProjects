#ifndef _MRC_SOUND_H_
#define _MRC_SOUND_H_



#define ACI_MIDI_DEVICE 1
#define ACI_WAVE_DEVICE 2
#define ACI_MP3_DEVICE 3
#define ACI_AMR_DEVICE 4
#define ACI_PCM_DEVICE 5
#define ACI_M4A_DEVICE 6
#define ACI_AMR_WB_DEVICE 7


typedef void (*ACI_PLAY_CB)(int32 result);

typedef struct
{
    int32 pos;
}T_DSM_AUDIO_POS;

/*
	ÿ�β���ǰ�����
	[in]:�����豸����ACI_MIDI_DEVICE��ACI_AMR_WB_DEVICE
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_playSoundExInit(int32 type);
/*
	������Ƶ�ļ�
	ע��:�÷�����resumeʱ�ֻ��������⣬������ֲ�汾��������齫
	�ļ�д��T��Ȼ��mrc_playSoundExLoad����
	[in]
	type:�豸����
	data:��������
	buf_len:���ݳ���
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_playSoundExLoad(int32 type, const void * data, int32 buf_len);
/*
	������Ƶ�ļ�
	[in]
	type:�豸����
	filename:�����ļ����ƣ����·����GB���룬����ע����Ǵ�mrp�ⲿ��ȡ�ļ�
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_playSoundExLoadFile(int32 type, char* filename);
/*
	������Ƶ�ļ�
	[in]
	type:�豸����
	block:1������ʽ��ײ㷢�Ͳ�������0��������������ײ㷢�Ͳ�������
	loop:0�����β��ţ�1��ѭ�����ţ�2��nes pcm ���ŷ�ʽ
	cb:�ص�����  ,������NULL
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_playSoundEx(int32 type, int32 block, int32 loop, ACI_PLAY_CB cb);

/*
	��ͣ������Ƶ�ļ�
	[in]
	type:�豸����
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_pauseSoundEx(int32 type);
/*
	����������Ƶ�ļ�
	[in]
	type:�豸����
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_resumeSoundEx(int32 type);
/*
	ֹͣ������Ƶ�ļ�
	[in]
	type:�豸����
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_stopSoundEx(int32 type);
/*
	�ر��豸
	[in]
	type:�豸����
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_closeSoundEx(int32 type);
/*
	��������
	[in]
	volume:������С0~5
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���
*/
int32 mrc_setVolume(int32 volume);

/*
	��ȡ���ֵ���ʱ����S
	[out]
	p:ָ��������ݽṹ��T_DSM_AUDIO_POS����ָ���ָ�룬
	������ݽṹ��ֵ�����ܵ�ʱ��
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���

	ע��mtk�ϴ˽ӿ������ļ���ʽ���ŵ�ʱ����Ч��
*/
int32 mrc_getSoundTotalTime(int32 type, uint8** p);

/*
	��ȡ��ǰ�Ѿ����ŵ�ʱ����S
	[out]
	p:ָ��������ݽṹ��T_DSM_AUDIO_POS����ָ���ָ�룬
	������ݽṹ��ֵ�����Ѿ����ŵ�ʱ��
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���

	ע��mtk�ϴ˽ӿ������ļ���ʽ���ŵ�ʱ����Ч��
*/

int32 mrc_getSoundCurTime(int32 type, uint8** p);


/*
	��ȡ��ǰ�Ѿ����ŵ�ʱ�����ms
	[out]
	p:ָ��������ݽṹ��T_DSM_AUDIO_POS����ָ���ָ�룬
	������ݽṹ��ֵ�����Ѿ����ŵ�ʱ��
	[return]:	MR_SUCCESS ��ʼ���ɹ�
			MR_FAILED ��ʼ��ʧ��
			MR_IGNORE ��֧�ָù���

	ע��mtk�ϴ˽ӿ������ļ���ʽ���ŵ�ʱ����Ч��
*/

int32 mrc_getSoundCurTimeMs(int32 type, uint8** p);

/*
���ò���λ�ã�����ļ����߻������ʼλ�õ�ƫ����
��IN��
	SOUND_POS pos������ļ����߻������ʼλ�õ�ƫ����
��return��
	MR_SUCCESS	�ɹ�
	MR_FAILED	ʧ��
	MR_IGNORE   ��֧�ָù���
*/
int32 mrc_setPlayPos(int32 type,T_DSM_AUDIO_POS pos);

/*
���ò���ʱ�䣬����ļ����߻������ʼʱ���ƫ����
��IN��
	SOUND_POS pos������ļ����߻������ʼʱ���ƫ��������λ��ms
��return��
	MR_SUCCESS	�ɹ�
	MR_FAILED	ʧ��
	MR_IGNORE   ��֧�ָù���
*/
int32 mrc_setPlayTime(int32 type,T_DSM_AUDIO_POS pos);

/*
��ȡ��ǰ�豸��״̬
��IN��
	��
��return��
	�豸״ֵ̬  �ɹ�
	MR_FAILED	ʧ��
	MR_IGNORE   ��֧�ָù���

*/
int32 mrc_getDeviceState(int32 type);
#endif
