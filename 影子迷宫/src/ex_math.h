
#ifndef _EX_MATH_H_
#define _EX_MATH_H_
#include "mpc.h"

// �жϵ�(x,y)�Ƿ��ھ�������(rectx,recty,rectw,recth)��
int ex_isPointCollisionRect(int x, int y, int rectx, int recty, int rectw, int recth);

// ���κ�Բ����ײ���
int ex_IsCircleCollisionRect(float circleXPos, float circleYPos, float radius, float rectX, float rectY, float rectW, float rectH);

int ex_isCollisionRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);



// ��(x,y)��תָ������r���õ���ת�������
// ��תһ��ˮƽ�ߣ��õ���ת�������
// ��������ת���ĵ�(px,py)����ת����뾶rx����ת����뾶ry�� ��ת������ָ��(*x,*y)
void ex_toSpin(int px, int py, int rx, int ry, int r, int *x, int *y);


// ������֮����� �����ڼ���Բ��Բ����ײ(������Բ��Բ�ľ���������ǵİ뾶ֻ�ͣ���ô��ײ�ɹ�)
float ex_getLineSize(int x, int y, int x2, int y2);

// ������֮�仡��
float ex_getRadiam(float x, float y, float rx, float ry);

#endif
