#include <mrc_base.h>
#include "uc3_font.h"
#include "mpc.h"
#include "mrc_graphics.h"
#include "game_menu.h"

/*
合成大金鱼游戏
功能机版本 240x320分辨率
*/

// ==================== 常量定义 ====================
#define MAX_FISH 50        // 最大鱼数量
#define FISH_TYPES_COUNT 9 // 鱼类型数量
#define GAME_WIDTH 240     // 游戏区域宽度
int32 GAME_HEIGHT = 0;    // 游戏区域高度
#define LIMIT_LINE_Y 80    // 警戒线高度（按比例从150缩放）
#define GRAVITY 0.3f       // 重力加速度
#define RESTITUTION 0.3f   // 弹性系数
#define FRICTION 0.98f     // 摩擦系数
#define DROP_DELAY 24      // 投放延迟（24帧约800ms）
#define DANGER_THRESHOLD 100 // 超过警戒线判定帧数


// ==================== 数据结构 ====================
// 鱼类型定义
typedef struct {
    int level;     // 等级
    int radius;    // 半径（按比例缩放）
    int val;       // 分值
    char* imgName; // 图片文件名
} FishType;

// 物理对象（鱼）
typedef struct {
    int active;      // 是否激活
    int level;       // 鱼的等级
    float x, y;      // 位置
    float vx, vy;    // 速度
    int dangerTime;  // 在警戒线上方的时间
    int mergeFlag;   // 合成标记（防止重复合成）
} Fish;

// 游戏状态
typedef struct {
    int score;           // 当前分数
    int bestScore;       // 最高分
    int currentFishType; // 当前要投放的鱼类型
    int nextFishType;    // 下一个鱼类型
    int canDrop;         // 是否可以投放
    int dropperX;        // 投放预览的X位置
    int dropDelay;       // 投放延迟计数
    int isGameOver;      // 游戏是否结束
} GameState;

// ==================== 窗口管理 ====================
// 窗口类型枚举
enum {
    WINDOW_MENU = 0,    // 菜单窗口
    WINDOW_GAME = 1,    // 游戏窗口
    WINDOW_HELP = 2,    // 帮助窗口
    WINDOW_ABOUT = 3    // 关于窗口
};

// 菜单选项枚举
enum {
    MENU_START = 0,     // 开始游戏
    MENU_HELP = 1,      // 游戏帮助
    MENU_ABOUT = 2,     // 关于
    MENU_EXIT = 3       // 退出
};

// ==================== 全局变量 ====================
// 鱼类型配置（从240宽度缩放）
FishType fishTypes[FISH_TYPES_COUNT];

Fish fishes[MAX_FISH];           // 所有鱼对象
GameState gameState;             // 游戏状态
BITMAP_565* fishBitmaps[FISH_TYPES_COUNT]; // 鱼图片资源
BITMAP_565* bgBitmap;            // 背景图片

int32 timer_cd;
uint32 randomSeed;

// 窗口管理
int currentWindow;               // 当前窗口
PMENU mainMenu;                  // 主菜单对象
int gameInited;                  // 游戏是否已初始化

/* 函数声明 */
void drawTextWithStroke(char* text, int16 x, int16 y);
void initGame(void);
void drawGame(void);
void drawHelp(void);
void helpEvent(int32 code, int32 param0, int32 param1);
void drawAbout(void);
void aboutEvent(int32 code, int32 param0, int32 param1);

// ==================== 窗口管理函数 ====================

// 窗口切换函数
void switchWindow(int windex, int level) {
    if (windex == 1) {
        /* 从菜单进入其他窗口 */
        if (level == MENU_START) {
            /* 开始新游戏 */
            currentWindow = WINDOW_GAME;
            initGame();
            gameInited = 1;
            drawGame();
        } else if (level == MENU_HELP) {
            /* 显示帮助 */
            currentWindow = WINDOW_HELP;
            drawHelp();
        } else if (level == MENU_ABOUT) {
            /* 显示关于 */
            currentWindow = WINDOW_ABOUT;
            drawAbout();
        } else if (level == MENU_EXIT) {
            /* 退出游戏 */
            mrc_exit();
        }
    } else if (windex == 0) {
        /* 返回菜单 */
        currentWindow = WINDOW_MENU;
        pmenu_draw(&mainMenu);
    }
}

