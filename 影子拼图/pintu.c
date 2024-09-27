#include <mrc_base.h>
#include <mrc_graphics.h>
#include "mpc.h"
#include "pintu.h"
#include "uc3_font.h"

extern void switchWindow(int windex, int leve);
void pintu_init(PuzzleGame *game, char *filename, int leve)
{
    game->rows = 3 + leve;
    game->columns = 3 + leve;
    game->emptyIndex = game->rows * game->columns - 1;
    game->image = readBitmapFromAssets(filename);
    loadImageParts(game);
    shuffle(game);
}

void loadImage(PuzzleGame *game)
{
    // 加载图像逻辑，替换为适当的图像加载代码
    // 假设图像加载成功
}

void loadImageParts(PuzzleGame *game)
{
    int i = 0;
    int column;
    int row;
    int itemw;
    BITMAPINFO bitmapInfo;
    game->imageParts = malloc(game->rows * game->columns * sizeof(ClipImage));
    bitmapGetInfo(game->image, &bitmapInfo);
    itemw = bitmapInfo.width / game->columns;
    for (i = 0; i < game->rows * game->columns; i++)
    {
        column = i % game->columns;
        row = i / game->columns;
        // 这里使用伪代码替换图像获取逻辑
        game->imageParts[i].image = game->image;
        game->imageParts[i].cw = itemw;
        game->imageParts[i].ch = itemw;
        game->imageParts[i].cx = column * itemw;
        game->imageParts[i].cy = row * itemw;
    }
}

void shuffle(PuzzleGame *game)
{
    int i, j;
    int32 direction[4];
    direction[0] = MR_KEY_UP;
    direction[1] = MR_KEY_DOWN;
    direction[2] = MR_KEY_LEFT;
    direction[3] = MR_KEY_RIGHT;
    for (i = 1000; i > 0; i--)
    {
        j = rand() % 4;

        pintu_doDirection(game, direction[j], FALSE);
    }
}

void movePiece(PuzzleGame *game, int index)
{
    if (canMove(game, index))
    {
        ClipImage temp = game->imageParts[index];
        game->imageParts[index] = game->imageParts[game->emptyIndex];
        game->imageParts[game->emptyIndex] = temp;
        game->emptyIndex = index;
    }
}

int canMove(PuzzleGame *game, int index)
{

    int row = index / game->columns;
    int col = index % game->columns;
    int emptyRow = game->emptyIndex / game->columns;
    int emptyCol = game->emptyIndex % game->columns;
    mrc_printf("canmove %d", index);
    if (index < 0 || index >= game->rows * game->rows)
        return 0;
    return (row == emptyRow && (col - 1 == emptyCol || col + 1 == emptyCol)) ||
           (col == emptyCol && (row - 1 == emptyRow || row + 1 == emptyRow));
}

// 绘图界面
void pintu_draw(PuzzleGame *game)
{
    BITMAPINFO bitmapInfo;
    ClipImage tempImage;
    int32 itemw = 0;
    int i = 0;
    bitmapGetInfo(game->image, &bitmapInfo);
    mrc_clearScreen(240, 240, 240);
    // 绘制图块
    for (i = 0; i < game->rows * game->columns; i++)
    {
        tempImage = game->imageParts[i];
        itemw = bitmapInfo.width / game->columns;
        // mrc_printf("itemw = %d, ", itemw);
        if (i != game->emptyIndex)
            drawBitmapEx(tempImage.image, i % game->columns * itemw, i / game->columns * itemw, tempImage.cw, tempImage.ch, tempImage.cx, tempImage.cy, tempImage.cw, tempImage.ch);
    }

    // 绘制缩略图片
    drawBitmapEx(game->image, SCRW / 2 - 30, SCRW + 4, 60, 60, 0, 0, bitmapInfo.width, bitmapInfo.height);

    uc3_drawText("菜单", 2, SCRH - 18, 20, 20, 20, 0);
    uc3_drawText("重新开始", SCRW - 2 - 64, SCRH - 18, 20, 20, 20, 0);
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

int32 pintu_doDirection(PuzzleGame *game, int32 p1, int32 isDraw)
{
    if (p1 == MR_KEY_DOWN)
    {
        if (canMove(game, game->emptyIndex - game->columns))
        {
            movePiece(game, game->emptyIndex - game->columns);
            if(isDraw)
            pintu_draw(game);
        }
    }
    if (p1 == MR_KEY_UP)
    {
        if (canMove(game, game->emptyIndex + game->columns))
        {
            movePiece(game, game->emptyIndex + game->columns);
            if(isDraw)
            pintu_draw(game);
        }
    }
    if (p1 == MR_KEY_RIGHT)
    {
        if (canMove(game, game->emptyIndex - 1))
        {
            movePiece(game, game->emptyIndex - 1);
            if(isDraw)
            pintu_draw(game);
        }
    }
    if (p1 == MR_KEY_LEFT)
    {
        if (canMove(game, game->emptyIndex + 1))
        {
            movePiece(game, game->emptyIndex + 1);
            if(isDraw)
            pintu_draw(game);
        }
    }
    return 0;
}

int32 pintu_event(PuzzleGame *game, int32 type, int32 p1, int32 p2)
{
    int32 itemw = 0;
    int32 ix, iy;
    int32 iindex;
    mrc_printf("pintu_event: %d %d %d\n", type, p1, p2);
    if (type == MR_KEY_RELEASE)
    {
        if (p1 == MR_KEY_UP || p1 == MR_KEY_DOWN || p1 == MR_KEY_LEFT || p1 == MR_KEY_RIGHT)
        {
            pintu_doDirection(game, p1, TRUE);
        }
        if (p1 == MR_KEY_SOFTLEFT)
        {
            switchWindow(0, 0);
        }
        // 点击重新开始
        if (p1 == MR_KEY_SOFTRIGHT)
        {
            shuffle(game);
            pintu_draw(game);
        }
    }
    if (type == MR_MOUSE_DOWN)
    {
        itemw = SCRW / game->columns;
        ix = p1 / itemw;
        iy = p2 / itemw;
        iindex = iy * game->columns + ix;
        if (canMove(game, iindex))
        {
            movePiece(game, iindex);
            pintu_draw(game);
        }
    }
    if (type == MR_MOUSE_UP)
    {
        // 点击菜单
        if (p1 >= 0 && p1 <= 64 && p2 >= SCRH - 18 && p2 <= SCRH)
        {
            switchWindow(0, 0);
        }
        // 点击重新开始
        if (p1 >= SCRW - 68 && p1 <= SCRW && p2 >= SCRH - 18 && p2 <= SCRH)
        {
            shuffle(game);
            pintu_draw(game);
        }
    }
    return 0;
}

int pintu_main(void)
{
    PuzzleGame game;
    pintu_init(&game, "sample_image.bma", 0);

    // 游戏的其他逻辑和循环可以在此处添加

    // 释放内存
    free(game.imageParts);

    return 0;
}

int32 pintu_free(PuzzleGame *game)
{
    // 释放内存
    free(game->imageParts);
    bitmapFree(game->image);
    return 0;
}