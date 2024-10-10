#include <mrc_base.h>
#include "mpc.h"
#include "mrc_graphics.h"
#include "uc3_font.h"
#include "xl_sound.h"
#define MAZE_WIDTH 40
#define MAZE_HEIGHT 40
typedef int bool;
#define false 0

#define true 1

int32 CELL_SIZE;

int32 timer;
BITMAP_565 *logo;
BITMAP_565 *logo2;
int rotation;
int alpha;
typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    bool passable; // 小球是否能通过
} Lattice;
Lattice *lattice;

/*
迷宫游戏实现
*/
void switchWindow(int windex, int leve) {}

// 绘制迷宫
void drawMaze(Lattice *mazeLattice, int colNumber, int rowNumber)
{
    int x, y; // 移动变量定义到函数顶部
    CELL_SIZE = SCRH / rowNumber;
    for (y = 0; y < rowNumber; y++)
    {
        for (x = 0; x < colNumber; x++)
        {
            if (mazeLattice[colNumber * y + x].passable)
            {
                mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 255, 255, 255); // 通路
            }
            else
            {
                mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 0, 0, 0); //
            }
            // if (maze[y][x] == 0) {
            //     // mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 0, 0, 0); // 墙壁
            // } else {
            //     mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 255, 255, 255); // 通路
            // }
        }
    }
}

bool isOutofBorder(int x, int y, int colNumber, int rowNumber)
{
    if ((x == 0 && y == 1) || (x == colNumber + 1 && y == rowNumber))
    {
        return false;
    }
    else
    {
        return (x > colNumber || y > rowNumber || x < 1 || y < 1);
    }
}

void setPassable(Lattice *lattice, bool value)
{
    lattice->passable = value;
}

bool isPassable(Lattice *lattice)
{
    return lattice->passable;
}

// DepthFirstSearchCreateMaze

Point arroundPoint(Lattice *mazeLattice, Point p, int colNumber, int rowNumber, int *stack, int *stackTop)
{
    Point tempPoint;
    int i,j,x,y;
    int arroundPoint[8]; // Adjust the array to represent both x and y changes
    int r = rand() % 4; // Get a random index from 0 to 3
    arroundPoint[0] = -2;
    arroundPoint[1] = 0;
    arroundPoint[2] = 2;
    arroundPoint[3] = 0;
    arroundPoint[4] = 0;
    arroundPoint[5] = -2;
    arroundPoint[6] = 0;
    arroundPoint[7] = 2;

    for ( i = 0; i < 4; ++i)
    {
         j = (r + i) % 4;  // Ensure we rotate through the possible directions
         x = p.x + arroundPoint[j * 2];        // Use j * 2 for x direction
         y = p.y + arroundPoint[j * 2 + 1];    // Use j * 2 + 1 for y direction

        if (!isOutofBorder(x, y, colNumber, rowNumber) && !isPassable(&mazeLattice[y * colNumber + x]))
        {
            setPassable(&mazeLattice[y * colNumber + x], true);
            setPassable(&mazeLattice[p.y + arroundPoint[j * 2 + 1] / 2 * colNumber + (p.x + arroundPoint[j * 2] / 2)], true);
            tempPoint.x = x;
            tempPoint.y = y;
            return tempPoint;
        }
    }
    
    // 返回一个无效的点，表示没有找到可用的点
    tempPoint.x = -1; 
    tempPoint.y = -1; 
    return tempPoint; 
}

void depthFirstSearchCreateMaze_createMaze(Lattice *mazeLattice, int colNumber, int rowNumber)
{
    Point currentPoint;
    int *stack = (int *)malloc((colNumber * rowNumber) * sizeof(int));
    int stackTop = -1;

    sand(getuptime()); // Initialize random seed
    currentPoint.x = 2 * (rand() % (colNumber / 2)) + 1;
    currentPoint.y = 2 * (rand() % (rowNumber / 2)) + 1;
    setPassable(&mazeLattice[currentPoint.y * (colNumber + 1) + currentPoint.x], true);

    stack[++stackTop] = currentPoint.y * (colNumber + 1) + currentPoint.x;
    currentPoint = arroundPoint(mazeLattice, currentPoint, colNumber, rowNumber, stack, &stackTop);

    while (true)
    {
        Point p = arroundPoint(mazeLattice, currentPoint, colNumber, rowNumber, stack, &stackTop);
        if (p.x != -1)
        {
            stack[++stackTop] = currentPoint.y * (colNumber + 1) + currentPoint.x; // Push current point to stack
            currentPoint = p;                                                      // Move to new point
        }
        else if (stackTop >= 0)
        {
            currentPoint.x = stack[stackTop] % (colNumber + 1);
            currentPoint.y = stack[stackTop] / (colNumber + 1); // Pop point from stack
            stackTop--;
        }
        else
        {
            break;
        }
    }

    free(stack);
}