// 绘制返回按钮（右下角）
void drawBackButton(void) {
    int btnX, btnY;
    int textW;
    char* btnText;

    btnText = "返回";
    textW = uc3_getWidth(btnText, 0);
    btnX = SCRW - textW - 10;
    btnY = SCRH - 25;

    /* 绘制黑色文字 */
    uc3_drawText(btnText, btnX, btnY, 0, 0, 0, 0);
}

// 检测是否点击返回按钮
int isBackButtonClicked(int32 x, int32 y) {
    int btnX, btnY, btnW, btnH;
    int textW;

    textW = uc3_getWidth("返回", 0);
    btnW = textW + 10;
    btnH = 20;
    btnX = SCRW - textW - 10;
    btnY = SCRH - 25;

    if (x >= btnX - 5 && x <= btnX + btnW && y >= btnY && y <= btnY + btnH) {
        return 1;
    }
    return 0;
}

// 帮助窗口绘制
void drawHelp(void) {
    char* helpText1;
    char* helpText2;
    char* helpText3;
    char* helpText4;
    char* helpText5;

    gl_clearScreen(240, 240, 240);

    drawTextWithStroke("游戏帮助", GAME_WIDTH / 2 - 32, 20);

    helpText1 = "点击屏幕投放鱼";
    helpText2 = "相同的鱼会合成";
    helpText3 = "不要超过红线";
    helpText4 = "按方向键左右移动";
    helpText5 = "按右软键返回菜单";

    uc3_drawText(helpText1, 20, 80, 50, 50, 50, 0);
    uc3_drawText(helpText2, 20, 110, 50, 50, 50, 0);
    uc3_drawText(helpText3, 20, 140, 50, 50, 50, 0);
    uc3_drawText(helpText4, 20, 170, 50, 50, 50, 0);
    uc3_drawText(helpText5, 20, 200, 50, 50, 50, 0);

    /* 绘制返回按钮 */
    drawBackButton();

    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

// 帮助窗口事件处理
void helpEvent(int32 code, int32 param0, int32 param1) {
    if (code == KY_UP) {
        if (param0 == _SRIGHT || param0 == _SELECT) {
            switchWindow(0, 0);
        }
    } else if (code == MS_UP) {
        /* 检测是否点击返回按钮 */
        if (isBackButtonClicked(param0, param1)) {
            switchWindow(0, 0);
        }
    }
}

// 关于窗口绘制
void drawAbout(void) {
    char* aboutText1;
    char* aboutText2;
    char* aboutText3;
    char* aboutText4;

    gl_clearScreen(240, 240, 240);

    drawTextWithStroke("关于游戏", GAME_WIDTH / 2 - 32, 20);

    aboutText1 = "合成大金鱼";
    aboutText2 = "版本: 1.0";
    aboutText3 = "作者：风的影子";
    aboutText4 = "开发日期：2025.11.23";

    uc3_drawText(aboutText1, 20, 80, 50, 50, 50, 0);
    uc3_drawText(aboutText2, 20, 110, 50, 50, 50, 0);
    uc3_drawText(aboutText3, 20, 140, 50, 50, 50, 0);
    uc3_drawText(aboutText4, 20, 170, 50, 50, 50, 0);

    /* 绘制返回按钮 */
    drawBackButton();

    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

// 关于窗口事件处理
void aboutEvent(int32 code, int32 param0, int32 param1) {
    if (code == KY_UP) {
        if (param0 == _SRIGHT || param0 == _SELECT) {
            switchWindow(0, 0);
        }
    } else if (code == MS_UP) {
        /* 检测是否点击返回按钮 */
        if (isBackButtonClicked(param0, param1)) {
            switchWindow(0, 0);
        }
    }
}

// 绘制描边文字（黑底白字）
void drawTextWithStroke(char* text, int16 x, int16 y) {
    // 绘制黑色描边（上下左右各偏移1像素）
    uc3_drawText(text, x - 1, y, 0, 0, 0, 0);     // 左
    uc3_drawText(text, x + 1, y, 0, 0, 0, 0);     // 右
    uc3_drawText(text, x, y - 1, 0, 0, 0, 0);     // 上
    uc3_drawText(text, x, y + 1, 0, 0, 0, 0);     // 下
    // 绘制白色文字
    uc3_drawText(text, x, y, 255, 255, 255, 0);
}

// ==================== 工具函数 ====================
// 快速平方根（牛顿迭代法）
float fastSqrt(float x) {
    float xhalf = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1); // 魔数
    x = *(float*)&i;
    x = x * (1.5f - xhalf * x * x); // 一次迭代
    x = x * (1.5f - xhalf * x * x); // 二次迭代提高精度
    return 1.0f / x;
}

// 简单随机数生成器（线性同余）
int getRandom(int max) {
    randomSeed = randomSeed * 1103515245 + 12345;
    return (randomSeed / 65536) % max;
}

// 两点距离
float distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return fastSqrt(dx * dx + dy * dy);
}

