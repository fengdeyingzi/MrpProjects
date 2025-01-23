#ifndef MRC_SPRITE_H
#define MRC_SPRITE_H
#include "mrc_base.h"
#include "mrc_file.h"

//��Ƭ��Ϣ��ԴͼƬ�и�ɼ���ͼ
typedef struct spriteCoord
{
    int16 dx; // ��ͼƬ���Xƫ��
    int16 dy; // ��ͼƬ���Yƫ��
    int16 width;   // ��
    int16 height;  // ��
} SpriteCoord;

//16�ֽ�
typedef struct spriteFrameFile
{
    //��Ƭ��SpriteCoordinate[]�е�����ֵ
    int32 coord_index;
    //���Ƶ�λ��
    int32 x;
    int32 y;
    //����
    int32 transform;
} SpriteFrameFile;

//16�ֽ�
typedef struct spriteFrameCol
{
    int32 x;
    int32 y;
    int32 w;
    int32 h;
} SpriteFrameCol;

//8�ֽ�
typedef struct spriteFrameRPT
{
    int32 rpx;
    int32 rpy;
} SpriteFrameRPT;


//֡��Ϣ
typedef struct spriteFrame
{
    //ÿһ֡����Ƭ����
    int8 tile_count;
    //��ײ���εĸ���
    int8 coll_count;
    //�ο������
    int8 rpt_count;

    //��ߵ�
    int8 top;
    //��͵�
    int8 bottom;
    //�����
    int8 left;
    //���ҵ�
    int8 right;

    //ÿһ֡����Щ��Ƭ��ɡ�
    SpriteFrameFile *tile;
    //��ײ����
    SpriteFrameCol *coll;
    //�ο���
    SpriteFrameRPT *rpt;

} SpriteFrame;

typedef struct spriteAct 
{	
	//ÿ��������֡��
	int32 frame_count;
	//ÿһ֡���ӳ�:
	int8 delay;
	//���񣬷Ƕ��뷽ʽ:
	int32 transform;
	//����������SpriteFrame[]��Index.
	int32 *frame;
}SpriteAct;

typedef struct spriteX
{
    int32 coord_count;
    SpriteCoord *coord; // SpriteCoord ��Ƭ����Ƭ����

    int32 frame_count;
    SpriteFrame *frame; // SpriteFrame ֡��֡��

    int32 action_count; // SpriteAct ����������
    SpriteAct *action;
} SpriteX;

extern SpriteX *loadSpxFile(char *filename);

#endif
