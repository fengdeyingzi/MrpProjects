#ifndef MRC_SPRITE_H
#define MRC_SPRITE_H
#include "mrc_base.h"
#include "mrc_file.h"

//切片信息，源图片切割成几个图
typedef struct spriteCoord
{
    int16 dx; // 在图片里的X偏移
    int16 dy; // 在图片里的Y偏移
    int16 width;   // 宽
    int16 height;  // 高
} SpriteCoord;

//16字节
typedef struct spriteFrameFile
{
    //切片在SpriteCoordinate[]中的索引值
    int32 coord_index;
    //绘制的位置
    int32 x;
    int32 y;
    //镜像
    int32 transform;
} SpriteFrameFile;

//16字节
typedef struct spriteFrameCol
{
    int32 x;
    int32 y;
    int32 w;
    int32 h;
} SpriteFrameCol;

//8字节
typedef struct spriteFrameRPT
{
    int32 rpx;
    int32 rpy;
} SpriteFrameRPT;


//帧信息
typedef struct spriteFrame
{
    //每一帧的切片个数
    int8 tile_count;
    //碰撞矩形的个数
    int8 coll_count;
    //参考点个数
    int8 rpt_count;

    //最高点
    int8 top;
    //最低点
    int8 bottom;
    //最左点
    int8 left;
    //最右点
    int8 right;

    //每一帧由那些切片组成。
    SpriteFrameFile *tile;
    //碰撞矩形
    SpriteFrameCol *coll;
    //参考点
    SpriteFrameRPT *rpt;

} SpriteFrame;

typedef struct spriteAct 
{	
	//每个动作总帧数
	int32 frame_count;
	//每一帧的延迟:
	int8 delay;
	//镜像，非对齐方式:
	int32 transform;
	//这里面存的是SpriteFrame[]的Index.
	int32 *frame;
}SpriteAct;

typedef struct spriteX
{
    int32 coord_count;
    SpriteCoord *coord; // SpriteCoord 切片，切片个数

    int32 frame_count;
    SpriteFrame *frame; // SpriteFrame 帧，帧数

    int32 action_count; // SpriteAct 动作，个数
    SpriteAct *action;
} SpriteX;

extern SpriteX *loadSpxFile(char *filename);

#endif
