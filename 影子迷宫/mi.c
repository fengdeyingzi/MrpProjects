#include <mrc_base.h>
#include "mpc.h"
#include "mrc_graphics.h"
#include "uc3_font.h"
#include "xl_sound.h"
#define MAZE_WIDTH 40
#define MAZE_HEIGHT 40
int32 CELL_SIZE;

typedef struct {
    int x, y; // 角色的坐标
} Player;
Player player; // 玩家起始位置
int maze[MAZE_HEIGHT][MAZE_WIDTH];
int32 timer;
BITMAP_565 *logo;
BITMAP_565 *logo2;
int rotation;
int alpha;

void switchWindow(int windex, int leve) {}

void initMaze() {
    int x, y; // 移动变量定义到函数顶部
    for (y = 0; y < MAZE_HEIGHT; y++) {
        for (x = 0; x < MAZE_WIDTH; x++) {
            maze[y][x] = 0; // 所有单元格初始化为墙
        }
    }
}

// 检查给定坐标是否在迷宫边界内且是墙
int isValid(int x, int y) {
    return (x > 0 && x < MAZE_WIDTH-1 && y > 0 && y < MAZE_HEIGHT-1 && maze[y][x] == 0);
}

// 随机打通墙，相邻单元格
void generateMaze(int x, int y) {
    int i, j; // 移动变量定义到函数顶部
    int temp[2];
    // 方向数组（右、下、左、上）
    int directions[4][2] = { {2, 0}, {0, 2}, {-2, 0}, {0, -2} };
    
    // 随机打乱方向
    for (i = 0; i < 4; i++) {
        j = rand() % 4;
        // 交换方向
        temp[0] = directions[i][0], 
        temp[1] = directions[i][1];
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp[0];
        directions[j][1] = temp[1];
    }

    // 遍历所有方向
    for (i = 0; i < 4; i++) {
        int nx = x + directions[i][0], ny = y + directions[i][1];
        // 确保新单元格在边界之内且是墙
        if (isValid(nx, ny)) {
            maze[y + directions[i][1] / 2][x + directions[i][0] / 2] = 1; // 打通墙
            maze[ny][nx] = 1; // 打通新位置
            generateMaze(nx, ny); // 递归生成迷宫
        }
    }
}

void drawMaze() {
    int x, y; // 移动变量定义到函数顶部
    for (y = 0; y < MAZE_HEIGHT; y++) {
        for (x = 0; x < MAZE_WIDTH; x++) {
            if (maze[y][x] == 0) {
                // mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 0, 0, 0); // 墙壁
            } else {
                mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 255, 255, 255); // 通路
            }
        }
    }
}

void drawPlayer(Player *player) {
    mrc_drawRect(player->x * CELL_SIZE, player->y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 0, 0, 255); // 玩家
}

void drawGame() {
    mrc_clearScreen(0, 0, 0); // 清屏
        drawMaze(); // 重新绘制迷宫
        drawPlayer(&player); // 绘制玩家
        mrc_refreshScreen(0, 0, SCRW, SCRH); // 刷新屏幕
}

