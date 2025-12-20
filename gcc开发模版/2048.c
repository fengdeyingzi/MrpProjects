#include <mrc_base.h>
#include <mrc_graphics.h>
#include "uc3_font.h"
#include "mpc.h"
#include "game_menu.h"

/* -------------------------------------------------------------------------- */
/*                                  宏定义                                    */
/* -------------------------------------------------------------------------- */

#define GRID_SIZE 4
#define TILE_GAP 6

/* 颜色定义 (ARGB) */
#define COLOR_BG        0xFFFAF8EF  /* 整体背景 */
#define COLOR_GRID_BG   0xFFBBADA0  /* 棋盘背景 */
#define COLOR_TILE_EMPTY 0xFFCDC1B4 /* 空格子 */
#define COLOR_TEXT_DARK 0xFF776E65  /* 深色文字 (2, 4) */
#define COLOR_TEXT_LIGHT 0xFFF9F6F2 /* 浅色文字 (8+) */

/* 方块颜色 */
#define COLOR_TILE_2    0xFFEEE4DA
#define COLOR_TILE_4    0xFFEDE0C8
#define COLOR_TILE_8    0xFFF2B179
#define COLOR_TILE_16   0xFFF59563
#define COLOR_TILE_32   0xFFF67C5F
#define COLOR_TILE_64   0xFFF65E3B
#define COLOR_TILE_128  0xFFEDCF72
#define COLOR_TILE_256  0xFFEDCC61
#define COLOR_TILE_512  0xFFEDC850
#define COLOR_TILE_1024 0xFFEDC53F
#define COLOR_TILE_2048 0xFFEDC22E

/* 游戏状态 */
#define STATE_MENU      0
#define STATE_PLAYING   1
#define STATE_GAMEOVER  2
#define STATE_WIN       3
#define STATE_ANIMATING 4
#define STATE_HELP      5
#define STATE_ABOUT     6

/* 动画参数 */
#define ANIM_SPEED      20  /* 像素/帧 */
#define POPup_SPEED     2   /* 弹出动画速度 */

typedef struct {
    int id;
    int val;
    int r, c;           /* 逻辑坐标 (目标) */
    int pixel_x, pixel_y; /* 当前像素坐标 */
    int dest_x, dest_y;   /* 目标像素坐标 */
    int to_delete;      /* 动画结束后删除 */
    int scale;          /* 缩放 (100 = 1.0) */
} VisualTile;

/* -------------------------------------------------------------------------- */
/*                                 全局变量                                   */
/* -------------------------------------------------------------------------- */

int grid[GRID_SIZE][GRID_SIZE];
int id_grid[GRID_SIZE][GRID_SIZE]; /* 存储Tile ID */
VisualTile tiles[32]; /* 动画方块池 */
int tile_count;
int next_tile_id = 1;
PMENU main_menu;
int score;
int game_state;
int32 timer_id;
int tile_size;
int grid_offset_x;
int grid_offset_y;
uint32 frame_counter = 0;
int seeded = 0;

/* 触摸相关 */
int touch_start_x;
int touch_start_y;
int is_touching;

/* -------------------------------------------------------------------------- */
/*                                外部函数声明                                */
/* -------------------------------------------------------------------------- */

extern void mrc_sand(uint32 seed);
extern uint32 mrc_rand(void);

/* 补充缺失的系统回调，防止链接错误 */
int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 app, int32 code, int32 param0, int32 param1, int32 param2, int32 param3) {
    return 0;
}

/* -------------------------------------------------------------------------- */
/*                                函数声明                                    */
/* -------------------------------------------------------------------------- */

void initGame(void);
void resetGame(void);
void addRandomTile(void);
int canMove(void);
void drawGame(void);
void drawTileAt(int x, int y, int size, int val);
void drawScore(void);
void drawMenu(void);
void drawGameOver(void);
void drawSoftKeys(char* left, char* right);
void updateLogic(void);
void handleInput(int key);
void handleTouch(int type, int x, int y);
void timer_cb(int32 data);
int getTileColor(int val);
int getTextColor(int val);
void moveLeft(void);
void moveRight(void);
void moveUp(void);
void moveDown(void);
void rotateGrid(void);
void prepareAnimation(void);
void updateAnimation(void);
int getPixelX(int c);
int getPixelY(int r);
void switchWindow(int windex, int leve);
void drawHelp(void);
void drawAbout(void);

