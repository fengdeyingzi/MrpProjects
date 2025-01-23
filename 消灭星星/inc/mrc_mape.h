#ifndef _MAPE_H_
#define _MAPE_H_

#include "mrc_base.h"
#include "mrc_file.h"
extern int SCRW, SCRH;

//斯凯网游戏：英雄联盟MAPE地图结构
//需要mape, png同时存在
//暂时：mape在mythroad目录，png放mrp内部
//pwd, aef
//pwd是png图片, aef是配置文件暂时不知道解析规则
typedef struct _mape
{
  char *dir; //当前地图所在文件夹，必填

  char *path; //地图文件路径
  char *name;

  int32 bkg; //地图背景色

  int32 mapw; //地图宽高以及地图块宽高
  int32 maph;
  int32 itemw;
  int32 itemh;

  char *layname; //层次名
  char *resname; //资源名
  char *restype; //资源类型
  int32 resnum;  //资源个数
  char **resarr; //资源名称数组
  void **resdata; //资源数据 MRC_TPNG

  int32 *data; //地图数据

  char *end; //未知的地图结构数据
  int32 endlen;
} _MAPE;

//加载mape文件
extern _MAPE *loadMape(char *name);

//打印mape信息
extern int printMape(_MAPE *mape);

//绘制mape到屏幕
extern int drawMape(_MAPE *mape);
extern int drawMapePos(_MAPE *mape, int32 dx, int32 dy);

//保存png为地图文件
extern int pngToMape(_MAPE *mape, char *png);

//保存mape地图文件
extern int saveMape(_MAPE *mape, char *name);

//释放mape结构体
extern int freeMape(_MAPE *mape);

#endif

