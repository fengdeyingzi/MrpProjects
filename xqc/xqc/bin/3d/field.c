
#include "3d.h"







/*************************************************
  Function:				create_ground
  Description:			����һ��ָ������ĵ���
  Calls:				create_model
						create_point
						create_line
						model_add_point
						model_add_line
  Table Accessed:  
  Table Updated:   
  Input:				length	ָ���ĳ�
						width	ָ���Ŀ�
  Output:         
  Return:				����һ������ģ��ָ��
  Others:         
*************************************************/
XM_MODEL *create_ground(int length, int width)
{
	XM_MODEL *new_model;
	XM_POINT *p1;
	XM_POINT *p2;
	XM_LINE *l;
	int i;

	/* ����һ����ģ�ͣ�����ʼ�� 
	   ��ԭ��Ϊ���� */
	new_model = create_model(NULL);
	new_model->offset->x = 0.0;
	new_model->offset->y = 0.0;
	new_model->offset->z = 0.0;

	/* ������ֱ�߼���Ӧ�ĵ� 
	   ������ϵ��
	      ��x
	      �O
	      �O
	      �O          y
	     ��������������
	   Ϊ׼��Z��Ϊ�� */
	for (i = 0; i <= length; i++)
	{
		p1 = create_point(-(float)length / 2 + i * 1.0, (float)width / 2, 0.0);
		p2 = create_point(-(float)length / 2 + i * 1.0, -(float)width / 2, 0.0);
		l = create_line(p1, p2);
		model_add_point(new_model, p1);
		model_add_point(new_model, p2);
		model_add_line(new_model, l);
	}

	/* ����ˮƽ�߼���Ӧ�ĵ� 
	   ������ϵ��
	      ��x
	      �O
	      �O
	      �O          y
	     ��������������
	   Ϊ׼��Z��Ϊ�� */
	for (i = 0; i <= width; i++)
	{
		p1 = create_point(-(float)length / 2, -(float)width / 2 + i * 1.0, 0.0);
		p2 = create_point((float)length / 2, -(float)width / 2 + i * 1.0, 0.0);
		l = create_line(p1, p2);
		model_add_point(new_model, p1);
		model_add_point(new_model, p2);
		model_add_line(new_model, l);
	}

	p1 = create_point(-(float)length / 2, -(float)width / 2 + (-3) * 1.0, 0.0);
	p2 = create_point((float)length / 2, -(float)width / 2 + (-3) * 1.0, 0.0);
	l = create_line(p1, p2);
	model_add_point(new_model, p1);
	model_add_point(new_model, p2);
	model_add_line(new_model, l);

	/* ����ģ�͵Ļ��Ʒ�ʽ 
	   ���߻��ƣ���ģ��������Ϊ���ģ�û�ж����ͼԪ
	   ֻ�е��������� */
	new_model->style = DMS_LINE | DMS_CUT3D;

	return new_model;
};