/* 动画更新逻辑 */
void updateAnimation(void) {
    int i, j;
    int animating_count = 0;
    int diff_x, diff_y;
    int speed = ANIM_SPEED;

    for(i=0; i<tile_count; i++) {
        /* 位置动画 */
        if(tiles[i].pixel_x != tiles[i].dest_x) {
            diff_x = tiles[i].dest_x - tiles[i].pixel_x;
            if(diff_x > 0) {
                tiles[i].pixel_x += speed;
                if(tiles[i].pixel_x > tiles[i].dest_x) tiles[i].pixel_x = tiles[i].dest_x;
            } else {
                tiles[i].pixel_x -= speed;
                if(tiles[i].pixel_x < tiles[i].dest_x) tiles[i].pixel_x = tiles[i].dest_x;
            }
            animating_count++;
        }
        
        if(tiles[i].pixel_y != tiles[i].dest_y) {
            diff_y = tiles[i].dest_y - tiles[i].pixel_y;
            if(diff_y > 0) {
                tiles[i].pixel_y += speed;
                if(tiles[i].pixel_y > tiles[i].dest_y) tiles[i].pixel_y = tiles[i].dest_y;
            } else {
                tiles[i].pixel_y -= speed;
                if(tiles[i].pixel_y < tiles[i].dest_y) tiles[i].pixel_y = tiles[i].dest_y;
            }
            animating_count++;
        }

        /* 缩放动画 (简单的弹跳恢复) */
        if(tiles[i].scale > 100) {
            tiles[i].scale -= 5;
            if(tiles[i].scale < 100) tiles[i].scale = 100;
            animating_count++;
        }
    }

    if(animating_count == 0) {
        /* 动画结束 */
        game_state = STATE_PLAYING;
        
        /* 1. 更新 grid 数据 */
        for(i=0; i<GRID_SIZE; i++) {
            for(j=0; j<GRID_SIZE; j++) {
                grid[i][j] = 0; /* 先清空 */
            }
        }
        
        /* 2. 将非删除的 tile 填回 grid */
        for(i=0; i<tile_count; i++) {
            if(!tiles[i].to_delete) {
                grid[tiles[i].r][tiles[i].c] = tiles[i].val;
            }
        }
        
        /* 3. 生成新方块 */
        addRandomTile();
        
        /* 4. 检查游戏结束 */
        if (!canMove()) {
            game_state = STATE_GAMEOVER;
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                                系统回调                                    */
/* -------------------------------------------------------------------------- */

int32 mrc_init(void) {
    int min_dim;
    
    mrc_printf("2048: mrc_init start\n");
    uc3_init();
    mpc_init();
    
    /* 计算布局参数 */
    min_dim = SCRW < SCRH ? SCRW : SCRH;
    tile_size = (min_dim - 20 - (GRID_SIZE + 1) * TILE_GAP) / GRID_SIZE;
    
    grid_offset_x = (SCRW - (GRID_SIZE * tile_size + (GRID_SIZE + 1) * TILE_GAP)) / 2;
    grid_offset_y = (SCRH - (GRID_SIZE * tile_size + (GRID_SIZE + 1) * TILE_GAP)) / 2;
    
    /* 如果竖屏，稍微往下移一点留出分数区域 */
    if (SCRH > SCRW) {
        grid_offset_y += 20;
    }

    game_state = STATE_MENU;
    score = 0;
    is_touching = 0;

    /* 初始化菜单 */
    pmenu_init(&main_menu);

    /* 启动定时器 30FPS */
    timer_id = mrc_timerCreate();
    mrc_timerStart(timer_id, 33, 0, timer_cb, 1);
    drawGame();
    return MR_SUCCESS;
}

int32 mrc_exitApp(void) {
    pmenu_free(&main_menu);
    mrc_timerStop(timer_id);
    mrc_timerDelete(timer_id);
    uc3_free();
    return MR_SUCCESS;
}

int32 mrc_event(int32 code, int32 p0, int32 p1) {
    /* mrc_printf("2048: Event code=%d p0=%d\n", code, p0); */
    if (code == KY_DOWN) {
        handleInput(p0);
    } else if (code == MS_DOWN || code == MS_UP || code == MS_MOVE) {
        handleTouch(code, p0, p1);
    }
    return MR_SUCCESS;
}

int32 mrc_pause(void) {
    return MR_SUCCESS;
}

int32 mrc_resume(void) {
    return MR_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*                                游戏逻辑                                    */
/* -------------------------------------------------------------------------- */

void timer_cb(int32 data) {
    frame_counter++;
    // mrc_printf("game_state = %d", game_state);
    if (game_state == STATE_ANIMATING) {
        updateAnimation();
    }
    /* 菜单状态下不自动刷新，节省资源，由事件驱动刷新 */
    if (game_state != STATE_MENU) {
        drawGame();
    }
}

void initGame(void) {
    resetGame();
}

void resetGame(void) {
    int i, j;
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }
    score = 0;
    mrc_printf("resetGame .....");
    addRandomTile();
    mrc_printf("addRandomTile .....");
    addRandomTile();
    game_state = STATE_PLAYING;
}

void addRandomTile(void) {
    int empty_cells[16][2];
    int count = 0;
    int i, j;
    int idx;
    int val;

    /* 查找所有空位 */
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                empty_cells[count][0] = i;
                empty_cells[count][1] = j;
                count++;
            }
        }
    }

    if (count > 0) {
        /* 使用系统随机函数 */
        uint32 r = mrc_rand();
        idx = r % count;
        val = ((mrc_rand() % 10) < 9) ? 2 : 4; /* 90% 概率生成 2 */
        grid[empty_cells[idx][0]][empty_cells[idx][1]] = val;
    }
}

