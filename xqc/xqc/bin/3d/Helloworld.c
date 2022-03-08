#include "mrc_base.h"





/*
Ӧ�ó�ʼ������
�ú�����Ӧ�ó�ʼ���ڼ䱻mythroadƽ̨���ã����������
�����н���ȫ�ֱ����ĳ�ʼ���ȹ�����
����ֵ:
MR_SUCCESS  Ӧ�ó�ʼ���ɹ�
MR_FAILED      Ӧ�ó�ʼ��ʧ��
*/
int32 mrc_init(void)
{
	
   ininint();
	return MR_SUCCESS;
}



/*
Ӧ���¼�����
�ú�����Ӧ�������ڼ䣬ÿ��mythroadƽ̨�յ��¼�ʱ
������
����:
code:
      code�Ŀ���ȡֵ����:
enum {
   MR_KEY_PRESS,           //���������¼�
   MR_KEY_RELEASE,        //�����ͷ��¼�
   MR_MOUSE_DOWN,       //����������꣩�����¼�
   MR_MOUSE_UP,            //����������̧꣩��/�ͷ��¼�
   MR_MENU_SELECT,       //�˵�ѡ���¼�
   MR_MENU_RETURN,       //�˵������¼�
   MR_DIALOG_EVENT,      // �Ի���/�༭��/�ı����¼�
   MR_EVENT02,             //VM�������벻Ҫʹ��
   MR_EXIT_EVENT,           //Ӧ���˳��¼�
   MR_EVENT03,                 //VM�������벻Ҫʹ��
   MR_LOCALUI_EVENT         //���ػ��ӿ��¼�
};

param0:
   ��codeΪMR_KEY_PRESS��MR_KEY_RELEASEʱ��param0�Ŀ���ȡֵ����:
enum {
   MR_KEY_0,               //���� 0
   MR_KEY_1,               //���� 1
   MR_KEY_2,               //���� 2
   MR_KEY_3,               //���� 3
   MR_KEY_4,               //���� 4
   MR_KEY_5,               //���� 5
   MR_KEY_6,               //���� 6
   MR_KEY_7,               //���� 7
   MR_KEY_8,               //���� 8
   MR_KEY_9,               //���� 9
   MR_KEY_STAR,            //���� *
   MR_KEY_POUND,           //���� #
   MR_KEY_UP,              //���� ��
   MR_KEY_DOWN,            //���� ��
   MR_KEY_LEFT,            //���� ��
   MR_KEY_RIGHT,           //���� ��
   MR_KEY_POWER,           //���� �һ���
   MR_KEY_SOFTLEFT,        //���� �����
   MR_KEY_SOFTRIGHT,       //���� �����
   MR_KEY_SEND,            //���� ������
   MR_KEY_SELECT,          //���� ȷ��/ѡ����������м���ȷ�ϼ���������Ϊ�ü���
   MR_KEY_VOLUME_UP,          //���� �����
   MR_KEY_VOLUME_DOWN,          //���� �����
   MR_KEY_NONE             //���� ����
};
��codeΪMR_MOUSE_DOWN��MR_MOUSE_UPʱ��param0Ϊ��Ļ��x���ꣻ
��codeΪMR_MENU_SELECTʱ��param0Ϊ�˵�index��
��codeΪMR_DIALOG_EVENTʱ��param0�Ŀ���ȡֵ����:
enum {
   MR_DIALOG_KEY_OK,         //�Ի���/�ı���ȵ�"ȷ��"�������(ѡ��)
   MR_DIALOG_KEY_CANCEL  //�Ի���/�ı���ȵ�"ȡ��"("����")�������(ѡ��)
};
��codeΪMR_LOCALUI_EVENTʱ��param0�Ŀ���ȡֵ����:
enum {
   MR_LOCALUI_KEY_OK,       //���ؿؼ���"ȷ��"�������(ѡ��)
   MR_LOCALUI_KEY_CANCEL//���ؿؼ���"ȡ��"("����")�������(ѡ��)
};

param1:
��codeΪMR_MOUSE_DOWN��MR_MOUSE_UPʱ��param1Ϊ��Ļ��y���ꣻ

����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
*/
int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	event3d(code,param0,param1);
	return MR_SUCCESS;
}

/*
Ӧ����ͣ����
�ú�����Ӧ�ñ���ͣʱ���е绰���¼���ͣӦ�ã�
��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
*/
int32 mrc_pause(void)
{
	return 0;
}

/*
�ú�����Ӧ�ûָ�����ʱ��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
*/
int32 mrc_resume(void)
{
	return 0;
}

/*
�ú�����Ӧ���˳�ʱ��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
*/
int32 mrc_exitApp(void)
{
	return 0;
}