// 绘制迷宫

// RandomizedPrimCreateMaze
void pushArroundWallToList(Lattice *mazeLattice, Point p, Point *list, int *listSize, int colNumber, int rowNumber)
{
    int arroundWall[4];
    int i, x, y;
    Point tempPoint;
    arroundWall[0] = 1;
    arroundWall[1] = 0;
    arroundWall[2] = 1;
    arroundWall[3] = 0;

    for (i = 0; i < 4; i += 2)
    {
        x = p.x + arroundWall[i];
        y = p.y + arroundWall[i + 1];
        if (!isOutofBorder(x, y, colNumber, rowNumber) && !isPassable(&mazeLattice[y * (colNumber + 1) + x]))
        {
            tempPoint.x = x;
            tempPoint.y = y;
            list[*listSize] = tempPoint;
            (*listSize)++;
        }
    }
}

Point findPoint(Point wall, Lattice *mazeLattice)
{
    int arroundWall[4];
    Point p;
    int i;
    bool add, sub;
    arroundWall[0] = 1;
    arroundWall[1] = 0;
    arroundWall[2] = 1;
    arroundWall[3] = 0;

    p.x = -1;
    p.y = -1; // Default invalid point
    for (i = (wall.y + 1) % 2; i < 2; i += 2)
    {
        add = isPassable(&mazeLattice[(wall.y + arroundWall[i + 1]) * (wall.x + arroundWall[i] + 1)]);
        sub = isPassable(&mazeLattice[(wall.y - arroundWall[i + 1]) * (wall.x - arroundWall[i] + 1)]);
        if (add && !sub)
        {
            // p = (Point){wall.x - arroundWall[i], wall.y - arroundWall[i + 1]};
            p.x = wall.x - arroundWall[i];
            p.y = wall.y - arroundWall[i + 1];
            break;
        }
        if (!add && sub)
        {
            // p = (Point){wall.x + arroundWall[i], wall.y + arroundWall[i + 1]};
            p.x = wall.x + arroundWall[i];
            p.y = wall.y + arroundWall[i + 1];
            break;
        }
    }
    return p;
}

void randomizedPrimCreateMaze_createMaze(Lattice *mazeLattice, int colNumber, int rowNumber)
{

    Point currentPoint;
    Point *listWall;
    Point wall;
    int k;
    int listWallSize;
    listWall = malloc((colNumber * rowNumber) * sizeof(Point));
    currentPoint.x = 2 * (rand() % (colNumber / 2)) + 1;
    currentPoint.y = 2 * (rand() % (rowNumber / 2)) + 1;
    sand(getuptime());
    setPassable(&mazeLattice[currentPoint.y * (colNumber + 1) + currentPoint.x], true);

    listWallSize = 0;
    pushArroundWallToList(mazeLattice, currentPoint, listWall, &listWallSize, colNumber, rowNumber);

    while (listWallSize > 0)
    {
        k = rand() % listWallSize; // Randomly choose a wall
        wall = listWall[k];

        // Remove wall from list
        listWall[k] = listWall[--listWallSize];

        currentPoint = findPoint(wall, mazeLattice);
        if (currentPoint.x != -1)
        {
            setPassable(&mazeLattice[wall.y * (colNumber + 1) + wall.x], true);
            setPassable(&mazeLattice[currentPoint.y * (colNumber + 1) + currentPoint.x], true);
            pushArroundWallToList(mazeLattice, currentPoint, listWall, &listWallSize, colNumber, rowNumber);
        }
    }
    free(listWall);
}

