#ifndef _GRAPHICS_H
#define _GRAPHICS_H

//��Բ
void circle(int r,int x0,int y0,uint8 rr,uint8 gg,uint8 bb);
//����Բ
void ellipse(int x0,int y0,int a,int b,uint8 rr,uint8 gg,uint8 bb);
//�����Ļ��16bin��old_color����ɫ��new_color����ɫ
void flood_fill_4( int x, int y, uint16 old_color, uint16 new_color);


#endif//_GRAPHICS_H