/* 检查是否还能移动 */
int canMove(void) {
    int i, j;
    
    /* 检查是否有空位 */
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) return 1;
        }
    }

    /* 检查是否可以合并 */
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            /* 检查右侧 */
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j+1]) return 1;
            /* 检查下方 */
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i+1][j]) return 1;
        }
    }

    return 0;
}


/* 获取逻辑坐标对应的像素坐标 */
int getPixelX(int c) {
    return grid_offset_x + TILE_GAP + c * (tile_size + TILE_GAP);
}

int getPixelY(int r) {
    return grid_offset_y + TILE_GAP + r * (tile_size + TILE_GAP);
}

/* 查找 tile 索引 */
int findTileIndex(int id) {
    int i;
    for(i=0; i<tile_count; i++) {
        if(tiles[i].id == id) return i;
    }
    return -1;
}

/* 准备动画：将 grid 转换为 tiles 并初始化 id_grid */
void prepareAnimation(void) {
    int i, j;
    tile_count = 0;
    
    for(i=0; i<GRID_SIZE; i++) {
        for(j=0; j<GRID_SIZE; j++) {
            if(grid[i][j] != 0) {
                int id = next_tile_id++;
                id_grid[i][j] = id;
                
                tiles[tile_count].id = id;
                tiles[tile_count].val = grid[i][j];
                tiles[tile_count].r = i;
                tiles[tile_count].c = j;
                tiles[tile_count].pixel_x = getPixelX(j);
                tiles[tile_count].pixel_y = getPixelY(i);
                tiles[tile_count].dest_x = tiles[tile_count].pixel_x;
                tiles[tile_count].dest_y = tiles[tile_count].pixel_y;
                tiles[tile_count].to_delete = 0;
                tiles[tile_count].scale = 100;
                tile_count++;
            } else {
                id_grid[i][j] = 0;
            }
        }
    }
}

/* 带有 ID 追踪的行合并逻辑 */
void slideAndMergeRowIds(int row_idx) {
    int j;
    int pos = 0;
    int temp_ids[GRID_SIZE];
    int write_pos = 0;
    int curr_id, next_id;
    int curr_idx, next_idx;
    
    /* 1. 提取非空 ID */
    for(j=0; j<GRID_SIZE; j++) {
        if(id_grid[row_idx][j] != 0) {
            temp_ids[pos++] = id_grid[row_idx][j];
            id_grid[row_idx][j] = 0; /* 清空原位置 */
        }
    }
    
    /* 2. 合并和放置 */
    for(j=0; j<pos; j++) {
        curr_id = temp_ids[j];
        next_id = (j < pos - 1) ? temp_ids[j+1] : 0;
        
        curr_idx = findTileIndex(curr_id);
        next_idx = (next_id != 0) ? findTileIndex(next_id) : -1;
        
        if(next_id != 0 && tiles[curr_idx].val == tiles[next_idx].val) {
            /* 发生合并 */
            /* 移动两个方块到同一个位置 write_pos */
            tiles[curr_idx].c = write_pos; /* 注意：这里只更新了 c，因为是行操作 */
            tiles[curr_idx].to_delete = 1; /* 这个块将消失 */
            
            tiles[next_idx].c = write_pos;
            tiles[next_idx].val *= 2; /* 升级 */
            tiles[next_idx].scale = 120; /* 放大效果 */
            score += tiles[next_idx].val;
            
            id_grid[row_idx][write_pos] = next_id;
            write_pos++;
            j++; /* 跳过下一个 */
        } else {
            /* 仅移动 */
            tiles[curr_idx].c = write_pos;
            id_grid[row_idx][write_pos] = curr_id;
            write_pos++;
        }
    }
}