// RecursiveDivisionCreateMaze
void openAdoor(Lattice *mazeLattice, Point p1, Point p2)
{
    if (p1.y == p2.y && p1.x == p2.x)
    {
        setPassable(&mazeLattice[p1.y * (p1.x + 1)], true);
        return;
    }

    if (p1.y == p2.y)
    {
        int pos = p1.x + (rand() % ((p2.x - p1.x) / 2)) * 2;
        setPassable(&mazeLattice[p1.y * (pos + 1)], true);
    }
    else if (p1.x == p2.x)
    {
        int pos = p1.y + (rand() % ((p2.y - p1.y) / 2)) * 2;
        setPassable(&mazeLattice[pos * (p1.x + 1)], true);
    }
}

void recursiveCreateMaze(Lattice *mazeLattice, Point start, int height, int width)
{
    int drawx, drawy;
    int i, opendoor;
    Point tempPoint1, tempPoint2, tempPoint3, tempPoint4;

    if (height <= 2 || width <= 2)
        return;

    drawx = start.y + (rand() % (height / 2)) * 2 + 1;
    for (i = start.x; i < start.x + width; ++i)
        setPassable(&mazeLattice[drawx * (i + 1)], false);

    drawy = start.x + (rand() % (width / 2)) * 2 + 1;
    for (i = start.y; i < start.y + height; ++i)
        setPassable(&mazeLattice[i * (drawy + 1)], false);

    opendoor = rand() % 4 + 1;
    switch (opendoor)
    {
    case 1:
        tempPoint1.y = drawy;
        tempPoint1.x = drawx + 1;
        tempPoint2.y = drawy;
        tempPoint2.x = start.y + height - 1;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = drawy + 1;
        tempPoint1.x = drawx;
        tempPoint2.y = start.x + width - 1;
        tempPoint2.x = drawx;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = drawy;
        tempPoint1.x = start.y;
        tempPoint2.y = drawy;
        tempPoint2.x = drawx - 1;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);
        break;
    case 2:
        tempPoint1.y = drawy + 1;
        tempPoint1.x = drawx;
        tempPoint2.y = start.x + width - 1;
        tempPoint2.x = drawx;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = drawy;
        tempPoint1.x = start.y;
        tempPoint2.y = drawy;
        tempPoint2.x = drawx - 1;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = start.x;
        tempPoint1.x = drawx;
        tempPoint2.y = drawy - 1;
        tempPoint2.x = drawx;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);
        break;
    case 3:
        tempPoint1.y = drawy;
        tempPoint1.x = start.y;
        tempPoint2.y = drawy;
        tempPoint2.x = drawx - 1;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = start.x;
        tempPoint1.x = drawx;
        tempPoint2.y = drawy - 1;
        tempPoint2.x = drawx;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = drawy;
        tempPoint1.x = drawx + 1;
        tempPoint2.y = drawy;
        tempPoint2.x = start.y + height - 1;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);
        break;
    case 4:
        tempPoint1.y = start.x;
        tempPoint1.x = drawx;
        tempPoint2.y = drawy - 1;
        tempPoint2.x = drawx;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = drawy;
        tempPoint1.x = drawx + 1;
        tempPoint2.y = drawy;
        tempPoint2.x = start.y + height - 1;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);

        tempPoint1.y = drawy + 1;
        tempPoint1.x = drawx;
        tempPoint2.y = start.x + width - 1;
        tempPoint2.x = drawx;
        openAdoor(mazeLattice, tempPoint1, tempPoint2);
        break;
    default:
        break;
    }

    recursiveCreateMaze(mazeLattice, start, drawx - start.y, drawy - start.x);
    tempPoint1.y = drawy + 1;
    tempPoint1.x = start.y;
    recursiveCreateMaze(mazeLattice, tempPoint1, drawx - start.y, width - drawy + start.x);
    tempPoint1.y = start.x;
    tempPoint1.x = drawx + 1;
    recursiveCreateMaze(mazeLattice, tempPoint1, height - drawx + start.y - 1, drawy - start.x);
    tempPoint1.y = drawy + 1;
    tempPoint1.x = drawx + 1;
    recursiveCreateMaze(mazeLattice, tempPoint1, height - drawx + start.y - 1, width - drawy + start.x);
}

