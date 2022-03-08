#ifndef _X_GRAPHICS_H__
#define _X_GRAPHICS_H__

#define  MAX_POINT 10   //�����������

/**
 * \����������
 */
typedef struct _RECT
{
    int left;		/*���ε����Ͻ�X����*/
    int top;		/*���ε����Ͻ�Y����*/
    int width;		/*���εĿ��*/
    int height;		/*���εĸ߶�*/

} RECT, *PRECT;
//��ά�ṹ
typedef struct 
{ 
	int x; 
	int y; 
}EACH_POINT;
//����νṹ
typedef struct 
{ 
	int y_top; 
	float x_int; 
	int delta_y; 
	float x_change_per_scan;
}EACH_ENTRY; 




//���θ�ֵ
void RectRe(RECT *rect,int x,int y,int w,int h);
//���ڻ����жϣ��Զ������������
void Rectlinepd(RECT *rect);
//�ж�psrcs1��psrcs2�Ƿ��ཻ
int Intersect(PRECT psrcs1, PRECT psrcs2);
//һ�����Ƿ�����Ļ��
int pointpd(int x,int y);
//pdst��psrc1�ཻ����ֵ��psrc2
void sectRect(PRECT pdst, PRECT psrc1, PRECT psrc2);
//16λת24λ
void rgb24(uint16 n,mr_colourSt *r);
//�����Ļ
void gr_cls(mr_colourSt color);
//����
void gr_point(int x,int y,uint16 color);
//��gr_point���ƣ���ͬ������ɫ����
void gr_pointex(int x,int y,mr_colourSt color);
//����
void gr_line(int x,int y,int x1,int y1,mr_colourSt color);
//������
void gr_rect(int x,int y,int w,int h,mr_colourSt color);
//�������
void gr_fill_area(EACH_POINT *zb,int count,mr_colourSt color);
//���е�����
void gr_bigger(EACH_POINT *zb,int n,mr_colourSt color) ;
//��Բ
void gr_Circle(int r,int x0,int y0,mr_colourSt colors);
//ʹ���е��㷨����Բ
void gr_Ellipse(int x0,int y0,int a,int b,mr_colourSt colors);
//��ȡ��Ļһ��λ�õ���ɫ
uint16 getpixel(int x,int y);
//������ʹ�ø�����ɫ������� �ķ����Flood�㷨 �õĵݹ�ɫ�鲻Ҫ̫��
void gr_Flood_Fill_4( int x, int y, uint16 old_color, uint16 new_color);

//���òü�����
void gr_set_clip_rect(int left, int top, int right, int botton);
//�ѻ�������������ʾ����Ļ��
void gr_graph();
//ͼ�ο��ʼ��
void gr_init(void);
//�ر�ͼ�ο�
void gr_close();
#endif