/* 仅执行逻辑移动，不处理动画准备和状态转换 */
void logicMoveLeft(void) {
    int i;
    for(i=0; i<GRID_SIZE; i++) {
        slideAndMergeRowIds(i);
    }
}

/* 计算目标位置并启动动画 */
void startAnimation(void) {
    int i;
    int moved = 0;
    
    for(i=0; i<tile_count; i++) {
        /* 此时 r, c 应该是恢复到真实世界方向的逻辑坐标 */
        tiles[i].dest_x = getPixelX(tiles[i].c);
        tiles[i].dest_y = getPixelY(tiles[i].r);
        
        if(tiles[i].pixel_x != tiles[i].dest_x || tiles[i].pixel_y != tiles[i].dest_y) {
            moved = 1;
        }
    }
    
    if(moved) {
        game_state = STATE_ANIMATING;
    }
}

void moveLeft(void) {
    prepareAnimation();
    logicMoveLeft();
    startAnimation();
}

/* 旋转矩阵 90度 顺时针 */
/* 同时旋转 id_grid 和更新 tiles 中的逻辑坐标 */
void rotateGrid(void) {
    int i, j;
    int new_grid[GRID_SIZE][GRID_SIZE];
    int new_id_grid[GRID_SIZE][GRID_SIZE];

    /* 旋转 grid 和 id_grid */
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            new_grid[j][GRID_SIZE - 1 - i] = grid[i][j];
            new_id_grid[j][GRID_SIZE - 1 - i] = id_grid[i][j];
        }
    }

    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = new_grid[i][j];
            id_grid[i][j] = new_id_grid[i][j];
        }
    }
    
    /* 更新 tiles 中的逻辑坐标 r, c */
    /* (r, c) -> (c, 3-r) */
    for(i=0; i<tile_count; i++) {
        int old_r = tiles[i].r;
        int old_c = tiles[i].c;
        tiles[i].r = old_c;
        tiles[i].c = GRID_SIZE - 1 - old_r;
    }
}

void moveRight(void) {
    prepareAnimation();
    rotateGrid();
    rotateGrid();
    logicMoveLeft();
    rotateGrid();
    rotateGrid();
    startAnimation();
}

void moveUp(void) {
    prepareAnimation();
    rotateGrid();
    rotateGrid();
    rotateGrid();
    logicMoveLeft();
    rotateGrid();
    startAnimation();
}

void moveDown(void) {
    prepareAnimation();
    rotateGrid();
    logicMoveLeft();
    rotateGrid();
    rotateGrid();
    rotateGrid();
    startAnimation();
}

/* -------------------------------------------------------------------------- */
/*                                输入处理                                    */
/* -------------------------------------------------------------------------- */

void handleInput(int key) {
    mrc_printf("2048: Input key=%d state=%d\n", key, game_state);
    if (!seeded) {
        mrc_sand(frame_counter);
        seeded = 1;
    }

    if (game_state == STATE_MENU) {
        pmenu_event(&main_menu, KY_UP, key, 0);
    } else if (game_state == STATE_PLAYING) {
        switch (key) {
            case _UP:    moveUp(); break;
            case _DOWN:  moveDown(); break;
            case _LEFT:  moveLeft(); break;
            case _RIGHT: moveRight(); break;
            case _SLEFT: resetGame(); break; /* 重玩 */
            case _SRIGHT: 
                game_state = STATE_MENU; 
                pmenu_draw(&main_menu);
                break; /* 返回菜单 */
        }
    } else if (game_state == STATE_GAMEOVER || game_state == STATE_WIN) {
        if (key == _SLEFT || key == _SELECT) {
            resetGame();
        } else if (key == _SRIGHT) {
            game_state = STATE_MENU;
            pmenu_draw(&main_menu);
        }
    } else if (game_state == STATE_HELP || game_state == STATE_ABOUT) {
        if (key == _SRIGHT || key == _SLEFT || key == _SELECT) {
            game_state = STATE_MENU;
            pmenu_draw(&main_menu);
        }
    }
}