// ==================== 游戏逻辑函数 ====================

// 初始化游戏状态
void initGame(void) {
    int i;
    GAME_HEIGHT = SCRH * 300/320;
    // 清空鱼数组
    for (i = 0; i < MAX_FISH; i++) {
        fishes[i].active = 0;
        fishes[i].mergeFlag = 0;
    }

    // 初始化游戏状态
    gameState.score = 0;
    gameState.bestScore = 0; // TODO: 从存储读取
    gameState.isGameOver = 0;
    gameState.canDrop = 1;
    gameState.dropperX = GAME_WIDTH / 2;
    gameState.dropDelay = 0;

    // 设置第一个鱼
    gameState.nextFishType = getRandom(5); // 只生成前5种
    gameState.currentFishType = gameState.nextFishType;
    gameState.nextFishType = getRandom(5);
}

// 加载图片资源
void loadAssets(void) {
    int i;
    BITMAPINFO info;

    // 加载背景图片
    bgBitmap = readBitmap565FromAssets("bg_240.bma");

    // 加载鱼图片并获取尺寸
    for (i = 0; i < FISH_TYPES_COUNT; i++) {
        fishBitmaps[i] = readBitmap565FromAssets(fishTypes[i].imgName);
        bitmap565Rotate(fishBitmaps[i], 1, 0);
        // 获取图片宽度并设置半径
        if (fishBitmaps[i] != NULL) {
            if (bitmap565getInfo(fishBitmaps[i], &info) == MR_SUCCESS) {
                fishTypes[i].radius = info.width / 2;
            }
        }
    }
}

// 添加分数
void addScore(int val) {
    gameState.score += val;
    if (gameState.score > gameState.bestScore) {
        gameState.bestScore = gameState.score;
    }
}

// 创建新鱼
int createFish(float x, float y, int level) {
    int i;
    for (i = 0; i < MAX_FISH; i++) {
        if (!fishes[i].active) {
            fishes[i].active = 1;
            fishes[i].level = level;
            fishes[i].x = x;
            fishes[i].y = y;
            fishes[i].vx = 0;
            fishes[i].vy = 0;
            fishes[i].dangerTime = 0;
            fishes[i].mergeFlag = 0;
            return i;
        }
    }
    return -1;
}

// 删除鱼
void removeFish(int index) {
    if (index >= 0 && index < MAX_FISH) {
        fishes[index].active = 0;
    }
}

