
#ifndef _EX_MATH_H_
#define _EX_MATH_H_
#include "mpc.h"

// 判断点(x,y)是否在矩形区域(rectx,recty,rectw,recth)内
int ex_isPointCollisionRect(int x, int y, int rectx, int recty, int rectw, int recth);

// 矩形和圆形碰撞检测
int ex_IsCircleCollisionRect(float circleXPos, float circleYPos, float radius, float rectX, float rectY, float rectW, float rectH);

int ex_isCollisionRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);



// 点(x,y)旋转指定弧度r，得到旋转后的坐标
// 旋转一条水平线，得到旋转后的坐标
// 参数：旋转中心点(px,py)，旋转横向半径rx，旋转纵向半径ry， 旋转后坐标指针(*x,*y)
void ex_toSpin(int px, int py, int rx, int ry, int r, int *x, int *y);


// 求两点之间距离 可用于计算圆与圆的碰撞(当两个圆的圆心距离大于它们的半径只和，那么碰撞成功)
float ex_getLineSize(int x, int y, int x2, int y2);

// 求点与点之间弧度
float ex_getRadiam(float x, float y, float rx, float ry);

#endif
