
#include <mrc_base.h>
#include "xl_debug.h"
#include "xl_bmp.h"

void* mr_malloc(int32 len){
  uint8 *buf = NULL;
  buf = mrc_malloc(len-4);
  return buf-4;
}

//����һ��bmp �������ڴ����ݣ���ȣ��߶ȣ�͸��ɫ(0��ʾ��͸��)
BITMAP_565* bmp_create(uint16 *bitmap, int width, int height, int transcolor){
 BITMAP_565* bmp = mrc_malloc(sizeof(BITMAP_565));
 memset(bmp, 0, sizeof(BITMAP_565));
 bmp->bitmap = bitmap;
 bmp->width = width;
 bmp->height = height;
 bmp->color_bit = 16;
 bmp->mode = BM_COPY;
 if(transcolor!=0){
  bmp->mode = BM_TRANSPARENT;
  bmp->transcolor = transcolor;
 }
 return bmp;
}

//��ȡһ����λint��������
int get_int(char *buf,int ptr){
 int num = (buf[ptr]&0xff) | (buf[ptr+1]<<8) | (buf[ptr+2]<<16) | (buf[ptr+3]<<24);
 return num;
}

static int get_short(char *buf,int ptr){
 int num = (buf[ptr]&0xff) | (buf[ptr+1]<<8);
 return num;
}

//��ȡbmp
BITMAP_565* bmp_read(void *buf, int len){
 char *bufc = (char*)buf;
 int ptr=0;
 int w=0,h=0;
 int bit=16; //bmpλ��
 int bmpstart=0; //bmp����λ��
 int iy=0;
 int i=0;
 uint16 *buf16 = NULL;
 char *buf24 = NULL;
 int tcolor = 0;
 int wsize;
 int iw;
 BITMAP_565* bmp = mrc_malloc(sizeof(BITMAP_565));
 memset(bmp,0,sizeof(BITMAP_565));
 bmp->color_bit = 16;
 bmp->mode = BM_COPY;
 bmp->buflen = len;
//  debug_log("����ļ�ͷ");
//  debug_log("����ļ�ͷ %c %c",*(bufc+1),*(bufc+2));
 //����ļ�ͷ
 if(bufc[0]=='B' && bufc[1]=='M'){
  // debug_log("BM\n");
  ptr = 10;
  bmpstart = get_int(bufc,ptr);
  // debug_printf("bmpstart\n");
  ptr = 18;
  w = get_int(bufc,ptr);
  ptr = 22;
  h = get_int(bufc,ptr);
  ptr = 28;
  bit = get_short(bufc,ptr);
  if(bit == 16){
  //  debug_printf("16λͼ\n");
   ptr = bmpstart;
   bmp->width = w;
   bmp->height = h;
   bmp->bitmap = (uint16*)mrc_malloc(w*h*2);
   bmp->buflen = w*h*2;
   //����λͼ����
   
   for( iy=0;iy<bmp->height;iy++){
    mrc_memcpy(bmp->bitmap+iy*bmp->width, bufc+ptr+(bmp->height-1-iy)*bmp->width*2, w*2);
    // mrc_printf("����λͼ����%d %d\n",iy, bmp->height-1-iy);
   }
   
  }
  else if(bit == 24){
  //  debug_printf("��ǰλͼ��24λ");
   ptr = bmpstart;
   bmp->width = w;
   bmp->height = h;
   //�����ֽ�
   wsize = w*3;
   if(wsize%4!=0) wsize = wsize - wsize%4 + 4;
  //  debug_printf("�����ڴ�\n");
   bmp->bitmap = (uint16*)mr_malloc(w*h*2);
   bmp->buflen = w*h*2;
   
   //32ת16λ
    buf16 = (unsigned short*)mrc_malloc(bmp->width*bmp->height*2);
    buf24 = (bufc+ptr);
   for(i=0;i<h;i++){
	   for(iw=0;iw<w;iw++){
		   
    //BGRA
     tcolor = (((buf24[i*wsize+iw*3]>>3)&0x1f) 
    | ((buf24[i*wsize+iw*3+1]<<3)&0x7e0) 
    | ((buf24[i*wsize+iw*3+2]<<8)&0xf800));
    buf16[i*w+iw] = (uint16)(tcolor&0xffff);
	
	   }
   }
   //����λͼ����
    for(iy=0;iy<bmp->height;iy++){
		
     //debug_printf("�������� %d %d\n",iy,bmp->height-1-iy);
	 
     mrc_memcpy (bmp->bitmap + iy*bmp->width, buf16+(bmp->height-1-iy)*bmp->width,w*2);
   }
   mrc_free(buf16);
  }
 }
 else
 {
	//  debug_printf("����bmpͼƬ");
  return NULL;
 }
//  debug_printf("����bmp\n");
 return bmp;
}

BITMAP_565* bma_read(void *buf, int len){
 char *bufc = (char*)buf;
 int ptr=0;





 BITMAP_565* bmp = mrc_malloc(sizeof(BITMAP_565));
 memset(bmp,0,sizeof(BITMAP_565));
 bmp->color_bit = 16;
 bmp->mode = BM_COPY;
 bmp->buflen = len;
 if(bufc[0] == 'M' && bufc[1] == 'A' && bufc[2] == 'P' && bufc[3] == '5'){
   ptr = 4;
   bmp->width = get_int(bufc, ptr);
   ptr = 8;
   bmp->height = get_int(bufc, ptr);
   ptr = 12;
   bmp->transcolor = get_short(bufc, ptr);
   if(bmp->transcolor != 0){
    bmp->mode = BM_TRANSPARENT;
   }
   ptr = 14;
   mrc_memmove(bufc, bufc+ptr, bmp->width * bmp->height * 2);
   bmp->bitmap = (uint16*)bufc;
 }
 else
 {
	//  debug_printf("����bmpͼƬ");
  return NULL;
 }
//  debug_printf("����bmp\n");
 return bmp;
}

//����͸��ɫ argb
void bmp_settranscolor(BITMAP_565* bmp, int color){
 bmp->transcolor = ((color>>8)&0xf800) | ((color>>5)&0x07e0) | ((color>>3)&0x1f);
 if(color != 0){
  bmp->mode = BM_TRANSPARENT;
 }
 else{
  bmp->mode = BM_COPY;
 }
 
}
//��ʾbmp
void bmp_draw(BITMAP_565* bmp, int x,int y){
//  mrc_printf("bmpdraw %d \n",bmp->width);
 mrc_bitmapShowExTrans(bmp->bitmap, (int16)x,(int16)y, (uint16)bmp->width, (uint16)bmp->width,(uint16)bmp->height, (uint16)bmp->mode, 0,0,(uint16)bmp->transcolor);
}
//������ʾbmp
void bmp_drawflip(BITMAP_565* bmp, int x,int y,int w,int h,int tx,int ty){
//  mrc_printf("bmpshowflip width=%d w=%d\n",bmp->width,w);
 mrc_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)w,(uint16)h, (uint16)bmp->mode, (int16)tx,(int16)ty,(uint16)bmp->transcolor);
}
//bmp�ͷ�
void bmp_free(BITMAP_565* bmp){
 mrc_freeFileData(bmp->bitmap, bmp->buflen);
 mrc_free(bmp);
}