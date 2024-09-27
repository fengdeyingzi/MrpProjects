#ifndef _PINTU_H_
#define _PINTU_H_
#include <mrc_base.h>

typedef struct
{
    int32 image; // �滻Ϊ�ʵ���ͼ������
    int cw;
    int ch;
    int cx;
    int cy;
} ClipImage;

typedef struct
{
    ClipImage *imageParts;
    int rows;
    int columns;
    int emptyIndex;
    int32 image; // �滻Ϊ�ʵ���ͼ������
} PuzzleGame;

// ��������
extern void pintu_init(PuzzleGame *game, char *filename, int leve);
extern void loadImageParts(PuzzleGame *game);
extern void shuffle(PuzzleGame *game);
extern void movePiece(PuzzleGame *game, int index);
extern int canMove(PuzzleGame *game, int index);
extern void loadImage(PuzzleGame *game);
int32 pintu_doDirection(PuzzleGame *game, int32 p1, int32 isDraw);
void pintu_draw(PuzzleGame *game);
int32 pintu_event(PuzzleGame *game, int32 type, int32 p1, int32 p2);
int32 pintu_free(PuzzleGame *game);

#endif
