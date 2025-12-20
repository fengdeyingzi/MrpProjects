
#include "base.h"
#include "bridge.h"
#include "utils.h"
#include "encode.h"


// int32 SCRW;
// int32 SCRH;
// int32 SCREEN_BIT;

// mr_table_st *_mr_table;
// T_mr_malloc mallocdata;
// T_mr_free freedata;
// T_mr_realloc realloc;
// T_memcpy memcpy;
// T_memmove memmove;
// T_strcpy strcpy;
// T_strncpy strncpy;
// T_strcat strcat;
// T_strncat strncat;
// T_memcmp memcmp;
// T_strcmp strcmp;
// T_strncmp strncmp;
// T_strcoll strcoll;
// T_memchr memchr;
// T_memset memset;
// T_strlen strlen;
// T_strstr strstr;
// T_strtoul strtoul;
// T_rand rand;
// T_mr_connectWAP wap;
// T_mr_open mrc_open;
// T_mr_seek mrc_seek;
// T_mr_close mrc_close;
// T_mr_write mrc_write;
// T_mr_read mrc_read;
// mrc_dtextline dtextline;
// mrc_dtext dtext;
// T_atoi atoi;
#define cls mrc_clearScreen
#define ref _ref
// T_mr_printf printf;
// T_sprintf sprintf;
// T_mr_getTime getuptime;
// T_mr_remove remove;
// T_mr_getLen getlen;
// T_mr_rename rename;
// T_mr_mkDir mkDir;
// T_mr_rmDir rmDir;
// T_mr_sleep sleep;
// T_mr_plat mrc_plat;
// T_mr_platEx mrc_platEx;

// MRC_TIMERCREATE timercreate;
// MRC_TIMERSTART timerstart;
// MRC_TIMERSETTIME timersettime;
// MRC_TIMERSTOP timerstop;
// MRC_TIMERDEL timerdel;

// T__mr_readFile mrc_readFileFromMrp;


// void mrc_clearScreen(int r, int g, int b) {
//     uint16 ncolor = MAKERGB565(r, g, b);
//     uint16 *screen = *(_mr_table->mr_screenBuf);
//     for (int ix = 0; ix < SCRW; ix++) {
//         *(screen + ix) = ncolor;
//     }
//     for (int iy = 1; iy < SCRH; iy++) {
//         mrc_memcpy((void *)(screen + iy * SCRW), (const void *)screen, (int)SCRW * 2);
//     }
// }

// void mrc_refreshScreen(int16 x, int16 y, uint16 w, uint16 h) {
//     uint16 *screen = *(_mr_table->mr_screenBuf);
//     _mr_table->mr_drawBitmap(screen, x, y, w, h);  // 2, 0, x, y, SCRW
// }

// void mrc_drawPoint(int16 x, int16 y, uint16 nativecolor)
// {
 
//  if(x>=SCRW || x<0 || y<0 || y>=SCRH)
// 	return;
//  *(w_getScreenBuffer() + y*SCRW + x) = nativecolor;
// }

// void mrc_drawPointEx(int16 x, int16 y, int32 r, int32 g, int32 b)
// {
//  uint16 color=MAKERGB(r,g,b);
//   if(x>=SCRW || x<0 || y<0 || y>=SCRH)
// 	 return ;
// *(w_getScreenBuffer() + y*SCRW + x) = color;
// }

// static void memset16(uint16 *ptr,uint16 color,uint16 len)
// {
// 	register uint16 i;
// 	for( i=0;i<len;i++)
// 	*(ptr+i)= color;
// }



// int32 mrc_bitmapShowFlip(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
// 		int16 sx, int16 sy, uint16 transcolor)
// 		{
// 		 	uint16 *dest = NULL, *src = NULL;
// 		 int dx=x,dy=y; //显示位置
// 		 int x_max=x+w,y_max=y+h;//计算dw,dh时的辅助变量，显示不能包括这个变量值
// 		 int dw,dh;
// 		 int row = 0, col = 0;
// 		 uint16* ptr=p;
// 		 int bx=sx,by=sy; //图片起始显示位置
// 		 int bx_max,by_max;
// 		 if(p==NULL)return -1;
// 		 //50%9 500%9
		 
// 		 switch((rop>>8)%9)
// 		 {
// 			case 0:
// 			case 2:
// 			case 4:
// 			case 6:
// 			case 8:
// 			 x_max=x+w;
// 			 y_max=y+h;
// 			 break;
// 			default:
// 			 x_max=x+h;
// 			 y_max=y+w;
// 		 }
		 
		 
// 		 //如果显示位置不在屏幕范围，重新计算显示区域
// 		 if(dx<0)dx=0;
// 		 if(dy<0)dy=0;
// 		 if(dx>=SCRW)return 0;
// 		 if(dy>=SCRH)return 0;
// 		 if(x_max<=0 || y_max<=0)return 0;
// 		 if(x_max>SCRW)x_max=SCRW;
// 		 if(y_max>SCRH)y_max=SCRH;
		 