void recursiveDivisionCreateMaze_createMaze(Lattice *mazeLattice, int colNumber, int rowNumber)
{
    int i, j;
    Point start;
    srand(time(0));
    for (i = 1; i < colNumber + 1; ++i)
        for (j = 1; j < rowNumber + 1; ++j)
            setPassable(&mazeLattice[j * (colNumber + 1) + i], true);

    start.x = 1;
    start.y = 1;
    recursiveCreateMaze(mazeLattice, start, rowNumber, colNumber);
}

int main_()
{
    int colNumber = 10; // Set maze width
    int rowNumber = 10; // Set maze height
    Lattice *mazeLattice = (Lattice *)malloc((colNumber + 1) * (rowNumber + 1) * sizeof(Lattice));

    // Example of creating the maze using different methods
    depthFirstSearchCreateMaze_createMaze(mazeLattice, colNumber, rowNumber);
    // randomizedPrimCreateMaze_createMaze(mazeLattice, colNumber, rowNumber);
    // recursiveDivisionCreateMaze_createMaze(mazeLattice, colNumber, rowNumber);

    // Free allocated memory for the maze
    free(mazeLattice);

    return 0;
}

void drawGame()
{
    // drawMaze(); // 重新绘制迷宫
    // drawPlayer(&player); // 绘制玩家
    cls(200, 200, 200);
    drawMaze(lattice, MAZE_WIDTH+2, MAZE_HEIGHT+2);
    mrc_refreshScreen(0, 0, SCRW, SCRH); // 刷新屏幕
}

int32 drawTime;
int32 logox, logoy;
void logoc(int32 data)
{
    int32 tt;
    int32 i;
    char temp[100];

    rotation += 10;
    alpha += 2;
    if (alpha > 255)
        alpha = 0;
    memset(w_getScreenBuffer(), 0xff, SCRW * SCRH * 2);
    gl_clearScreen(32, 240, 66);
    tt = getuptime();
    for (i = 0; i < 100; i++)
    {
        // mrc_clearScreen(32, 238, 240);
        // gl_clearScreen(32, 255, 255);
        // drawBitmap(logo2, 10, logoy);
        // drawBitmapEx(logo, 10, logoy, 60, 60, 0,0, 30, 30);
        drawBitmap(logo2, 100, logoy);
    }

    // cls(255, 255, 255);
    // mrc_printf("cls\n");

    // mrc_printf("cls1\n");
    // drawBitmap565Rotate(logo, SCRW/2, SCRH/2, 5, logo2->height/2, rotation);
    // mrc_printf("cls2\n");
    // drawBitmapAlpha(logo, SCRW/2-logo->width/2, SCRH/2-logo->height/2, alpha);

    // drawBitmapAlpha(logo2, SCRW/2-logo2->width/2, SCRH/2-logo2->height/2 - 100, alpha);
    // mrc_printf("cl3s\n");
    // ref(0, 0, SCRW, SCRH);
    uc3_drawText("测试", 100, 100, 255, 0, 255, 0);
    // gl_drawTriangle(0, 0, 30, 100, 100, 20, 0x00ff00 | (alpha<<24));
    // gl_drawTriangle(0, 30, 30, 130, 100, 50, 0x00ff00 | (alpha<<24));
    // for(i=0;i<1;i++)
    // drawBitmapAlpha(logo2, logox, logoy, alpha);
    // drawBitmapAlpha(logo, logox-32, logoy, alpha);

    // gl_drawHollowTriangle(0, 0, 30, 100, 100, 20, 0xff000000);
    // gl_drawHollowTriangle(0, 30, 30, 130, 100, 50, 0xff000000);
    // gl_drawRotatedRect(30, 30, 50, 50,24,24, rotation, 0x00f000 | (alpha<<24));
    // gl_drawRotatedHollowRect(30, 30, 50, 50,24,24, rotation, 0x0 | (255<<24));

    // gl_drawCir(logox, logoy, 32, 0xff0000 | (alpha<<24));
    // gl_drawHollowCir(logox, logoy, 32, 0x0 | (255<<24));
    // gl_drawColor(0x0000ff | (alpha<<24));

    logoy++;
    logox++;
    logoy++;
    logox++;
    // mrc_drawRotatedRect(SCRW/2, SCRH/2, 50, 50, 25, 25, rotation, 255,0,0);
    // mrc_printf("cls4\n");

    // gl_drawRect(0, 0, SCRW, SCRH, 0x67ff00 | (alpha<<24));
    // gl_drawCir(SCRW, SCRH, 60, 0x00ff00 | (alpha<<24));
    // gl_drawCir(SCRW/2, SCRH/2, 120, 0x00ff00 | (alpha<<24));
    // for(i=0;i<1;i++){
    //     drawShadeRect(0, 0, SCRW, 24, 0x30a0f0| (alpha<<24), 0xf0d065| (alpha<<24), SHADE_UPDOWN);
    //     drawShadeRect(0, 100, SCRW, 24, 0x30a0f0| (alpha<<24), 0xf0d065| (alpha<<24), SHADE_LEFTRIGHT);
    // }

    // gl_drawTriangle(0,0, 100,20, 10, 100, 0x00ff00 | (alpha<<24));
    // gl_drawTriangle(0,100, 100,120, 10, 200, 0x00ff00 | (alpha<<24));
    drawTime = getuptime() - tt;
    mrc_sprintf(temp, "Time: %d ms", drawTime);

    uc3_drawTextHor(temp, 1, 1, 255, 0, 255, 0);
    ref(0, 0, SCRW, SCRH);
}

