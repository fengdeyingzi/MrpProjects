#include <mrc_base.h>
#ifndef UC3_FONT_H
#define UC3_FONT_H
//��ʼ��uc3����
int32 uc3_init(void);

// ��uc3����
int32 uc3_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

// ��ȡ���ֿ��
int uc3_getWidth(char *pcText, int is_unicode);

// �ͷ�uc3����
void uc3_free(void);

#endif