// 		 //计算出显示的宽高
// 		 dw=x_max-x;
// 		 dh=y_max-y;
// 		 //如果图片起始位置不在图片上，则return
// 		 if(bx<0)return 0 ;
// 		 if(by<0)return 0;
// 		 if(bx>mw)return 0;
// 		 //if(by>mh)return;
// 		 //显示
// 		// mr_screenBuf
// 		 dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 	src = p + (by * mw + bx);
//   bx_max=bx+dw;
// 	by_max=by+dh;
// 	if(rop==BM_COPY)
// 	{
// 		uint32 w_bytes;
// 		w_bytes = dw * 2;
// 		for(row=0;row<dh;row++)
// 		{
// 			memcpy(dest, src, w_bytes);
// 			dest += SCRW;
// 			src+=mw;
// 		}
// 	}
// 	/*
// 		 if(rop==BM_COPY)
// 		 {
// 			for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src++;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += mr_screen_w - dw;
// 			src += mw - dw;
// 			}
// 		//	mr_screenBuf +dy*mr_screen_w + dx = by*mw +bx;
// 		 }
// 		 */
// 		 else if(rop==BM_TRANSPARENT)
// 		 {
// 			for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 if(*src!=transcolor)
// 				 *dest=*src;
// 				 dest++; src++;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dw;
// 			src += mw - dw;
// 			}
// 		 }
// 		 else
// 		 {
// 		 switch ((rop>>8)%9) 
// 		   {
// 			  case 0:// TRANS_NONE:
// 				 for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src++;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dw;
// 			src += mw - dw;
// 			}

// 				 break;
// 			  case 1: //TRANS_ROT90: //256
// 				x_max=x+h;
// 			 y_max=y+w;
// 				 //从左上角显示
// 				dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 				//从左下角开始显示
//       	src = p + ((by_max-1) * mw +bx+row);
//       for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src-=mw;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dw;
// 			src =p+((dw-1)*mw+row);
// 			}

				
				
				
// 				 break;
// 			  case 2: //TRANS_ROT180: //180度转动； 
// 				x_max=x+w;
// 			 y_max=y+h;
// 				 //从左上角显示
// 				dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 				//从右下角开始显示
//       	src = p + (by_max-1) * mw +bx_max-1;
//       for(row=0;row<dw;row++)
// 			{
// 			  for(col=0;col<dh;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src--;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dw;
// 			src-= mw-dw;
// 			}

				
				
				
// 				 break;

// 				case 3: //TRANS_ROT270: //270度转动； 
// 				x_max=x+h;
// 			 y_max=y+w;
// 				 //从左上角显示
// 				dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 				//从右上角开始显示
//       	src = p + bx_max-1 -row;
//       for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src+=mw;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dw;
// 			src = p + bx_max-1-row;
// 			}

				
				
				
// 				 break;

// 				case 4: //TRANS_MIRROR: //左右翻转； 
// 				x_max=x+w;
// 			 y_max=y+h;
// 				 //从左上角显示
// 				dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 				//从右上角开始显示
//       	src = p + bx_max-1 ;
//       for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src--;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dw;
// 			src += dw+mw;
// 			}

				
				
				
// 				 break;

// 				case 5: //TRANS_MIRROR_ROT90: //左右翻转后90度转动； 
// 				x_max=x+h;
// 			 y_max=y+w;
// 				 //从左上角显示
// 				dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 				//从右下角开始显示
//       	src = p + ((by_max-1) * mw +bx_max-1);
//       for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src-=mw;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dw;
// 			src =p+((by_max-1)*mw+bx_max-1-row);
// 			}

				
				
				
// 				 break;

// 				case 6:  //TRANS_MIRROR_ROT180: //左右翻转后180度转动； 
// 				x_max=x+w;
// 			 y_max=y+h;
// 				 //从左上角显示
// 				dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 				//从左下角开始显示
//       	src = p + ((by_max-1) * mw +bx);
//       for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src++;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dh;
// 			src -= dw+mw;
// 			}

				
				
				
// 				 break;

