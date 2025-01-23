#ifndef _MAPE_H_
#define _MAPE_H_

#include "mrc_base.h"
#include "mrc_file.h"
extern int SCRW, SCRH;

//˹������Ϸ��Ӣ������MAPE��ͼ�ṹ
//��Ҫmape, pngͬʱ����
//��ʱ��mape��mythroadĿ¼��png��mrp�ڲ�
//pwd, aef
//pwd��pngͼƬ, aef�������ļ���ʱ��֪����������
typedef struct _mape
{
  char *dir; //��ǰ��ͼ�����ļ��У�����

  char *path; //��ͼ�ļ�·��
  char *name;

  int32 bkg; //��ͼ����ɫ

  int32 mapw; //��ͼ����Լ���ͼ����
  int32 maph;
  int32 itemw;
  int32 itemh;

  char *layname; //�����
  char *resname; //��Դ��
  char *restype; //��Դ����
  int32 resnum;  //��Դ����
  char **resarr; //��Դ��������
  void **resdata; //��Դ���� MRC_TPNG

  int32 *data; //��ͼ����

  char *end; //δ֪�ĵ�ͼ�ṹ����
  int32 endlen;
} _MAPE;

//����mape�ļ�
extern _MAPE *loadMape(char *name);

//��ӡmape��Ϣ
extern int printMape(_MAPE *mape);

//����mape����Ļ
extern int drawMape(_MAPE *mape);
extern int drawMapePos(_MAPE *mape, int32 dx, int32 dy);

//����pngΪ��ͼ�ļ�
extern int pngToMape(_MAPE *mape, char *png);

//����mape��ͼ�ļ�
extern int saveMape(_MAPE *mape, char *name);

//�ͷ�mape�ṹ��
extern int freeMape(_MAPE *mape);

#endif

