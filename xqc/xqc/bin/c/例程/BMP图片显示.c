#include <base.h>


#define BMPW 16
#define BMPH 16

int init(void)
{
    int32 f;
    int32 len;
    uint16 *ptr;
    char *filename="c/����/1.bmp";

    len=getlen(filename);//ȡ���ļ�����
    f=open(filename,1);//��ֻ����ʽ���ļ�
    ptr=malloc(len);//����ͼƬ��ŵ��ڴ�
    read(f,ptr,len);//��ͼƬ��ȡ���ڴ���
    close(f);//�ر��ļ�

    cls(0,0,0);
    bmpshowflip(ptr,0,0,BMPW,BMPW,BMPH,BM_COPY,0,0,0);//��ͼƬ
    ref(0,0,SCRW,SCRH);

    free(ptr);



    return 0;
}

int event(int type, int p1, int p2)
{
    if(KY_DOWN == type)
    {
        switch(p1)
        {
        case _SRIGHT:
            exit();
            break;
        case _1:
            break;
        }
    }

    return 0;
}

int pause(void)
{
    return 0;
}

int resume(void)
{
    return 0;
}
