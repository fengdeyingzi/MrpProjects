#ifndef _CAMERA_H
#define _CAMERA_H


typedef enum
{
STATUS_UNINIT,
STATUS_INIT,
STATUS_INIT_FAILED,
STATUS_START,
STATUS_START_FAILED,
STATUS_CAPTURE,
STATUS_CAPTURE_FAILED,
STATUS_PAUSE,
STATUS_RESUME_FAILED,
STATUS_CLOSE
} STATUS_T;
typedef enum
{
CAPTURE_MODE_MULTIPLE = 1,
CAPTURE_MODE_SINGLE = 2
} CAPTURE_MODE_T;
typedef enum
{
IMG_QUALITY_LOW,
IMG_QUALITY_NORMAL,
IMG_QUALITY_HIGH,
IMG_QUALITY_T};
typedef struct
{
void* img_data;
int32 img_data_len;
}IMG_DATA_T;


/*
������:camera_init
����:����ͷ�豸��ʼ��
���룺��
���:��
���أ�
     SUCCESS  �ɹ�
     FAILED   ʧ��
     IGNORE ��֧�ָù���

��ע:ʹ������ͷ֮ǰ������øýӿڣ������޷���������
*/
int32 camera_init(void);

/*
������:camera_creat
����:��������ͷԤ������
���룺
x:Ԥ�������x����
y:Ԥ�������y����
prevW:Ԥ������Ŀ��
prevH:Ԥ������ĸ߶�
imgW:����������Ƭ�Ŀ��
imgH:����������Ƭ�ĸ߶�
img_quality_type:��Ƭ��������
	 IMG_QUALITY_LOW:�ϲ��ͼƬ����
	 IMG_QUALITY_NORMAL:һ��Ч����ͼƬ����
	 IMG_QUALITY_HIGH:��Ʒ�ʵ�ͼƬ����

���:��
���أ�
     SUCCESS  �ɹ�
     FAILED   ʧ��
     IGNORE ��֧�ָù���

ע��:prevW��prevH��󣬷ֱ�Ϊ��Ļ�Ŀ�͸ߣ������Ļ��С��240x320��
��ô���ǵ����ֵ�ֱ�Ϊ240��320��
imgW��imgH�����ֵ������ͷ�������й�ϵ�����Դ�����Ļ�Ŀ�͸ߡ�
*/
int32 camera_creat(uint16 x, uint16 y, uint16 prevW, uint16 prevH, uint16 imgW, uint16 imgH,uint32 img_quality_type);

/*
������:camera_capture
����:����
���룺
mode:����ģʽ�������ĺ͵�������ģʽ
	 CAPTURE_MODE_MULTIPLE: ��������ģʽ
	 CAPTURE_MODE_SINGLE: ��������ģʽ,��������Ժ����Ҫ��������Ҫ����camera_resume

���:
img_data:����������յ�jpgeͼƬ���ݣ���������ָ������ݴ�С,���������Ժ�
�û���Ҫ�Լ������ڴ�ռ䣬��ͼƬ���п���
���أ�
     SUCCESS  �ɹ�
     FAILED   ʧ��
     IGNORE ��֧�ָù���
*/
int32 camera_capture(uint8 mode, IMG_DATA_T* img_data);

/*
������:camera_destroy
����:�ر�����ͷ�豸
����:��
���:��
����:��
��ע:���øýӿ��Ժ�������ٴ�ʹ�ã���ô�������³�ʼ������ͷ
*/
void camera_destroy(void);

/*
������:camera_pause
����:��ͣ����ͷ�������ǰ����Ԥ��״̬��
��ô�����˳�Ԥ��״̬
���룺��
���:��
���أ�
     SUCCESS  �ɹ�
     FAILED   ʧ��
     IGNORE ��֧�ָù���
*/
int32 camera_pause(void);

/*
������:camera_resume
����:�ָ�����ͷ�������ǰ����Ԥ��״̬��
��ô����ָ���Ԥ��״̬
���룺��
���:��
���أ�
     SUCCESS  �ɹ�
     FAILED   ʧ��
     IGNORE ��֧�ָù���
*/
int32 camera_resume(void);

/*
������:camera_getZoom
����:��ȡ����ͷ���ŵȼ�
���룺��
���:��
���أ�����ͷ���ŵȼ�
*/
int32 camera_getzoom(void);

/*
������:camera_zoomOut
����:����ͷ��С����
���룺��
���:��
���أ�
     SUCCESS  �ɹ�
     FAILED   ʧ��
     IGNORE ��֧�ָù���
*/
int32 camera_zoomout(void);

/*
������:camera_zoomIn
����:����ͷ�Ŵ���
���룺��
���:��
���أ�
     SUCCESS  �ɹ�
     FAILED   ʧ��
     IGNORE ��֧�ָù���
*/
int32 camera_zoomin(void);

/*
������:camera_status
����:��ȡ����ͷ��ǰ״̬
���룺��
���:��
���أ�����ͷ״ֵ̬������״ֵ̬����:
	 STATUS_UNINIT δ��ʼ��״̬
	 STATUS_INIT ��ʼ���ɹ�״̬
	 STATUS_INIT_FAILED ��ʼ��ʧ��״̬ 
	 STATUS_START �����ɹ�״̬
	 STATUS_START_FAILED ����ʧ��״̬
	 STATUS_CAPTURE ���ճɹ�״̬
	 STATUS_CAPTURE_FAILED ����ʧ��״̬
	 STATUS_PAUSE ��ͣ״̬
	 STATUS_RESUME_FAILED �ָ�ʧ��״̬
	 STATUS_CLOSE �ر�״̬

*/
uint8 camera_status(void);

#endif//_CAMERA_H
