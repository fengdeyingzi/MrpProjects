#ifndef __FONT_H__
#define __FONT_H__
extern int32 FONT;

//��ʼ��
void fontinit(void);
//�����ֿ�  12��16
int32 openfont(int font);
//�ͷ��ֿ�
int32 closefont(void);

//���Կ�ȸ߶�
int32 fontwh(char* pcText, int is_unicode, uint16 font, int32* w, int32* h);
//��ʾ�ַ���
int gbsez(char *a,int x,int y,int r,int g,int b,int un,int font);
//�������ֿ�ֱ����ʾ
int gbsezex(char *a,int x,int y,int r,int g,int b,int un,int font);
int32 gbsez2h(char* pcText, int16 w,int flag,uint16 font);
//��������
int32 gbsez2(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);
//��������
int32 gbsezex2(char* pcText, int16 x, int16 y, int w,int h,int r,int g,int b, int flag, uint16 font);

#endif