// 投放鱼
void dropFish(void) {
    if (!gameState.canDrop || gameState.isGameOver) return;

    createFish((float)gameState.dropperX, 50.0f, gameState.currentFishType);

    gameState.canDrop = 0;
    gameState.dropDelay = DROP_DELAY;
}

// 更新投放器
void updateDropper(void) {
    if (gameState.dropDelay > 0) {
        gameState.dropDelay--;
        if (gameState.dropDelay == 0) {
            // 生成下一个
            gameState.currentFishType = gameState.nextFishType;
            gameState.nextFishType = getRandom(5);
            gameState.canDrop = 1;
        }
    }
}

// 物理更新
void updatePhysics(void) {
    int i;
    FishType* type;

    for (i = 0; i < MAX_FISH; i++) {
        if (!fishes[i].active) continue;

        type = &fishTypes[fishes[i].level];

        // 应用重力
        fishes[i].vy += GRAVITY;

        // 更新位置
        fishes[i].x += fishes[i].vx;
        fishes[i].y += fishes[i].vy;

        // 应用摩擦力
        fishes[i].vx *= FRICTION;
        fishes[i].vy *= FRICTION;

        // 边界碰撞检测 - 左右墙
        if (fishes[i].x - type->radius < 0) {
            fishes[i].x = (float)type->radius;
            fishes[i].vx = -fishes[i].vx * RESTITUTION;
        }
        if (fishes[i].x + type->radius > GAME_WIDTH) {
            fishes[i].x = (float)(GAME_WIDTH - type->radius);
            fishes[i].vx = -fishes[i].vx * RESTITUTION;
        }

        // 边界碰撞检测 - 地面
        if (fishes[i].y + type->radius > GAME_HEIGHT) {
            fishes[i].y = (float)(GAME_HEIGHT - type->radius);
            fishes[i].vy = -fishes[i].vy * RESTITUTION;
            // 地面摩擦力
            fishes[i].vx *= 0.9f;
        }
    }
}

// 碰撞检测和合成
void checkCollisions(void) {
    int i, j;
    FishType *typeA, *typeB, *newType;
    float dist, minDist;
    float midX, midY;
    float dx, dy;
    float overlap;
    float nx, ny;

    // 重置合成标记
    for (i = 0; i < MAX_FISH; i++) {
        if (fishes[i].active) {
            fishes[i].mergeFlag = 0;
        }
    }

    for (i = 0; i < MAX_FISH; i++) {
        if (!fishes[i].active || fishes[i].mergeFlag) continue;

        typeA = &fishTypes[fishes[i].level];

        for (j = i + 1; j < MAX_FISH; j++) {
            if (!fishes[j].active || fishes[j].mergeFlag) continue;

            typeB = &fishTypes[fishes[j].level];

            // 计算距离
            dist = distance(fishes[i].x, fishes[i].y, fishes[j].x, fishes[j].y);
            minDist = (float)(typeA->radius + typeB->radius);

            // 检测碰撞
            if (dist < minDist) {
                // 相同等级合成
                if (fishes[i].level == fishes[j].level && fishes[i].level < FISH_TYPES_COUNT - 1) {
                    // 计算中点
                    midX = (fishes[i].x + fishes[j].x) / 2.0f;
                    midY = (fishes[i].y + fishes[j].y) / 2.0f;

                    // 标记删除
                    fishes[i].mergeFlag = 1;
                    fishes[j].mergeFlag = 1;

                    // 创建新鱼
                    createFish(midX, midY, fishes[i].level + 1);

                    // 加分
                    addScore(typeA->val * 2);
                } else {
                    // 普通碰撞 - 弹开
                    dx = fishes[j].x - fishes[i].x;
                    dy = fishes[j].y - fishes[i].y;

                    if (dist > 0.1f) {
                        nx = dx / dist;
                        ny = dy / dist;
                        overlap = minDist - dist;

                        // 分离
                        fishes[i].x -= nx * overlap * 0.5f;
                        fishes[i].y -= ny * overlap * 0.5f;
                        fishes[j].x += nx * overlap * 0.5f;
                        fishes[j].y += ny * overlap * 0.5f;

                        // 反弹
                        fishes[i].vx -= nx * 0.5f;
                        fishes[i].vy -= ny * 0.5f;
                        fishes[j].vx += nx * 0.5f;
                        fishes[j].vy += ny * 0.5f;
                    }
                }
            }
        }
    }

    // 删除被标记的鱼
    for (i = 0; i < MAX_FISH; i++) {
        if (fishes[i].active && fishes[i].mergeFlag) {
            removeFish(i);
        }
    }
}

