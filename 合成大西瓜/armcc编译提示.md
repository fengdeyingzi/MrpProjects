以下C语言代码我使用armcc编译,它只支持在函数体最前面定义变量,请帮我修复以下变量定义的问题:
```
#include <mrc_base.h>
#include "mpc.h"
#include <time.h>
#include <stdlib.h>

#define MAZE_WIDTH 12
#define MAZE_HEIGHT 12
#define CELL_SIZE 20

typedef struct {
    int x, y; // 角色的坐标
} Player;

int maze[MAZE_HEIGHT][MAZE_WIDTH];

void switchWindow(int windex, int leve) {}

void initMaze() {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            maze[y][x] = 0; // 所有单元格初始化为墙
        }
    }
}

// 检查给定坐标是否在迷宫边界内且是墙
int isValid(int x, int y) {
    return (x > 0 && x < MAZE_WIDTH - 1 && y > 0 && y < MAZE_HEIGHT - 1 && maze[y][x] == 0);
}

// 随机打通墙，相邻单元格
void generateMaze(int x, int y) {
    // 方向数组（右、下、左、上）
    int directions[4][2] = { {2, 0}, {0, 2}, {-2, 0}, {0, -2} };
    
    // 随机打乱方向
    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        // 交换方向
        int temp[2] = { directions[i][0], directions[i][1] };
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp[0];
        directions[j][1] = temp[1];
    }

    // 遍历所有方向
    for (int i = 0; i < 4; i++) {
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
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (maze[y][x] == 0) {
                mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 255, 255, 255); // 墙壁
            } else {
                mrc_drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 0, 0, 0); // 通路
            }
        }
    }
}

void drawPlayer(Player *player) {
    mrc_drawRect(player->x * CELL_SIZE, player->y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 0, 0, 255); // 玩家
}

int32 mrc_init(void) {
    // 初始化随机数种子
    srand(time(NULL));
    mpc_init();
    initMaze(); // 初始化迷宫
    maze[1][1] = 1; // 起点
    generateMaze(1, 1); // 生成迷宫
    return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1) {
    static Player player = { 1, 1 }; // 玩家起始位置

    if (code == KY_DOWN) {
        int new_x = player.x;
        int new_y = player.y;

        switch (param0) {
            case _UP: new_y -= 1; break;
            case _DOWN: new_y += 1; break;
            case _LEFT: new_x -= 1; break;
            case _RIGHT: new_x += 1; break;
        }

        if (new_x >= 0 && new_x < MAZE_WIDTH && new_y >= 0 && new_y < MAZE_HEIGHT && maze[new_y][new_x] == 1) {
            player.x = new_x;
            player.y = new_y;
        }

        mrc_clearScreen(0, 0, 0); // 清屏
        drawMaze(); // 重新绘制迷宫
        drawPlayer(&player); // 绘制玩家
        mrc_refreshScreen(0, 0, SCRW, SCRH); // 刷新屏幕
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
    return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5) {
    return 0;
}
```

