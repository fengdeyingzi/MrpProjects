#include <base.h>


int event(int type, int p1, int p2)
{
    if(KY_DOWN == type)
    {
        switch(p1)
        {
        case _SRIGHT:
            exit();
            break;
        }
    }

    return 0;
}

typedef struct
{
    uint16 year;                 //��
    uint8  month;                //��
    uint8  day;                  //��
    uint8  hour;                 //ʱ��24Сʱ��
    uint8  minute;               //��
    uint8  second;               //��
}mr_datetime;

int init(void)
{
    mr_datetime date;

    if(!getdatetime(&date))
        printf("��ȡ�ɹ�,%d��%d��%d��%dʱ%d��%d��",date.year,date.month,date.day,date.hour,date.minute,date.second);
    else
        printf("��ȡʧ��");
    cls(0,0,0);
    dtext("�������",0,0,255,255,0,0,1);
    ref(0,0,SCRW,SCRH);
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