// 检测游戏结束
void checkGameOver(void) {
    int i;
    FishType* type;
    float speed;
    float vxvy;

    if (gameState.isGameOver) return;

    for (i = 0; i < MAX_FISH; i++) {
        if (!fishes[i].active) continue;
        type = &fishTypes[fishes[i].level];
 vxvy = fishes[i].vx * fishes[i].vx + fishes[i].vy * fishes[i].vy;
/*
当鱼完全静止时 (vx = 0, vy = 0)，传入 fastSqrt(0)：
    - 算法计算的是 1/√x，然后返回 1.0f / (1/√x)
    - 当 x = 0 时，1/√0 → ∞，最后 1.0f / ∞ → 0，但中间过程数值不稳定
  2. x 接近 0 时精度问题：当速度非常小时，魔数近似在极小值附近精度很差，可能产生 NaN  或 Inf
*/
speed = (vxvy < 0.0001f) ? 0.0f : fastSqrt(vxvy);
        // speed = fastSqrt(fishes[i].vx * fishes[i].vx + fishes[i].vy * fishes[i].vy);

        // 检查是否在警戒线以上且速度很慢（静止）
        if (fishes[i].y - type->radius < LIMIT_LINE_Y && speed < 0.5f) {
            fishes[i].dangerTime++;
            if (fishes[i].dangerTime > DANGER_THRESHOLD) {
                gameState.isGameOver = 1;
                return;
            }
        } else {
            fishes[i].dangerTime = 0;
        }
    }
}

// 游戏主循环（定时器回调）
void timer_run(int32 id) {
    /* 只在游戏窗口时执行游戏逻辑 */
    if (currentWindow == WINDOW_GAME) {
        if (!gameState.isGameOver) {
            updateDropper();
            updatePhysics();
            checkCollisions();
            checkGameOver();
        }

        /* 渲染（游戏结束时也需要渲染结束画面） */
        drawGame();
    }
}

