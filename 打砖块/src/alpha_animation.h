#include "mrc_graphics.h"

//��ͼƬsrc���Ƶ�ͼƬdst��,��ָ�����Ƶ�͸����
//����ͼƬ�Ŀ�����ݱ���һ��
void drawBitmapFormAlpha(BITMAP_565 *dst, BITMAP_565 *src, int alpha);
//�򿪴��̻���
//��ģʽ0���� 1���
void openFSBitmapCache();
//��bitmap���浽���̻�����(ֻ�����ֽ�����),�����ػ���λ��offset
int32 saveBitmapCache(BITMAP_565 *bitmap);
//�Ӵ��̻���ָ��λ�ö�ȡ��bitmap
int32 readBitmapFormCache(BITMAP_565 *bitmap, int32 offset);
//�رջ���
void closeFSBitmapCache();