// 				case 7: //TRANS_MIRROR_ROT270: //左右翻转后270度转动。 
// 				x_max=x+h;
// 			 y_max=y+w;
//  //从左上角显示
// 				dest = w_getScreenBuffer() + (dy * SCRW + dx);
// 				//从左上角开始显示
//       	src = p + ((by) * mw +bx);
//       for(row=0;row<dh;row++)
// 			{
// 			  for(col=0;col<dw;col++)
// 				{
// 				 *dest=*src;
// 				 dest++; src+=mw;
// 				// 	mr_screenBuf +col*mr_screen_w + row = *(ptr+ col*mw +row);
// 				}
// 			dest += SCRW - dh;
// 			src =p+((by)*mw+bx+row+1);
// 			}

				
				
				
// 				 break;

// 			  break;
			  
// 		   }
// 		 }
		 
// 		 return 0;
// 		}
		


// int mrc_bitmapShowEx(uint16* p,
//    int16 x,
//    int16 y,
//    int16 mw,
//    int16 w,
//    int16 h,
//    uint16 rop,
//    int16 sx,
//    int16 sy
// )
// {
// 	return mrc_bitmapShowFlip(p,x,y,mw,w,h,rop,sx,sy,0);
// }

// int32 mrc_wstrlen(const char *txt) {
//     int32 i = 0;
//     if (txt == NULL) return 0;
//     while (txt[i] || txt[i + 1]) i += 2;

//     return i;
// }

// int _dtext(const char *pcText, int x, int y, int r, int g, int b, int is_unicode, int font) {
//     uint16 *un_text = pcText;
// 	uint32 temp_len = 0;
//     if(!is_unicode){
// 		temp_len = strlen(pcText)*2+2;
// 		un_text = malloc(temp_len);
//         UTF8ToUni(pcText, un_text,temp_len);
//     }
//     int32 len = mrc_wstrlen(un_text);
//     int ix = x;
//     int iy = y;
//     uint16 ncolor = MAKERGB565(r, g, b);
//     int fontw, fonth;
//     for (int i = 0; i < len/2; i++) {
//         uint16 id = un_text[i] << 8 | un_text[i] >> 8;
//         _mr_table->mr_platDrawChar(id, ix, iy, ncolor);
//         _mr_table->mr_getCharBitmap(id, font, &fontw, &fonth);
//         ix += fontw;
//     }
// 	if(!is_unicode){
// 		free(un_text);
// 	}
//     return 0;
// }

// int _dtextline(const char *pcText, int x, int y, int r, int g, int b, int is_unicode, int font) {
//     uint16 *un_text = pcText;
//     int32 len = mrc_wstrlen((const char*)un_text);
// 	uint32 temp_len = 0;
//     if(!is_unicode){
// 		temp_len = strlen(pcText)*2+2;
// 		un_text = malloc(temp_len);
//         UTF8ToUni(pcText, un_text,temp_len);
//     }
//     int ix = x;
//     int iy = y;
//     uint16 ncolor = MAKERGB565(r, g, b);
//     int fontw, fonth;
//     for (int i = 0; i < len/2; i++) {
//         uint16 id = un_text[i] << 8 | un_text[i] >> 8;
//         _mr_table->mr_platDrawChar(id, ix, iy, ncolor);
//         _mr_table->mr_getCharBitmap(id, font, &fontw, &fonth);
//         ix += fontw;
//         if (id == '\n') {
// 		    ix = x;
//             iy += fonth + 2;
//         }
//     }
// 	if(!is_unicode){
// 		free(un_text);
// 	}
//     return 0;
// }

// void *mrc_malloc(uint32 len){
//     void *data = mallocdata(len+sizeof(uint32));\
//     return data+sizeof(uint32);
// }

// void mrc_free(void *data){
//     freedata(data - sizeof(int32), *((uint32*)data));
// }

