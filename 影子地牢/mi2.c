#include <mrc_base.h>
#include "mrc_graphics.h"
#include "mpc.h"
#include "uc3_font.h"
#include "logo.h"
#include "spritex.h"

#define MAX_ROW 20
#define MAX_COL 20
#define true 1
#define false 0
typedef int bool;
SpriteX *spx;
enum
{
    GAME_RUNING,
    GAME_OVER
};

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    bool passable; // 小球是否能通过
} Lattice;

typedef struct
{
    int x; // 横坐标
    int y; // 纵坐标
} Ball;

typedef struct
{
    Lattice maze[MAX_ROW + 2][MAX_COL + 2];
    Ball ball;
    Point entrance;
    Point exit;
    int rowNumber;
    int colNumber;
} Maze;

// 函数声明
void initializeMaze(Maze *maze);
void printMaze(Maze *maze);
bool isOutOfBorder(Point *p, Maze *maze);
void moveBall(Maze *maze, int direction);
bool isWin(Maze *maze);
void gameOverMessage(Maze *maze);
void createMaze(Maze *maze);
void drawGame(Maze *maze);
int game_state;
Maze maze;

int32 game_init()
{
    initializeMaze(&maze);
    createMaze(&maze);
    printMaze(&maze);
    drawGame(&maze);
    return 0;
}
int32 mrc_init()
{

    mpc_init();
    uc3_init();
    logo_draw();
    spx = createSpriteXWithImageName("a.sprite", "a.bma");
    setVisible(spx, 1);
    update(spx, 100);
    setAction(spx, 0);
    setPosition(spx, SCRW / 2, SCRH / 2);

    return 0;
}

// 初始化迷宫
void initializeMaze(Maze *maze)
{
    int i, j;
    maze->rowNumber = MAX_ROW;
    maze->colNumber = MAX_COL;
    maze->entrance.x = 0; // 入口
    maze->entrance.y = 1;
    maze->exit.x = maze->colNumber + 1; // 出口
    maze->exit.y = maze->rowNumber;

    // 设置所有格子为不可通行（墙）
    for (i = 0; i < maze->rowNumber + 2; i++)
    {
        for (j = 0; j < maze->colNumber + 2; j++)
        {
            maze->maze[i][j].passable = false;
        }
    }

    // 打开入口和出口
    maze->maze[maze->entrance.y][maze->entrance.x].passable = true;
    maze->maze[maze->exit.y][maze->exit.x].passable = true;

    // 设置小球的起始位置为入口
    maze->ball.x = maze->entrance.x;
    maze->ball.y = maze->entrance.y;
}

// 绘制迷宫
void drawGame(Maze *maze)
{
    int mapw;
    int ix, iy;
    cls(240, 240, 240);
    mapw = SCRW / (maze->rowNumber + 3);
    for (iy = 0; iy < maze->rowNumber + 2; iy++)
    {
        for (ix = 0; ix < maze->colNumber + 2; ix++)
        {

            if (ix == maze->ball.x && iy == maze->ball.y)
            {
                gl_drawCir(ix * mapw + mapw / 2, iy * mapw + mapw / 2, mapw / 2 - 1, 0xfff04080);
                printf("o "); // 小球
            }
            else if (iy == maze->entrance.y && ix == maze->entrance.x)
            {
                gl_drawRect(ix * mapw, iy * mapw, mapw, mapw, 0xff00ff00);
                printf("s "); // 入口
            }
            else if (iy == maze->exit.y && ix == maze->exit.x)
            {
                gl_drawRect(ix * mapw, iy * mapw, mapw, mapw, 0xffff0000);
                printf("e "); // 出口
            }
            else if (maze->maze[iy][ix].passable)
            {
                gl_drawRect(ix * mapw, iy * mapw, mapw, mapw, 0xffffffff);
                printf("  "); // 可通过的空间
            }
            else
            {
                gl_drawRect(ix * mapw, iy * mapw, mapw, mapw, 0xff000000);
                printf("* "); // 墙壁
            }
        }
        gl_drawRect((ix)*mapw, iy * mapw, mapw, mapw, 0xff000000);
    }
    gl_drawRect(0, (maze->rowNumber + 2) * mapw, (maze->colNumber + 3) * mapw, mapw, 0xff000000);
    if (game_state == GAME_OVER)
    {
        uc3_drawText("游戏结束", 10, 10, 20, 240, 20, 0);
    }
    setPosition(spx, SCRW / 2, SCRH / 2);
    setAction(spx, 0);

    paint(spx, spx->image, SCRW / 2, SCRH / 2);
    update(spx, getuptime());
    ref(0, 0, SCRW, SCRH);
}

