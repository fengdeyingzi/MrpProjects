
��Ҫ����ĻΪ240*320�Ĺ��ܻ��ϱ�д����
ʹ��armcc����,��ֻ֧���ں�������ǰ�涨�����
����,���ܻ���C���Դ�����ѭ�����趨:
```c
typedef  unsigned short     uint16;      //�з���16bit����
typedef  unsigned long int  uint32;      //�޷���32bit����
typedef  long int                int32;      //�з���32bit����
typedef  unsigned char      uint8;        //�޷���8bit����
typedef  signed char          int8;        //�з���8bit����
typedef  signed short         int16;       //�з���16bit����
//��������ֵ��δ�������������Ҳ����ʹ�ã�����֪�����ֵ��
enum {  
   _0,           //���� 0
   _1,           //���� 1
   _2,           //���� 2
   _3,           //���� 3
   _4,           //���� 4
   _5,           //���� 5
   _6,           //���� 6
   _7,           //���� 7
   _8,           //���� 8
   _9,           //���� 9
   _STAR,        //���� *
   _POUND,       //���� #
   _UP,          //���� ��
   _DOWN,        //���� ��
   _LEFT,        //���� ��
   _RIGHT,       //���� ��
   _SLEFT=17,    //���� �����
   _SRIGHT,      //���� �����
   _SEND,        //���� ������
   _SELECT       //���� ȷ��/ѡ����������м���ȷ�ϼ���������Ϊ�ü���
};

//�����¼��������¼����Լ����壩
enum {
    KY_DOWN, 	 //��������
    KY_UP,       //�����ͷ�
    MS_DOWN, 	 //��갴��
    MS_UP 	     //����ͷ�
};
extern int16 SCRW;   //��ʾ��Ļ���
extern int16 SCRH ; //��ʾ��Ļ�߶�

typedef struct {
 uint16* bitmap;
 int width; //
 int height;
 int color_bit; //��ɫλ�� Ĭ��16
 int transcolor; //͸��ɫ Ĭ��0xffff
 int mode; //����ģʽ Ĭ��BM_COPY
 int32 buflen; //���������
} BITMAP_565; 

/*
������ں���,������main
*/
int32 mrc_init(void):
/*
�����¼�����,��Ҫ���ڽ��ܴ���/�����¼�
��code==KY_UP��KY_DOWNʱ��ʾ���������¼�,param0Ϊ������ֵ
��code==MS_UP��MS_DOWNʱ��ʾ�����¼� param0��param1Ϊ�������Ļ����
*/
int32 mrc_event(int32 code, int32 param0, int32 param1);
/*
�����˳��¼�����,�����ﴦ���ͷ��ڴ�Ȳ���
*/
int32 mrc_exitApp();

```
���ܻ��ɵ������º���,��Щ����������mrc_base.h:
```c

//ȡ��Ļ���������ڴ��ַ
uint16 * w_getScreenBuffer(void);

/*
������ʱ��

����:
      ��NULL     ��ʱ�����
      NULL          ʧ��
*/
int32 mrc_timerCreate (void);

/*
ɾ����ʱ��

����:
t           ��ʱ�����
*/
void mrc_timerDelete (int32 t);

/*
ֹͣ��ʱ��

����:
t           ��ʱ�����
*/
void mrc_timerStop (int32 t);
/*
��ʱ���ص�����
����:
data:
   ������ʱ��ʱ�����data������
*/
typedef void (*mrc_timerCB)(int32 data);

/*
������ʱ��

����:
t           ��ʱ�����
time      ��ʱ��ʱ������λ����
data      ��ʱ������
f           ��ʱ���ص�����
loop      �Ƿ�ѭ����0:��ѭ����1:ѭ��

����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ��
*/
int32 mrc_timerStart (int32 t, int32 time, int32 data, mrc_timerCB f, int32 loop);
/*
ʹ��ָ������ɫ�����Ļ��
����:
r,g,b	          ������ɫ
*/
void mrc_clearScreen(int32 r, int32 g, int32 b);

/*
ˢ����Ļָ�������򡣸ú����Ĺ����ǽ�mythroad��Ļ
����ָ��������ˢ�µ���Ļ�ϡ�
����:
x, y, w, h	       ��Ļָ�����������Ͻ�Ϊ��x,y�������
                        Ϊ��w,h����
*/
void mrc_refreshScreen(int16 x, int16 y, uint16 w, uint16 h);


/*
���ƾ�����ָ������
����:
x,y,w,h:	λ��
r,g,b	     ������ɫ
*/
void mrc_drawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);

/*
�����߶Ρ�
����:
x1,y1,x2,y2:	��ĩ��λ��
r,g,b	          ������ɫ
*/
void mrc_drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 r, uint8 g, uint8 b);

/*
����Ļ��ָ��λ�û��Ƶ㡣
����:
x, y	           ���Ƶ��λ��
r,g,b	          ������ɫ
*/
void mrc_drawPointEx(int16 x, int16 y, int32 r, int32 g, int32 b);
```