void base_init() {
    // SCRW = *(_mr_table->mr_screen_w);
    // SCRH = *(_mr_table->mr_screen_h);
    // SCREEN_BIT = *(_mr_table->mr_screen_bit);
    // mallocdata = (void *)_mr_table->mr_malloc;
    // freedata = (void *)_mr_table->mr_free;
    // realloc = (void *)_mr_table->mr_realloc;
    // memcpy = (void *)_mr_table->memcpy;
    // memmove = (void *)_mr_table->memmove;
    // strcpy = (void *)_mr_table->strcpy;
    // strncpy = (void *)_mr_table->strncpy;
    // strcat = _mr_table->strcat;
    // strncat = _mr_table->strncat;
    // memcmp = _mr_table->memcmp;
    // strcmp = _mr_table->strcmp;
    // strncmp = _mr_table->strncmp;
    // strcoll = _mr_table->strcoll;
    // memchr = _mr_table->memchr;
    // memset = _mr_table->memset;
    // strlen = _mr_table->strlen;
    // strstr = _mr_table->strstr;
    // strtoul = _mr_table->strtoul;
    // rand = _mr_table->rand;
    // wap = _mr_table->mr_connectWAP;

    // mrc_open = _mr_table->mr_open;
    // mrc_read = _mr_table->mr_read;
    // mrc_seek = _mr_table->mr_seek;
    // mrc_write = _mr_table->mr_write;
    // mrc_close = _mr_table->mr_close;

	// mrc_readFileFromMrp = _mr_table->_mr_readFile;
    

    // mr_platDrawChar = _mr_table->mr_platDrawChar;
    // mrc_menuCreate = _mr_table->mr_menuCreate;
    // mrc_menuSetItem = _mr_table->mr_menuSetItem;
    // mrc_menuShow = _mr_table->mr_menuShow;
    // mrc_menuSetFocus = _mr_table->mr_menuSetFocus;
    // mrc_menuRelease = _mr_table->mr_menuRelease;
    // mrc_menuRefresh = _mr_table->mr_menuRefresh;
    // mrc_dialogCreate = _mr_table->mr_dialogCreate;
    // mrc_dialogRelease = _mr_table->mr_dialogRelease;
    // mrc_dialogRefresh = _mr_table->mr_dialogRefresh;
    // mrc_textCreate = _mr_table->mr_textCreate;
    // mrc_textRelease = _mr_table->mr_textRelease;
    // mrc_textRefresh = _mr_table->mr_textRefresh;
    // mrc_editCreate = _mr_table->mr_editCreate;
    // mrc_editRelease = _mr_table->mr_editRelease;
    // mrc_editGetText = _mr_table->mr_editGetText;
    // mrc_initNetwork = _mr_table->mr_initNetwork;
    // mrc_closeNetwork = _mr_table->mr_closeNetwork;
    // mrc_getHostByName = _mr_table->mr_getHostByName;
    // mrc_socket = _mr_table->mr_socket;
    // mrc_connect = _mr_table->mr_connect;
    // mrc_closeSocket = _mr_table->mr_closeSocket;
    // mrc_recv = _mr_table->mr_recv;
    // mrc_recvfrom = _mr_table->mr_recvfrom;
    // mrc_send = _mr_table->mr_send;
    // mrc_sendto = _mr_table->mr_sendto;


//     printf = _mr_table->mr_printf;
//     sprintf = _mr_table->sprintf;
//     atoi = _mr_table->atoi;
//     dtext = _dtext;
//     dtextline = _dtextline;
//     getuptime = _mr_table->mr_getTime;
//      remove = _mr_table->mr_remove;
//  getlen = _mr_table->mr_getLen;
//  rename = _mr_table->mr_rename;
//  mkDir = _mr_table->mr_mkDir;
//  rmDir = _mr_table->mr_rmDir;
    // getdatetime = _mr_table->mr_getDateTime;
    // mr_getUserInfo = _mr_table->mr_getUserInfo;
    // sleep = _mr_table->mr_sleep;
    // mrc_plat = _mr_table->mr_plat;
    // mrc_platEx = _mr_table->mr_platEx;
    // mr_ferrno = _mr_table->mr_ferrno;
    // mrc_findStart = _mr_table->mr_findStart;
    // mrc_findGetNext = _mr_table->mr_findGetNext;
    // mrc_findStop = _mr_table->mr_findStop;
    // mrc_exit = _mr_table->mr_exit;
    // mrc_getScreenInfo = _mr_table->mr_getScreenInfo;
}

// outFuncs_st *_start(inFuncs_st *in) {
//     outFuncs_st *ret = in->mrc_malloc(sizeof(outFuncs_st));
//     ret->mrc_init = mrc_init;
//     ret->mrc_event = mrc_event;
//     ret->mrc_exitApp = mrc_exitApp;
//     ret->mrc_pause = mrc_pause;
//     ret->mrc_resume = mrc_resume;

//     // 把一些常用的函数放到全局变量
//     _mr_table = in->mr_table;
//     // timercreate = in->mrc_timerCreate;
//     // timerstart = in->mrc_timerStart;
//     // timersettime = in->mrc_timerSetTimeEx;
//     // timerstop = in->mrc_timerStop;
//     // timerdel = in->mrc_timerDelete;

//     base_init();

//     return ret;
// }

// int32 mrc_init(void) {
//     return init();
// }

// int32 mrc_pause(void) {
//     return pause();
// }

// int32 mrc_resume(void) {
//     return resume();
// }

// int32 mrc_exitApp(void) {
//     return exitApp();
// }

// int32 mrc_event(int32 code, int32 p0, int32 p1) {
//     return event((int)code, (int)p0, (int)p1);
// }
