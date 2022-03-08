#ifndef _GRAPHICS_H
#define _GRAPHICS_H

//»­Ô²
void circle(int r,int x0,int y0,uint8 rr,uint8 gg,uint8 bb);
//»­ÍÖÔ²
void ellipse(int x0,int y0,int a,int b,uint8 rr,uint8 gg,uint8 bb);
//Ìî³äÆÁÄ»£¬16bin£¬old_color¾ÉÑÕÉ«£¬new_colorĞÂÑÕÉ«
void flood_fill_4( int x, int y, uint16 old_color, uint16 new_color);


#endif//_GRAPHICS_H
