//SpriteX V3.9

#include "mrc_spritex.h"

SpriteX *loadSpxFile(char *filename)
{
	int32 file = 0;
	int32 file_pos = 0;
	int32 i, j, k, m, n, x, y, data;
	SpriteX *sprite;
	SpriteFrame *frame;
	SpriteAct *action;
	SpriteCoord *coord;
	SpriteFrameFile *tile;
	SpriteFrameCol *coll;
	SpriteFrameRPT *rpt;

	file = mrc_open(filename, MR_FILE_RDONLY);
	mrc_printf("loadSpxFile(%s)->open:%p", filename, file);
	sprite = (SpriteX *)mrc_malloc(sizeof(SpriteX));
	file_pos = 6;

	//��ȡ��Ƭ����
	sprite->coord_count = mrc_readFileInt32(file, file_pos);
	file_pos += 4;
	mrc_printf("loadSpxFile->coord:%d", sprite->coord_count);

	//��ȡ��Ƭ����
	sprite->coord = mrc_malloc(sizeof(SpriteCoord) * sprite->coord_count);
	for (i = 0; i < sprite->coord_count; i++)
	{
		coord = sprite->coord + i;
		coord->dx = mrc_readFileInt16(file, file_pos);
		coord->dy = mrc_readFileInt16(file, file_pos + 2);
		coord->width = mrc_readFileInt16(file, file_pos + 4);
		coord->height = mrc_readFileInt16(file, file_pos + 6);

		file_pos += sizeof(SpriteCoord);
		mrc_printf("coord%d:%d, %d, %d, %d", i, coord->dx, coord->dy, coord->width, coord->height);
	}

	//��ȡ֡��
	sprite->frame_count = mrc_readFileInt32(file, file_pos);
	mrc_printf("loadSpxFile->frame:%d", sprite->frame_count);

	//��ȡ֡����
	file_pos += 4;
	sprite->frame = mrc_malloc(sizeof(SpriteFrame) * sprite->frame_count);
	for (i = 0; i < sprite->frame_count; i++)
	{
		frame = (SpriteFrame *)sprite->frame + i * sprite->frame_count;

		//��ȡ֡7������
		frame->tile_count = mrc_readFileInt32(file, file_pos);
		frame->coll_count = mrc_readFileInt32(file, file_pos + 4);
		frame->rpt_count = mrc_readFileInt32(file, file_pos + 4);
		frame->top = mrc_readFileInt32(file, file_pos + 4);
		frame->bottom = mrc_readFileInt32(file, file_pos + 4);
		frame->left = mrc_readFileInt32(file, file_pos + 4);
		frame->right = mrc_readFileInt32(file, file_pos + 4);
		mrc_printf("frame%d:%d, %d, %d", i, frame->tile_count, frame->coll_count, frame->rpt_count);
		file_pos += 7 * 4;
	}

	/*
	//��ʼ��ȡ֡������
	for (i = 0; i < sprite->frame_count; i++)
	{
		frame = (SpriteFrame *)sprite->frame + i * sprite->frame_count;

		//��ȡ֡����Ƭ����
		if (frame->tile_count > 0)
			frame->tile = (SpriteFrameFile *)mrc_malloc(sizeof(SpriteFrameFile) * frame->tile_count);
		for (j = 0; j < frame->tile_count; j++)
		{
			tile = frame->tile + j;
			mrc_readFileNot(file, file_pos, sizeof(SpriteFrameFile), tile);
			file_pos += sizeof(SpriteFrameFile);
		}

		//��ȡ֡����ײ����
		if (frame->coll_count > 0)
			frame->coll = (SpriteFrameCol *)mrc_malloc(sizeof(SpriteFrameCol) * frame->coll_count);
		for (k = 0; k < frame->coll_count; k++)
		{
			coll = frame->coll + k;
			mrc_readFileNot(file, file_pos, sizeof(SpriteFrameCol), coll);
			file_pos += sizeof(SpriteFrameCol);
		}

		//��ȡ֡�ڲο���
		if (frame->rpt_count > 0)
			frame->rpt = (SpriteFrameRPT *)mrc_malloc(sizeof(SpriteFrameRPT) * frame->rpt_count);
		for (m = 0; m < frame->coll_count; m++)
		{
			rpt = frame->rpt + m;
			mrc_readFileNot(file, file_pos, sizeof(SpriteFrameRPT), rpt);
			file_pos += sizeof(SpriteFrameRPT);
		}
		//֡���ݽ���
	}
*/
	//��ȡ����
	sprite->action_count = mrc_readFileInt32(file, file_pos);
	sprite->action = mrc_malloc(sizeof(SpriteAct) * sprite->action_count);
	mrc_printf("loadSpxFile->action:%d", sprite->action_count);

	//��ȡ��������
	file_pos += 4;
	for (i = 0; i < sprite->action_count; i++)
	{
		action = (SpriteAct *)sprite->action + i * sprite->action_count;

		//��ȡ9�ֽڵ�����
		action->frame_count = mrc_readFileInt32(file, file_pos);
		action->delay = mrc_readFileInt8(file, file_pos + 4);
		action->transform = mrc_readFileInt32(file, file_pos + 1);
		file_pos += 9;
		mrc_printf("action%d:%d, %d, %d", i, action->frame_count, action->delay, action->transform);

		//��ȡ��Ӧ֡����
		action->frame = malloc(action->frame_count);
		mrc_readFileNot(file, file_pos, action->frame_count, action->frame);
		file_pos += action->frame_count;
	}

	mrc_close(file);
	mrc_printf("loadSpxFile(%s)->end", filename);
	return sprite;
}