// 打印迷宫
void printMaze(Maze *maze)
{
    int i, j;

    for (i = 0; i < maze->rowNumber + 2; i++)
    {
        for (j = 0; j < maze->colNumber + 2; j++)
        {
            if (j == maze->ball.x && i == maze->ball.y)
            {
                printf("o "); // 小球
            }
            else if (i == maze->entrance.y && j == maze->entrance.x)
            {
                printf("s "); // 入口
            }
            else if (i == maze->exit.y && j == maze->exit.x)
            {
                printf("e "); // 出口
            }
            else if (maze->maze[i][j].passable)
            {
                printf("  "); // 可通过的空间
            }
            else
            {
                printf("* "); // 墙壁
            }
        }
        printf("*\n");
    }
    printf("Ball position: (%d, %d)\n", maze->ball.x, maze->ball.y);
}

// 检查是否出界
bool isOutOfBorder(Point *p, Maze *maze)
{
    return (p->x < 0 || p->x > maze->colNumber + 1 ||
            p->y < 0 || p->y > maze->rowNumber + 1);
}

// 移动小球
void moveBall(Maze *maze, int direction)
{
    Ball newPosition = maze->ball;

    switch (direction)
    {
    case 0: // 上
        newPosition.y -= 1;
        break;
    case 1: // 下
        newPosition.y += 1;
        break;
    case 2: // 左
        newPosition.x -= 1;
        break;
    case 3: // 右
        newPosition.x += 1;
        break;
    }

    if (!isOutOfBorder((Point *)&newPosition, maze) && maze->maze[newPosition.y][newPosition.x].passable)
    {
        maze->ball = newPosition;
    }
    else
    {
        printf("Can't move there!\n");
    }
}

// 检查是否赢得游戏
bool isWin(Maze *maze)
{
    return (maze->ball.x == maze->exit.x && maze->ball.y == maze->exit.y);
}

// 游戏结束消息
void gameOverMessage(Maze *maze)
{
    game_state = GAME_OVER;
    drawGame(maze);
}

// 使用 DFS 算法生成迷宫
void createMaze(Maze *maze)
{

    Point stack[MAX_ROW * MAX_COL]; // 用于存储路径
    int top = -1;
    int found;
    int i;
    int randIndex;
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 上、下、左、右
    // 从一个随机起点开始
    Point start;
    Point direction;
    start.x = 2 * (rand() % (maze->colNumber / 2)) + 1;
    start.y = 2 * (rand() % (maze->rowNumber / 2)) + 1;
    maze->maze[start.y][start.x].passable = true;
    stack[++top] = start;
    srand(time(NULL));
    while (top >= 0)
    {
        Point currentPoint = stack[top]; // 当前点
        Point nextPoint;
        found = false;

        // 随机选择未访问的相邻单元

        for (i = 0; i < 4; i++)
        {
            randIndex = rand() % 4;
            direction.x = directions[randIndex][0];
            direction.y = directions[randIndex][1];

            nextPoint.x = currentPoint.x + direction.x * 2;
            nextPoint.y = currentPoint.y + direction.y * 2;

            if (!isOutOfBorder(&nextPoint, maze) && !maze->maze[nextPoint.y][nextPoint.x].passable)
            {
                maze->maze[currentPoint.y + direction.y][currentPoint.x + direction.x].passable = true; // 打开墙
                maze->maze[nextPoint.y][nextPoint.x].passable = true;                                   // 打开下一个单元
                stack[++top] = nextPoint;                                                               // 压入栈中
                found = true;
                break;
            }
        }

        if (!found)
        {
            top--; // 如果没有找到可走的单元，退回上一个单元
        }
    }
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    if (code == KY_UP)
    { // 检测是否有按键被按下
        switch (param0)
        {
        case _UP: // Up (箭头上)
            moveBall(&maze, 0);
            break;
        case _DOWN: // Down (箭头下)
            moveBall(&maze, 1);
            break;
        case _LEFT: // Left (箭头左)
            moveBall(&maze, 2);
            break;
        case _RIGHT: // Right (箭头右)
            moveBall(&maze, 3);
            break;
        case _SRIGHT: // Quit game
            mrc_exit();
        }
        printMaze(&maze);
        drawGame(&maze);
        if (isWin(&maze))
        {
            gameOverMessage(&maze);
        }
    }
    if (code == LOGO_EVENT)
    {
        game_init();
    }
    return 0;
}

int32 mrc_pause(void)
{
    return 0;
}

int32 mrc_resume(void)
{
    return 0;
}

int32 mrc_exitApp()
{

    uc3_free();

    return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5)
{
    return 0;
}