// 绘制游戏画面
void drawGame(void) {
    int i;
    FishType* type;
    char scoreText[32];
    char bestText[32];

    // 绘制背景图片
    if (bgBitmap != NULL) {
        drawBitmap(bgBitmap, 0, 0);
    } else {
        // 降级方案：纯色背景
        gl_clearScreen(224, 247, 250);
    }

    // 绘制警戒线
    gl_drawLine(0, LIMIT_LINE_Y, GAME_WIDTH, LIMIT_LINE_Y, 0xFFFF6464);

    // 绘制所有鱼
    for (i = 0; i < MAX_FISH; i++) {
        if (fishes[i].active) {
            type = &fishTypes[fishes[i].level];
            if (fishBitmaps[fishes[i].level] != NULL) {
                // 绘制鱼图片（居中）
                drawBitmap(fishBitmaps[fishes[i].level],
                          (int32)(fishes[i].x - type->radius),
                          (int32)(fishes[i].y - type->radius));
            } else {
                // 降级方案：绘制圆形
                gl_drawCir((int32)fishes[i].x, (int32)fishes[i].y, type->radius, 0xFF90CAF9);
            }
        }
    }

    // 绘制投放预览
    if (gameState.canDrop && !gameState.isGameOver) {
        type = &fishTypes[gameState.currentFishType];
        if (fishBitmaps[gameState.currentFishType] != NULL) {
            // 半透明效果通过绘制来模拟（功能机限制）
            drawBitmap(fishBitmaps[gameState.currentFishType],
                      gameState.dropperX - type->radius,
                      50 - type->radius);
        } else {
            // 降级方案：绘制半透明圆形
            gl_drawHollowCir(gameState.dropperX, 50, type->radius, 0xFFFFFF);
        }
        // 辅助线（白色虚线效果）
        gl_drawLine(gameState.dropperX, 50 + type->radius,
                   gameState.dropperX, GAME_HEIGHT, 0xCCCCCC);
    }

    // 绘制UI
    mrc_sprintf(scoreText, "Score: %d", gameState.score);
    drawTextWithStroke(scoreText, 10, 10);

    mrc_sprintf(bestText, "Best: %d", gameState.bestScore);
    drawTextWithStroke(bestText, 10, 30);

    // 绘制下一个预览
    drawTextWithStroke("Next:", GAME_WIDTH - 60, 10);
    if (fishBitmaps[gameState.nextFishType] != NULL) {
        type = &fishTypes[gameState.nextFishType];
        drawBitmap(fishBitmaps[gameState.nextFishType],
                  GAME_WIDTH - 40, 30);
    }

    // 游戏结束提示
    if (gameState.isGameOver) {
        gl_drawRect(0, GAME_HEIGHT/2 - 40, GAME_WIDTH, 80, 0x80000000);
        drawTextWithStroke("Game Over!", GAME_WIDTH/2 - 40, GAME_HEIGHT/2 - 20);
        mrc_sprintf(scoreText, "Score: %d", gameState.score);
        drawTextWithStroke(scoreText, GAME_WIDTH/2 - 35, GAME_HEIGHT/2);
        drawTextWithStroke("Click to restart", GAME_WIDTH/2 - 60, GAME_HEIGHT/2 + 20);
    }

    /* 绘制返回按钮 */
    drawBackButton();

    // 刷新屏幕
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

// ==================== 系统回调函数 ====================

int32 mrc_init(void) {
    mrc_printf("[GAME]init...");
    /* 初始化随机数种子 */
    randomSeed = mrc_getUptime();

    /* 初始化窗口状态 */
    currentWindow = WINDOW_MENU;
    gameInited = 0;

    /* 初始化鱼类型配置 */
    fishTypes[0].level = 0; fishTypes[0].radius = 11; fishTypes[0].val = 1;  fishTypes[0].imgName = "fish_01_240.bma";
    fishTypes[1].level = 1; fishTypes[1].radius = 16; fishTypes[1].val = 3;  fishTypes[1].imgName = "fish_02_240.bma";
    fishTypes[2].level = 2; fishTypes[2].radius = 22; fishTypes[2].val = 6;  fishTypes[2].imgName = "fish_03_240.bma";
    fishTypes[3].level = 3; fishTypes[3].radius = 30; fishTypes[3].val = 10; fishTypes[3].imgName = "fish_04_240.bma";
    fishTypes[4].level = 4; fishTypes[4].radius = 38; fishTypes[4].val = 15; fishTypes[4].imgName = "fish_05_240.bma";
    fishTypes[5].level = 5; fishTypes[5].radius = 48; fishTypes[5].val = 21; fishTypes[5].imgName = "fish_06_240.bma";
    fishTypes[6].level = 6; fishTypes[6].radius = 59; fishTypes[6].val = 28; fishTypes[6].imgName = "fish_07_240.bma";
    fishTypes[7].level = 7; fishTypes[7].radius = 70; fishTypes[7].val = 36; fishTypes[7].imgName = "fish_08_240.bma";
    fishTypes[8].level = 8; fishTypes[8].radius = 83; fishTypes[8].val = 45; fishTypes[8].imgName = "fish_09_240.bma";

    mrc_printf("[GAME]mpc_init...");
    /* 初始化MPC */
    mpc_init();

    mrc_printf("[GAME]uc3_init...");
    /* 初始化字体 */
    uc3_init();

    mrc_printf("[GAME]loadAssets...");
    /* 加载资源 */
    loadAssets();

    mrc_printf("[GAME]init menu...");
    /* 初始化菜单 */
    pmenu_init(&mainMenu);
    pmenu_draw(&mainMenu);

    mrc_printf("[GAME]create timer");
    /* 创建定时器 */
    timer_cd = mrc_timerCreate();
    mrc_timerStart(timer_cd, 33, 0, timer_run, 1); /* 33ms = 30fps */

    mrc_printf("[GAME]done.\n");
    return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1) {
    FishType* type;

    /* 根据当前窗口分发事件 */
    if (currentWindow == WINDOW_MENU) {
        /* 菜单窗口事件处理 */
        pmenu_event(&mainMenu, code, param0, param1);
    } else if (currentWindow == WINDOW_HELP) {
        /* 帮助窗口事件处理 */
        helpEvent(code, param0, param1);
    } else if (currentWindow == WINDOW_ABOUT) {
        /* 关于窗口事件处理 */
        aboutEvent(code, param0, param1);
    } else if (currentWindow == WINDOW_GAME) {
        /* 游戏窗口事件处理 */
        if (code == KY_UP) {
            /* 按键松开 */
            if (param0 == _SRIGHT) {
                /* 右软键返回菜单 */
                switchWindow(0, 0);
            } else if (param0 == _SELECT) {
                /* 确认键投放或重启 */
                if (gameState.isGameOver) {
                    mrc_printf("Restarting game...\n");
                    initGame();
                } else {
                    mrc_printf("Dropping fish...\n");
                    dropFish();
                }
            } else if (param0 == _LEFT) {
                /* 向左移动投放位置 */
                if (gameState.canDrop) {
                    type = &fishTypes[gameState.currentFishType];
                    gameState.dropperX -= 10;
                    if (gameState.dropperX < type->radius) {
                        gameState.dropperX = type->radius;
                    }
                }
            } else if (param0 == _RIGHT) {
                /* 向右移动投放位置 */
                if (gameState.canDrop) {
                    type = &fishTypes[gameState.currentFishType];
                    gameState.dropperX += 10;
                    if (gameState.dropperX > GAME_WIDTH - type->radius) {
                        gameState.dropperX = GAME_WIDTH - type->radius;
                    }
                }
            }
        } else if (code == MS_UP) {
            /* 触摸松开 */
            /* 检测是否点击返回按钮 */
            if (isBackButtonClicked(param0, param1)) {
                switchWindow(0, 0);
            } else if (gameState.isGameOver) {
                /* 重启游戏 */
                initGame();
            } else {
                /* 设置投放位置并投放 */
                type = &fishTypes[gameState.currentFishType];
                gameState.dropperX = param0;

                /* 限制范围 */
                if (gameState.dropperX < type->radius) {
                    gameState.dropperX = type->radius;
                }
                if (gameState.dropperX > GAME_WIDTH - type->radius) {
                    gameState.dropperX = GAME_WIDTH - type->radius;
                }

                dropFish();
            }
        }
    }

    return 0;
}

int32 mrc_pause() {
    return 0;
}

int32 mrc_resume() {
    return 0;
}

int32 mrc_exitApp() {
    int i;

    /* 停止定时器 */
    mrc_timerStop(timer_cd);
    mrc_timerDelete(timer_cd);

    /* 释放菜单资源 */
    pmenu_free(&mainMenu);

    /* 释放背景图片 */
    if (bgBitmap != NULL) {
        bitmapFree(bgBitmap);
    }

    /* 释放鱼图片资源 */
    for (i = 0; i < FISH_TYPES_COUNT; i++) {
        if (fishBitmaps[i] != NULL) {
            bitmapFree(fishBitmaps[i]);
        }
    }

    /* 释放字体 */
    uc3_free();

    return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3,
                            int32 p4, int32 p5) {
    return 0;
}