int32 mrc_init(void)
{
    int32 re;
    Point pstart;
    Point pend;
    // 初始化随机数种子
    sand(getuptime());
    mpc_init();
    uc3_init();
    lattice = mrc_malloc((MAZE_WIDTH+2) * (MAZE_HEIGHT+2) * sizeof(Lattice));
    mrc_memset(lattice, 0, (MAZE_WIDTH+2) * (MAZE_HEIGHT+2) * sizeof(Lattice));
    //打开始点
    pstart.x = 0;
    pstart.y = 1;
    pend.x = MAZE_WIDTH-1;
    pend.y = MAZE_HEIGHT-2;
    lattice[MAZE_WIDTH*pstart.y + pstart.x].passable = 1;
    lattice[MAZE_WIDTH*pend.y + pend.x].passable = 1;
    depthFirstSearchCreateMaze_createMaze(lattice, MAZE_WIDTH+2, MAZE_HEIGHT+2);
    logox = -22;
    logoy = -40;
    CELL_SIZE = SCRW / MAZE_WIDTH;

    // 生成迷宫
    drawGame();

    logo = readBitmap565FromAssets("0001.bma");
    // re = bitmapAutoMemcpy(logo);

    logo2 = readBitmap565FromAssets("logo.bma");
    drawBitmap565Old(logo2, logo, 10, 0, 30, 30, 10, 10);

    timer = timercreate();

    // logoc(0);

    // timerstart(timer, 50, 0, logoc, 1); // 1s 后开始计时]

    return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    uint8 *buftemp = NULL;

    int new_x, new_y; // 移动变量定义到函数顶部
    buftemp = mrc_malloc(1122);
    mrc_printf("malloc ........... %d\n", *((uint32 *)(buftemp - 4)));
    mrc_freeFileData(buftemp - 4, 1122);
    if (code == KY_DOWN)
    {

        switch (param0)
        {
        case _UP:
            new_y -= 1;
            break;
        case _DOWN:
            new_y += 1;
            break;
        case _LEFT:
            new_x -= 1;
            break;
        case _RIGHT:
            new_x += 1;
            break;
        case _SRIGHT:
            mrc_exit();
            return 0;
        }

        // if (new_x >= 0 && new_x < MAZE_WIDTH && new_y >= 0 && new_y < MAZE_HEIGHT && maze[new_y][new_x] == 1) {

        // }

        // drawGame();
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
    timerstop(timer);
    timerdel(timer);

    uc3_free();
    bitmapFree(logo);
    bitmapFree(logo2);
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