int32 drawTime;
int32 logox,logoy;
void logoc(int32 data) {
    int32 tt;
    int32 i;
    char temp[100];
    mrc_printf("logoc");
    rotation+=10;
    alpha+=2;
    if(alpha>255) alpha=0;
    memset(w_getScreenBuffer(), 0xff, SCRW * SCRH * 2);
    gl_clearScreen(32, 240, 66);
    tt = getuptime();
    for(i=0;i<100;i++){
        // mrc_clearScreen(32, 238, 240);
        // gl_clearScreen(32, 255, 255);
        drawBitmap565(logo, 10, logoy);
    }
    drawTime = getuptime()-tt;
    
    // cls(255, 255, 255);
    // mrc_printf("cls\n");
    
    // mrc_printf("cls1\n");
    // drawBitmap565Rotate(logo, SCRW/2, SCRH/2, 5, logo2->height/2, rotation);
    // mrc_printf("cls2\n");
    // drawBitmapAlpha(logo, SCRW/2-logo->width/2, SCRH/2-logo->height/2, alpha);
    mrc_printf("draw start");
    
    mrc_printf("draw end");
    // drawBitmapAlpha(logo2, SCRW/2-logo2->width/2, SCRH/2-logo2->height/2 - 100, alpha);
    // mrc_printf("cl3s\n");
    // ref(0, 0, SCRW, SCRH);
    uc3_drawText("测试", 100, 100, 255, 0, 255, 0);
    // gl_drawTriangle(0, 0, 30, 100, 100, 20, 0x00ff00 | (alpha<<24));
    // gl_drawTriangle(0, 30, 30, 130, 100, 50, 0x00ff00 | (alpha<<24));
    // for(i=0;i<1;i++)
    drawBitmapAlpha(logo2, logox, logoy, alpha);
    drawBitmapAlpha(logo, logox-32, logoy, alpha);
    mrc_printf("alpha success");
    // gl_drawHollowTriangle(0, 0, 30, 100, 100, 20, 0xff000000);
    // gl_drawHollowTriangle(0, 30, 30, 130, 100, 50, 0xff000000);
    // gl_drawRotatedRect(30, 30, 50, 50,24,24, rotation, 0x00f000 | (alpha<<24));
    // gl_drawRotatedHollowRect(30, 30, 50, 50,24,24, rotation, 0x0 | (255<<24));

    // gl_drawCir(logox, logoy, 32, 0xff0000 | (alpha<<24));
    // gl_drawHollowCir(logox, logoy, 32, 0x0 | (255<<24));
    // gl_drawColor(0x0000ff | (alpha<<24));
    
    
    logoy++;
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
    
    mrc_sprintf(temp, "Time: %d ms", drawTime);
    uc3_drawTextHor(temp, 1, 1, 255, 0, 255, 0);
    ref(0, 0, SCRW, SCRH);
}

int32 sound_test;
int32 mrc_init(void) {
    int32 re;
    // 初始化随机数种子
    sand(getuptime());
    mpc_init();
    uc3_init();
    sound_test = soundLoad("birds_fly.mp3");
    logox = 50;
    logoy = -1;
    player.x = 1;
    player.y = 1; // 起点
    CELL_SIZE = SCRW/MAZE_WIDTH;
    initMaze(); // 初始化迷宫
    maze[1][1] = 1; // 起点
    generateMaze(1, 1); // 生成迷宫
    drawGame();
    mrc_printf("11111111111");
    logo = readBitmap565FromAssets("test1.bma");
    re = bitmapAutoMemcpy(logo);
    mrc_printf("memcpy auto success");
    mrc_printf("11111111111");
    logo2 = readBitmap565FromAssets("logo2.bma");
    mrc_printf("11111111111");
    timer = timercreate();
    mrc_printf("11111111111");
    logoc(0);
    mrc_printf("11111111111");
    timerstart(timer, 1000, 0, logoc, 1); // 1s 后开始计时]
    
    return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1) {
     uint8* buftemp = NULL;
    
    int new_x, new_y; // 移动变量定义到函数顶部
buftemp = mrc_malloc(1122);
    mrc_printf("malloc ........... %d\n", *((uint32*)(buftemp-4)));
    mrc_freeFileData(buftemp-4, 1122);
    if (code == KY_DOWN) {
        new_x = player.x;
        new_y = player.y;

        switch (param0) {
            case _UP: new_y -= 1; break;
            case _DOWN: new_y += 1; break;
            case _LEFT: new_x -= 1; break;
            case _RIGHT: new_x += 1; break;
            case _SRIGHT: mrc_exit(); return 0;
            case _1: soundPlay(sound_test, 0);
        }

        if (new_x >= 0 && new_x < MAZE_WIDTH && new_y >= 0 && new_y < MAZE_HEIGHT && maze[new_y][new_x] == 1) {
            player.x = new_x;
            player.y = new_y;
        }

        // drawGame();
    }
    
    return 0;
}

int32 mrc_pause(void) {
    return 0;
}

int32 mrc_resume(void) {
    return 0;
}

int32 mrc_exitApp() {
    timerstop(timer);
    timerdel(timer);
    soundFree(sound_test);
    uc3_free();
    bitmap565Free(logo);
    bitmap565Free(logo2);
    return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5) {
    return 0;
}