void handleTouch(int type, int x, int y) {
    int dx, dy;
    
    if (game_state == STATE_MENU) {
        /* 菜单触屏处理 */
        /* game_menu.c 中定义 MR_MOUSE_DOWN 为 MS_UP */
        if (type == MS_UP) {
            pmenu_event(&main_menu, MS_UP, x, y);
        }
        return;
    }

    if (type == MS_DOWN) {
        touch_start_x = x;
        touch_start_y = y;
        is_touching = 1;
    } else if (type == MS_UP && is_touching) {
        is_touching = 0;
        dx = x - touch_start_x;
        dy = y - touch_start_y;

        /* 优先检测软键点击 (底部区域) */
        if (y > SCRH - 40 && (dx*dx + dy*dy < 400)) {
             if (x < SCRW / 2) {
                 /* 左软键: 重玩 */
                 if (game_state == STATE_PLAYING || game_state == STATE_GAMEOVER) {
                     resetGame();
                 }
             } else {
                 /* 右软键: 菜单/返回 */
                 game_state = STATE_MENU;
                 pmenu_draw(&main_menu);
             }
             return;
        }

        if (game_state == STATE_PLAYING) {
            if ((dx * dx + dy * dy) > 400) { /* 移动距离大于 20px */
                if ((dx > 0 ? dx : -dx) > (dy > 0 ? dy : -dy)) {
                    /* 水平移动 */
                    if (dx > 0) moveRight();
                    else moveLeft();
                } else {
                    /* 垂直移动 */
                    if (dy > 0) moveDown();
                    else moveUp();
                }
            }
        } else {
             /* 简单的点击处理 (非软键区域) */
             if (game_state == STATE_GAMEOVER) resetGame();
             else if (game_state == STATE_HELP || game_state == STATE_ABOUT) {
                 game_state = STATE_MENU;
                 pmenu_draw(&main_menu);
             }
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                                绘图函数                                    */
/* -------------------------------------------------------------------------- */

int getTileColor(int val) {
    switch(val) {
        case 0: return COLOR_TILE_EMPTY;
        case 2: return COLOR_TILE_2;
        case 4: return COLOR_TILE_4;
        case 8: return COLOR_TILE_8;
        case 16: return COLOR_TILE_16;
        case 32: return COLOR_TILE_32;
        case 64: return COLOR_TILE_64;
        case 128: return COLOR_TILE_128;
        case 256: return COLOR_TILE_256;
        case 512: return COLOR_TILE_512;
        case 1024: return COLOR_TILE_1024;
        case 2048: return COLOR_TILE_2048;
        default: return COLOR_TILE_2048;
    }
}

int getTextColor(int val) {
    return (val <= 4) ? COLOR_TEXT_DARK : COLOR_TEXT_LIGHT;
}

void drawTileAt(int x, int y, int size, int val) {
    char str[8];
    int text_w, text_h;
    int color;

    color = getTileColor(val);
    
    /* 绘制方块背景 */
    gl_drawRect(x, y, size, size, color);

    /* 绘制数字 */
    if (val > 0) {
        mrc_sprintf(str, "%d", val);
        text_w = uc3_getWidth(str, 1); /* 1: 大字体 */
        text_h = 16; /* 假设大字体高度 */
        
        uc3_drawText(str, x + (size - text_w)/2, y + (size - text_h)/2, 
                     (getTextColor(val) >> 16) & 0xFF,
                     (getTextColor(val) >> 8) & 0xFF,
                     (getTextColor(val)) & 0xFF, 0);
    }
}

void drawGame(void) {
    int i, j;
    int bg_w, bg_h;
    // mrc_printf("draw game...");
    /* 清屏 */
    mrc_clearScreen(250, 248, 239); /* COLOR_BG */

    if (game_state == STATE_MENU) {
        pmenu_draw(&main_menu);
    } else if (game_state == STATE_HELP) {
        drawHelp();
    } else if (game_state == STATE_ABOUT) {
        drawAbout();
    } else {
        /* 绘制分数 */
        mrc_printf("draw score...");
        drawScore();

        /* 绘制棋盘背景 */
        bg_w = GRID_SIZE * tile_size + (GRID_SIZE + 1) * TILE_GAP;
        bg_h = bg_w;
        mrc_printf("draw grid bg...");
        gl_drawRect(grid_offset_x, grid_offset_y, bg_w, bg_h, COLOR_GRID_BG);
        mrc_printf("draw tiles...");
        /* 绘制背景空方块 */
        for (i = 0; i < GRID_SIZE; i++) {
            for (j = 0; j < GRID_SIZE; j++) {
                int xx = getPixelX(j);
                int yy = getPixelY(i);
                drawTileAt(xx, yy, tile_size, 0);
            }
        }

        /* 绘制前景方块 */
        if (game_state == STATE_ANIMATING) {
            for(i=0; i<tile_count; i++) {
                int size = tile_size * tiles[i].scale / 100;
                int offset = (tile_size - size) / 2;
                drawTileAt(tiles[i].pixel_x + offset, tiles[i].pixel_y + offset, size, tiles[i].val);
            }
        } else {
            for (i = 0; i < GRID_SIZE; i++) {
                for (j = 0; j < GRID_SIZE; j++) {
                    if (grid[i][j] != 0) {
                        int xx = getPixelX(j);
                        int yy = getPixelY(i);
                        drawTileAt(xx, yy, tile_size, grid[i][j]);
                    }
                }
            }
        }

        if (game_state == STATE_GAMEOVER) {
            drawGameOver();
        } else if (game_state == STATE_WIN) {
            /* 简单处理胜利，继续玩 */
        }
        mrc_printf("draw softkeys...");
        /* 绘制软键 */
        drawSoftKeys("重玩", "菜单");
    }

    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

void drawScore(void) {
    char str[32];
    mrc_sprintf(str, "Score: %d", score);
    mrc_printf("draw score text...");
    uc3_drawText(str, grid_offset_x, grid_offset_y - 25, 119, 110, 101, 0); /* COLOR_TEXT_DARK */
}

void switchWindow(int windex, int leve) {
    mrc_printf("2048: switchWindow leve=%d\n", leve);
    if (leve == 0) {
        resetGame();
    } else if (leve == 1) {
        game_state = STATE_HELP;
        drawGame();
    } else if (leve == 2) {
        game_state = STATE_ABOUT;
        drawGame();
    } else if (leve == 3) {
        mrc_exit();
    }
}

void drawHelp(void) {
    int y = 40;
    uc3_drawText("游戏帮助", 10, y, 0, 0, 0, 0); y += 30;
    uc3_drawText("滑动或按键移动方块", 10, y, 100, 100, 100, 0); y += 20;
    uc3_drawText("相同数字合并升级", 10, y, 100, 100, 100, 0); y += 20;
    uc3_drawText("目标是凑出2048!", 10, y, 100, 100, 100, 0);
    drawSoftKeys(NULL, "返回");
}

void drawAbout(void) {
    int y = 60;
    int w;
    char* title = "2048 功能机版";
    w = uc3_getWidth(title, 0);
    uc3_drawText(title, (SCRW - w)/2, y, 0, 0, 0, 0); y += 40;
    
    title = "Ver 1.0";
    w = uc3_getWidth(title, 0);
    uc3_drawText(title, (SCRW - w)/2, y, 100, 100, 100, 0);

    title = "风的影子 制作";
    w = uc3_getWidth(title, 0);
    uc3_drawText(title, (SCRW - w)/2, y + 30, 100, 100, 100, 0);
    
    drawSoftKeys(NULL, "返回");
}

void drawGameOver(void) {
    int w, h, x, y;
    char* text = "Game Over";
    int text_w;
    
    w = 160;
    h = 60;
    x = (SCRW - w) / 2;
    y = (SCRH - h) / 2;
    
    /* 半透明覆盖层效果模拟 (画个框) */
    gl_drawRect(x, y, w, h, 0x80EEE4DA); /* 半透明背景 */
    
    text_w = uc3_getWidth(text, 1);
    uc3_drawText(text, (SCRW - text_w)/2, y + 20, 119, 110, 101, 0);
}

void drawSoftKeys(char* left, char* right) {
    int y = SCRH - 20;
    int w;
    
    if (left) {
        uc3_drawText(left, 5, y, 0, 0, 0, 0);
    }
    
    if (right) {
        w = uc3_getWidth(right, 0);
        uc3_drawText(right, SCRW - w - 5, y, 0, 0, 0, 0);
    }
}
