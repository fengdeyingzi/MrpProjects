#include "mrc_base.h"
#include "mpc.h"
#include "mrc_graphics.h"
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 10

// 游戏状态
typedef struct {
    int32 paddle_x;
    int32 paddle_y;
    int32 paddle_isleft;
    int32 paddle_isright;
    int32 ball_x;
    int32 ball_y;
    int32 ball_dx; // 球的x方向速度
    int32 ball_dy; // 球的y方向速度
    int32 bricks[BRICK_ROWS][BRICK_COLS]; // 砖块状态
    int32 score; // 得分
    int32 is_running; // 游戏是否运行中
} GameState;

GameState state;

// 函数声明
void mrc_initGame(void);
void mrc_drawBricks(void);
void mrc_drawPaddle(void);
void mrc_drawBall(void);
void mrc_updateBall(void);
void mrc_checkCollision(int32 param0);
void mrc_gameLoop(int32 data);
int32 timer;
// 游戏初始化
int32 mrc_init(void) {
    mpc_init();

    mrc_clearScreen(0, 0, 0); // 清屏为黑色
    mrc_initGame();

    // 启动定时器
     timer = mrc_timerCreate();
    mrc_timerStart(timer, 30, 0, mrc_gameLoop, 1); // 每30毫秒更新一次

    return 0;
}

// 初始化游戏状态
void mrc_initGame(void) {
    int32 row, col;
    state.paddle_x = (SCRW - PADDLE_WIDTH) / 2;
    state.paddle_y = SCRH - PADDLE_HEIGHT - 10;
    state.ball_x = SCRW / 2;
    state.ball_y = SCRH - PADDLE_HEIGHT - 20;
    state.ball_dx = 1; // 向右移动
    state.ball_dy = -1; // 向上移动

    // 初始化砖块
    for ( row = 0; row < BRICK_ROWS; row++) {
        for ( col = 0; col < BRICK_COLS; col++) {
            state.bricks[row][col] = 1; // 1表示存在砖块
        }
    }
    state.score = 0;
    state.is_running = 1;
}

// 绘制砖块
void mrc_drawBricks(void) {
    int32 row, col;
    for ( row = 0; row < BRICK_ROWS; row++) {
        for ( col = 0; col < BRICK_COLS; col++) {
            if (state.bricks[row][col]) {
                mrc_drawRect(col * (SCRW / BRICK_COLS),
                              row * 15,
                              SCRW / BRICK_COLS - 1,
                              10,
                              255, 0, 0); // 红色砖块
            }
        }
    }
}

// 绘制挡板
void mrc_drawPaddle(void) {
    mrc_drawRect(state.paddle_x, state.paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, 0, 255, 0); // 绿色挡板
}

// 绘制球
void mrc_drawBall(void) {
    gl_drawCir(state.ball_x, state.ball_y, 4, 0xffffffff); // 白色球
}

// 更新球的位置
void mrc_updateBall(void) {
    state.ball_x += state.ball_dx;
    state.ball_y += state.ball_dy;

    // 边界碰撞处理
    if (state.ball_x <= 0 || state.ball_x >= SCRW - 1) {
        state.ball_dx = -state.ball_dx; // 水平反弹
    }
    if (state.ball_y <= 0) {
        state.ball_dy = -state.ball_dy; // 垂直反弹
    }

    // 球与挡板碰撞
    if (state.ball_y >= state.paddle_y && 
        state.ball_x >= state.paddle_x && 
        state.ball_x <= state.paddle_x + PADDLE_WIDTH) {
        state.ball_dy = -state.ball_dy; // 反弹
        state.ball_y = state.paddle_y - 1; // 确保球不穿透挡板
    }

    // 球与砖块碰撞
    mrc_checkCollision(0);

    // 如果球掉落到屏幕底部，游戏结束
    if (state.ball_y >= SCRH) {
        state.is_running = 0; // 游戏结束
    }
}

// 检查球与砖块的碰撞
void mrc_checkCollision(int32 param0) {
    int32 row, col, brick_x, brick_y;
    for ( row = 0; row < BRICK_ROWS; row++) {
        for ( col = 0; col < BRICK_COLS; col++) {
            if (state.bricks[row][col]) {
                 brick_x = col * (SCRW / BRICK_COLS);
                 brick_y = row * 15;

                if (state.ball_x >= brick_x && state.ball_x < brick_x + (SCRW / BRICK_COLS) &&
                    state.ball_y >= brick_y && state.ball_y < brick_y + 10) {
                    state.bricks[row][col] = 0; // 移除砖块
                    state.ball_dy = -state.ball_dy; // 反弹
                    state.score++; // 增加分数
                }
            }
        }
    }
}

// 游戏主循环
void mrc_gameLoop(int32 data) {
    if (!state.is_running) {
        mrc_clearScreen(0, 0, 0); // 清屏
        return;
    }

    mrc_clearScreen(0, 0, 0); // 清屏
    mrc_updateBall(); // 更新球位置
    mrc_updateBall(); // 更新球位置
    if (state.paddle_isleft && state.paddle_x > 0) {
            state.paddle_x -= 5; // 向左移动挡板
            if(state.paddle_x < 0) state.paddle_x = 0;
        } else if (state.paddle_isright && state.paddle_x < SCRW - PADDLE_WIDTH) {
            state.paddle_x += 5; // 向右移动挡板
            if(state.paddle_x >= SCRW-PADDLE_WIDTH) state.paddle_x = SCRW-PADDLE_WIDTH;
        }
    mrc_drawBricks(); // 绘制砖块
    mrc_drawPaddle(); // 绘制挡板
    mrc_drawBall(); // 绘制球

    mrc_refreshScreen(0, 0, SCRW, SCRH); // 刷新屏幕
}

// 退出事件函数
int32 mrc_exitApp() {
    // 这里可以释放内存等，当前无动态分配
    return 0;
}

// 事件处理
int32 mrc_event(int32 code, int32 param0, int32 param1) {
    if (code == KY_DOWN) {
        if(param0 == _LEFT){
            state.paddle_isleft = 1;
            state.paddle_isright = 0;
        }
        else if(param0 == _RIGHT){{
            state.paddle_isleft = 0;
            state.paddle_isright = 1;
        }
        
    }
    }
    else if(code == KY_UP){
        if(param0 == _LEFT || param0 == _RIGHT){
            state.paddle_isleft = 0;
            state.paddle_isright = 0;
        }
    }
    
    return 0;
}

void switchWindow(int windex, int leve) {}


int32 mrc_pause() {
    return 0;
}

int32 mrc_resume() {
    return 0;
}




int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5) {
    return 0;
}