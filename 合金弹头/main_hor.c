/*
 * Metal Slug - 合金弹头 (横屏版本)
 * 功能机伪横屏游戏 (320x240)
 * 使用横屏绘图库在竖屏设备上实现横屏效果
 * 遵循C98标准
 */

#include <mrc_base.h>
#include "uc3_font.h"
#include "mrc_graphics.h"
#include "mrc_graphics_hor.h"
#include "mpc.h"

/* ==============================
 * 常量定义
 * ============================== */
#define MAX_ENEMIES 30
#define MAX_BULLETS 50
#define MAX_PARTICLES 100
#define MAX_PLATFORMS 30

#define GRAVITY 1
#define PLAYER_SPEED 3
#define PLAYER_JUMP_FORCE 10
#define BULLET_SPEED 8

#define GAME_FPS 30
#define TIMER_INTERVAL 33  /* 33ms = 30fps */

/* 窗口类型 */
enum {
    WINDOW_MENU = 0,
    WINDOW_GAME = 1,
    WINDOW_HELP = 2,
    WINDOW_GAMEOVER = 3
};

/* 实体类型 */
enum {
    ENTITY_NONE = 0,
    ENTITY_PLAYER = 1,
    ENTITY_ENEMY_SOLDIER = 2,
    ENTITY_ENEMY_ELITE = 3,
    ENTITY_ENEMY_TANK = 4
};

/* 平台类型 */
enum {
    PLATFORM_SOLID = 0,
    PLATFORM_ONEWAY = 1
};

/* ==============================
 * 数据结构定义
 * ============================== */

/* 实体基础结构 */
typedef struct {
    int16 x;
    int16 y;
    int16 w;
    int16 h;
    int16 vx;
    int16 vy;
    int16 hp;
    int16 maxHp;
    uint8 active;
    uint8 type;
    uint8 facingRight;
    uint8 flashTimer;
} Entity;

/* 子弹 */
typedef struct {
    int16 x;
    int16 y;
    int16 vx;
    uint8 active;
    uint8 isPlayer;
    int16 life;
} Bullet;

/* 粒子 */
typedef struct {
    int16 x;
    int16 y;
    int16 vx;
    int16 vy;
    uint8 active;
    int16 life;
    uint32 color;
} Particle;

/* 平台 */
typedef struct {
    int16 x;
    int16 y;
    int16 w;
    int16 h;
    uint8 type;
} Platform;

/* 关卡数据 */
typedef struct {
    Platform platforms[MAX_PLATFORMS];
    int16 platformCount;
    int16 mapWidth;
} LevelData;

/* 敌人生成数据 */
typedef struct {
    int16 x;
    int16 y;
    uint8 type;
} EnemySpawn;

/* 游戏状态 */
typedef struct {
    int32 score;
    int16 cameraX;
    int16 shake;
    int16 frame;
    uint8 running;
    uint8 bossActive;
    uint8 gameOver;
    uint8 currentLevel;
    uint8 totalLevels;
} GameState;

/* 玩家状态 */
typedef struct {
    Entity base;
    uint8 onGround;
    int16 shootTimer;
    int16 invincible;
} Player;

/* 敌人状态 */
typedef struct {
    Entity base;
    int16 timer;
} Enemy;

/* ==============================
 * 全局变量
 * ============================== */
int currentWindow;
int32 timer_cd;

GameState gameState;
Player player;
Enemy enemies[MAX_ENEMIES];
Bullet bullets[MAX_BULLETS];
Particle particles[MAX_PARTICLES];
LevelData currentLevelData;

/* 按键状态 */
uint8 keyLeft, keyRight, keyUp, keyDown, keyJump, keyShoot;
uint8 keyJumpPressed, keyShootPressed;

/* ==============================
 * 关卡数据
 * ============================== */
const EnemySpawn level1Enemies[] = {
    {200, 140, ENTITY_ENEMY_SOLDIER},
    {350, 140, ENTITY_ENEMY_SOLDIER},
    {500, 140, ENTITY_ENEMY_SOLDIER},
    {650, 140, ENTITY_ENEMY_ELITE},
    {800, 140, ENTITY_ENEMY_SOLDIER},
    {950, 140, ENTITY_ENEMY_ELITE},
    {1100, 140, ENTITY_ENEMY_SOLDIER},
    {1250, 140, ENTITY_ENEMY_SOLDIER},
    {1400, 140, ENTITY_ENEMY_ELITE},
    {1700, 140, ENTITY_ENEMY_TANK}
};

const EnemySpawn level2Enemies[] = {
    {150, 140, ENTITY_ENEMY_SOLDIER},
    {280, 140, ENTITY_ENEMY_ELITE},
    {400, 140, ENTITY_ENEMY_SOLDIER},
    {530, 140, ENTITY_ENEMY_ELITE},
    {670, 140, ENTITY_ENEMY_SOLDIER},
    {800, 140, ENTITY_ENEMY_ELITE},
    {930, 140, ENTITY_ENEMY_SOLDIER},
    {1060, 140, ENTITY_ENEMY_ELITE},
    {1200, 140, ENTITY_ENEMY_SOLDIER},
    {1340, 140, ENTITY_ENEMY_ELITE},
    {1480, 140, ENTITY_ENEMY_SOLDIER},
    {1800, 140, ENTITY_ENEMY_TANK}
};

const EnemySpawn level3Enemies[] = {
    {120, 140, ENTITY_ENEMY_ELITE},
    {230, 140, ENTITY_ENEMY_ELITE},
    {350, 140, ENTITY_ENEMY_SOLDIER},
    {470, 140, ENTITY_ENEMY_ELITE},
    {590, 140, ENTITY_ENEMY_ELITE},
    {710, 140, ENTITY_ENEMY_SOLDIER},
    {830, 140, ENTITY_ENEMY_ELITE},
    {950, 140, ENTITY_ENEMY_ELITE},
    {1070, 140, ENTITY_ENEMY_SOLDIER},
    {1200, 140, ENTITY_ENEMY_ELITE},
    {1330, 140, ENTITY_ENEMY_ELITE},
    {1460, 140, ENTITY_ENEMY_ELITE},
    {1590, 140, ENTITY_ENEMY_SOLDIER},
    {1900, 140, ENTITY_ENEMY_TANK}
};

/* ==============================
 * 函数前向声明
 * ============================== */
void initGame(void);
void updateGame(void);
void drawGame(void);
void gameEvent(int32 code, int32 param0, int32 param1);
void loadLevel(int level);
void resetPlayer(void);
void updatePlayer(void);
void updateEnemies(void);
void updateBullets(void);
void updateParticles(void);
void drawPlayer(void);
void drawEnemies(void);
void drawBullets(void);
void drawParticles(void);
void drawPlatforms(void);
void drawUI(void);
void drawSoftKeyHints(char* left, char* right);
void createExplosion(int16 x, int16 y, int16 count);
void createBullet(int16 x, int16 y, int16 vx, uint8 isPlayer);
void createParticle(int16 x, int16 y, int16 vx, int16 vy, int16 life, uint32 color);
void spawnEnemies(const EnemySpawn* spawns, int count);
void checkCollisions(void);
void playerShoot(void);
void playerTakeDamage(int16 damage);
void enemyTakeDamage(int idx, int16 damage);
void nextLevel(void);
void showGameOver(void);
void showVictory(void);
void switchWindow(int windex, int level);
void timer_run(int32 id);
void checkLevelComplete(void);
void drawGameOver(void);
void gameOverEvent(int32 code, int32 param0, int32 param1);

/* ==============================
 * 主要函数实现
 * ============================== */

/* 初始化游戏 */
void initGame(void) {
    int i;

    /* 初始化游戏状态 */
    gameState.score = 0;
    gameState.cameraX = 0;
    gameState.shake = 0;
    gameState.frame = 0;
    gameState.running = 1;
    gameState.bossActive = 0;
    gameState.gameOver = 0;
    gameState.currentLevel = 1;
    gameState.totalLevels = 3;

    /* 初始化按键状态 */
    keyLeft = 0;
    keyRight = 0;
    keyUp = 0;
    keyDown = 0;
    keyJump = 0;
    keyShoot = 0;
    keyJumpPressed = 0;
    keyShootPressed = 0;

    /* 清空所有实体 */
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].base.active = 0;
    }
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    for (i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = 0;
    }

    /* 加载第一关 */
    loadLevel(1);
}

/* 加载关卡 */
void loadLevel(int level) {
    int i;

    gameState.currentLevel = level;
    gameState.bossActive = 0;
    gameState.cameraX = 0;

    /* 清空实体 */
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].base.active = 0;
    }
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    for (i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = 0;
    }

    /* 根据关卡创建平台 */
    if (level == 1) {
        currentLevelData.platformCount = 8;
        currentLevelData.mapWidth = 2000;

        /* 地面 */
        currentLevelData.platforms[0].x = 0;
        currentLevelData.platforms[0].y = 200;
        currentLevelData.platforms[0].w = 2000;
        currentLevelData.platforms[0].h = 40;
        currentLevelData.platforms[0].type = PLATFORM_SOLID;

        /* 跳跃平台 */
        currentLevelData.platforms[1].x = 150;
        currentLevelData.platforms[1].y = 150;
        currentLevelData.platforms[1].w = 80;
        currentLevelData.platforms[1].h = 10;
        currentLevelData.platforms[1].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[2].x = 300;
        currentLevelData.platforms[2].y = 130;
        currentLevelData.platforms[2].w = 80;
        currentLevelData.platforms[2].h = 10;
        currentLevelData.platforms[2].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[3].x = 450;
        currentLevelData.platforms[3].y = 160;
        currentLevelData.platforms[3].w = 80;
        currentLevelData.platforms[3].h = 10;
        currentLevelData.platforms[3].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[4].x = 600;
        currentLevelData.platforms[4].y = 140;
        currentLevelData.platforms[4].w = 90;
        currentLevelData.platforms[4].h = 10;
        currentLevelData.platforms[4].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[5].x = 800;
        currentLevelData.platforms[5].y = 120;
        currentLevelData.platforms[5].w = 80;
        currentLevelData.platforms[5].h = 10;
        currentLevelData.platforms[5].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[6].x = 1000;
        currentLevelData.platforms[6].y = 150;
        currentLevelData.platforms[6].w = 100;
        currentLevelData.platforms[6].h = 10;
        currentLevelData.platforms[6].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[7].x = 1200;
        currentLevelData.platforms[7].y = 130;
        currentLevelData.platforms[7].w = 80;
        currentLevelData.platforms[7].h = 10;
        currentLevelData.platforms[7].type = PLATFORM_ONEWAY;

        spawnEnemies(level1Enemies, sizeof(level1Enemies) / sizeof(EnemySpawn));

    } else if (level == 2) {
        currentLevelData.platformCount = 10;
        currentLevelData.mapWidth = 2200;

        /* 地面 */
        currentLevelData.platforms[0].x = 0;
        currentLevelData.platforms[0].y = 200;
        currentLevelData.platforms[0].w = 2200;
        currentLevelData.platforms[0].h = 40;
        currentLevelData.platforms[0].type = PLATFORM_SOLID;

        /* 更多平台 */
        currentLevelData.platforms[1].x = 120;
        currentLevelData.platforms[1].y = 140;
        currentLevelData.platforms[1].w = 70;
        currentLevelData.platforms[1].h = 10;
        currentLevelData.platforms[1].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[2].x = 250;
        currentLevelData.platforms[2].y = 110;
        currentLevelData.platforms[2].w = 70;
        currentLevelData.platforms[2].h = 10;
        currentLevelData.platforms[2].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[3].x = 380;
        currentLevelData.platforms[3].y = 150;
        currentLevelData.platforms[3].w = 80;
        currentLevelData.platforms[3].h = 10;
        currentLevelData.platforms[3].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[4].x = 520;
        currentLevelData.platforms[4].y = 120;
        currentLevelData.platforms[4].w = 100;
        currentLevelData.platforms[4].h = 10;
        currentLevelData.platforms[4].type = PLATFORM_SOLID;

        currentLevelData.platforms[5].x = 680;
        currentLevelData.platforms[5].y = 160;
        currentLevelData.platforms[5].w = 80;
        currentLevelData.platforms[5].h = 10;
        currentLevelData.platforms[5].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[6].x = 820;
        currentLevelData.platforms[6].y = 130;
        currentLevelData.platforms[6].w = 90;
        currentLevelData.platforms[6].h = 10;
        currentLevelData.platforms[6].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[7].x = 970;
        currentLevelData.platforms[7].y = 100;
        currentLevelData.platforms[7].w = 80;
        currentLevelData.platforms[7].h = 10;
        currentLevelData.platforms[7].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[8].x = 1120;
        currentLevelData.platforms[8].y = 140;
        currentLevelData.platforms[8].w = 100;
        currentLevelData.platforms[8].h = 10;
        currentLevelData.platforms[8].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[9].x = 1280;
        currentLevelData.platforms[9].y = 110;
        currentLevelData.platforms[9].w = 80;
        currentLevelData.platforms[9].h = 10;
        currentLevelData.platforms[9].type = PLATFORM_ONEWAY;

        spawnEnemies(level2Enemies, sizeof(level2Enemies) / sizeof(EnemySpawn));

    } else if (level == 3) {
        currentLevelData.platformCount = 12;
        currentLevelData.mapWidth = 2400;

        /* 地面 */
        currentLevelData.platforms[0].x = 0;
        currentLevelData.platforms[0].y = 200;
        currentLevelData.platforms[0].w = 2400;
        currentLevelData.platforms[0].h = 40;
        currentLevelData.platforms[0].type = PLATFORM_SOLID;

        /* 最终关卡平台 - 更复杂 */
        currentLevelData.platforms[1].x = 100;
        currentLevelData.platforms[1].y = 150;
        currentLevelData.platforms[1].w = 60;
        currentLevelData.platforms[1].h = 10;
        currentLevelData.platforms[1].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[2].x = 200;
        currentLevelData.platforms[2].y = 110;
        currentLevelData.platforms[2].w = 60;
        currentLevelData.platforms[2].h = 10;
        currentLevelData.platforms[2].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[3].x = 300;
        currentLevelData.platforms[3].y = 80;
        currentLevelData.platforms[3].w = 70;
        currentLevelData.platforms[3].h = 10;
        currentLevelData.platforms[3].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[4].x = 410;
        currentLevelData.platforms[4].y = 120;
        currentLevelData.platforms[4].w = 80;
        currentLevelData.platforms[4].h = 10;
        currentLevelData.platforms[4].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[5].x = 540;
        currentLevelData.platforms[5].y = 160;
        currentLevelData.platforms[5].w = 70;
        currentLevelData.platforms[5].h = 10;
        currentLevelData.platforms[5].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[6].x = 650;
        currentLevelData.platforms[6].y = 120;
        currentLevelData.platforms[6].w = 100;
        currentLevelData.platforms[6].h = 10;
        currentLevelData.platforms[6].type = PLATFORM_SOLID;

        currentLevelData.platforms[7].x = 800;
        currentLevelData.platforms[7].y = 140;
        currentLevelData.platforms[7].w = 80;
        currentLevelData.platforms[7].h = 10;
        currentLevelData.platforms[7].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[8].x = 930;
        currentLevelData.platforms[8].y = 100;
        currentLevelData.platforms[8].w = 70;
        currentLevelData.platforms[8].h = 10;
        currentLevelData.platforms[8].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[9].x = 1060;
        currentLevelData.platforms[9].y = 130;
        currentLevelData.platforms[9].w = 90;
        currentLevelData.platforms[9].h = 10;
        currentLevelData.platforms[9].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[10].x = 1200;
        currentLevelData.platforms[10].y = 90;
        currentLevelData.platforms[10].w = 80;
        currentLevelData.platforms[10].h = 10;
        currentLevelData.platforms[10].type = PLATFORM_ONEWAY;

        currentLevelData.platforms[11].x = 1350;
        currentLevelData.platforms[11].y = 140;
        currentLevelData.platforms[11].w = 100;
        currentLevelData.platforms[11].h = 10;
        currentLevelData.platforms[11].type = PLATFORM_ONEWAY;

        spawnEnemies(level3Enemies, sizeof(level3Enemies) / sizeof(EnemySpawn));
    }

    /* 重置玩家 */
    resetPlayer();
}

/* 生成敌人 */
void spawnEnemies(const EnemySpawn* spawns, int count) {
    int i;

    for (i = 0; i < count && i < MAX_ENEMIES; i++) {
        enemies[i].base.active = 1;
        enemies[i].base.x = spawns[i].x;
        enemies[i].base.y = spawns[i].y;
        enemies[i].base.vx = 0;
        enemies[i].base.vy = 0;
        enemies[i].base.type = spawns[i].type;
        enemies[i].base.facingRight = 0;
        enemies[i].base.flashTimer = 0;
        enemies[i].timer = 0;

        if (spawns[i].type == ENTITY_ENEMY_TANK) {
            enemies[i].base.w = 60;
            enemies[i].base.h = 40;
            enemies[i].base.hp = 300;
            enemies[i].base.maxHp = 300;
        } else if (spawns[i].type == ENTITY_ENEMY_ELITE) {
            enemies[i].base.w = 20;
            enemies[i].base.h = 25;
            enemies[i].base.hp = 30;
            enemies[i].base.maxHp = 30;
        } else {
            enemies[i].base.w = 15;
            enemies[i].base.h = 25;
            enemies[i].base.hp = 20;
            enemies[i].base.maxHp = 20;
        }
    }
}

/* 重置玩家 */
void resetPlayer(void) {
    player.base.active = 1;
    player.base.x = 50;
    player.base.y = 150;
    player.base.w = 15;
    player.base.h = 25;
    player.base.vx = 0;
    player.base.vy = 0;
    player.base.hp = 100;
    player.base.maxHp = 100;
    player.base.type = ENTITY_PLAYER;
    player.base.facingRight = 1;
    player.base.flashTimer = 0;
    player.onGround = 0;
    player.shootTimer = 0;
    player.invincible = 0;
}

/* 创建子弹 */
void createBullet(int16 x, int16 y, int16 vx, uint8 isPlayer) {
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = 1;
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].vx = vx;
            bullets[i].isPlayer = isPlayer;
            bullets[i].life = 100;
            break;
        }
    }
}

/* 创建粒子 */
void createParticle(int16 x, int16 y, int16 vx, int16 vy, int16 life, uint32 color) {
    int i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = 1;
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = vx;
            particles[i].vy = vy;
            particles[i].life = life;
            particles[i].color = color;
            break;
        }
    }
}

/* 创建爆炸效果 */
void createExplosion(int16 x, int16 y, int16 count) {
    int i;
    int16 vx, vy;
    uint32 color;

    for (i = 0; i < count; i++) {
        vx = (mrc_rand() % 10) - 5;
        vy = (mrc_rand() % 10) - 5;
        color = (mrc_rand() % 2) ? 0xFFFF4444 : 0xFFFFCC00;
        createParticle(x, y, vx, vy, 20 + (mrc_rand() % 20), color);
    }
}

/* 玩家射击 */
void playerShoot(void) {
    int16 bx, bvx;

    player.shootTimer = 8;

    if (player.base.facingRight) {
        bx = player.base.x + player.base.w;
        bvx = BULLET_SPEED;
    } else {
        bx = player.base.x - 5;
        bvx = -BULLET_SPEED;
    }

    createBullet(bx, player.base.y + 8, bvx, 1);
}

/* 玩家受伤 */
void playerTakeDamage(int16 damage) {
    if (player.invincible > 0) return;

    player.base.hp -= damage;
    player.invincible = 60;
    gameState.shake = 5;

    if (player.base.hp <= 0) {
        player.base.hp = 0;
        gameState.gameOver = 1;
        gameState.running = 0;
        createExplosion(player.base.x + player.base.w/2, player.base.y + player.base.h/2, 30);
    }
}

/* 敌人受伤 */
void enemyTakeDamage(int idx, int16 damage) {
    enemies[idx].base.hp -= damage;
    enemies[idx].base.flashTimer = 5;

    if (enemies[idx].base.hp <= 0) {
        /* 敌人死亡 */
        createExplosion(enemies[idx].base.x + enemies[idx].base.w/2,
                       enemies[idx].base.y + enemies[idx].base.h/2, 15);
        enemies[idx].base.active = 0;
        gameState.shake = 8;

        /* 增加分数 */
        if (enemies[idx].base.type == ENTITY_ENEMY_TANK) {
            gameState.score += 5000;
        } else if (enemies[idx].base.type == ENTITY_ENEMY_ELITE) {
            gameState.score += 300;
        } else {
            gameState.score += 100;
        }
    }
}

/* 更新玩家 */
void updatePlayer(void) {
    int i;
    Platform* p;

    /* 左右移动 */
    if (keyRight) {
        player.base.vx = PLAYER_SPEED;
        player.base.facingRight = 1;
    } else if (keyLeft) {
        player.base.vx = -PLAYER_SPEED;
        player.base.facingRight = 0;
    } else {
        player.base.vx = 0;
    }

    /* 跳跃 */
    if (keyJumpPressed && player.onGround) {
        if (keyDown) {
            /* 下跳 */
            player.base.y += 1;
            player.onGround = 0;
            player.base.vy = 1;
        } else {
            player.base.vy = -PLAYER_JUMP_FORCE;
            player.onGround = 0;
        }
        keyJumpPressed = 0;
    }

    /* 射击 - 自动射击模式（像HTML版本一样） */
    if (player.shootTimer > 0) {
        player.shootTimer--;
    } else {
        /* 计时器归零时自动射击 */
        playerShoot();
    }

    /* 按下射击键可以立即射击（提前射击） */
    if (keyShootPressed && player.shootTimer > 0) {
        playerShoot();
        keyShootPressed = 0;
    }

    /* 物理更新 */
    player.base.vy += GRAVITY;
    player.base.x += player.base.vx;
    player.base.y += player.base.vy;

    /* 限制边界 */
    if (player.base.x < 0) player.base.x = 0;
    if (player.base.y > 300) {
        playerTakeDamage(100);
    }

    /* 平台碰撞 */
    player.onGround = 0;
    for (i = 0; i < currentLevelData.platformCount; i++) {
        p = &currentLevelData.platforms[i];

        if (player.base.x < p->x + p->w &&
            player.base.x + player.base.w > p->x &&
            player.base.y < p->y + p->h &&
            player.base.y + player.base.h > p->y) {

            /* 从上方落下 */
            if (player.base.vy > 0 && player.base.y + player.base.h - player.base.vy <= p->y + 10) {
                if (p->type == PLATFORM_SOLID || (p->type == PLATFORM_ONEWAY && !keyDown)) {
                    player.base.y = p->y - player.base.h;
                    player.base.vy = 0;
                    player.onGround = 1;
                }
            } else if (p->type == PLATFORM_SOLID) {
                /* 墙壁碰撞 */
                if (player.base.vx > 0) {
                    player.base.x = p->x - player.base.w;
                } else if (player.base.vx < 0) {
                    player.base.x = p->x + p->w;
                }
            }
        }
    }

    if (player.invincible > 0) player.invincible--;
    if (player.base.flashTimer > 0) player.base.flashTimer--;
}

/* 更新敌人 */
void updateEnemies(void) {
    int i, j;
    int16 dist;
    int16 bvx;
    Platform* p;

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].base.active) continue;

        if (enemies[i].base.type == ENTITY_ENEMY_SOLDIER) {
            /* 普通士兵AI */
            dist = player.base.x - enemies[i].base.x;
            enemies[i].base.facingRight = (dist > 0) ? 1 : 0;

            /* 射击 */
            if (dist > -200 && dist < 200 && (dist < -30 || dist > 30)) {
                enemies[i].timer++;
                if (enemies[i].timer > 80) {
                    bvx = enemies[i].base.facingRight ? 5 : -5;
                    createBullet(enemies[i].base.x + (enemies[i].base.facingRight ? 15 : 0),
                               enemies[i].base.y + 8, bvx, 0);
                    enemies[i].timer = 0;
                }
            }

            /* 重力 */
            enemies[i].base.vy += GRAVITY;
            enemies[i].base.y += enemies[i].base.vy;

            /* 平台碰撞 */
            for (j = 0; j < currentLevelData.platformCount; j++) {
                p = &currentLevelData.platforms[j];
                if (enemies[i].base.x < p->x + p->w &&
                    enemies[i].base.x + enemies[i].base.w > p->x &&
                    enemies[i].base.y < p->y + p->h &&
                    enemies[i].base.y + enemies[i].base.h > p->y) {
                    if (enemies[i].base.vy > 0) {
                        enemies[i].base.y = p->y - enemies[i].base.h;
                        enemies[i].base.vy = 0;
                    }
                }
            }

        } else if (enemies[i].base.type == ENTITY_ENEMY_ELITE) {
            /* 精英士兵AI - 更激进 */
            dist = player.base.x - enemies[i].base.x;
            enemies[i].base.facingRight = (dist > 0) ? 1 : 0;

            /* 靠近玩家 */
            if (dist > 60 && dist < 250) {
                enemies[i].base.vx = 1;
                enemies[i].base.x += enemies[i].base.vx;
            } else if (dist < -60 && dist > -250) {
                enemies[i].base.vx = -1;
                enemies[i].base.x += enemies[i].base.vx;
            }

            /* 快速三连发 */
            if (dist > -250 && dist < 250 && (dist < -30 || dist > 30)) {
                enemies[i].timer++;
                if (enemies[i].timer == 50 || enemies[i].timer == 58 || enemies[i].timer == 66) {
                    bvx = enemies[i].base.facingRight ? 6 : -6;
                    createBullet(enemies[i].base.x + (enemies[i].base.facingRight ? 20 : 0),
                               enemies[i].base.y + 10, bvx, 0);
                }
                if (enemies[i].timer > 150) enemies[i].timer = 0;
            }

            /* 重力和碰撞 */
            enemies[i].base.vy += GRAVITY;
            enemies[i].base.y += enemies[i].base.vy;

            for (j = 0; j < currentLevelData.platformCount; j++) {
                p = &currentLevelData.platforms[j];
                if (enemies[i].base.x < p->x + p->w &&
                    enemies[i].base.x + enemies[i].base.w > p->x &&
                    enemies[i].base.y < p->y + p->h &&
                    enemies[i].base.y + enemies[i].base.h > p->y) {
                    if (enemies[i].base.vy > 0) {
                        enemies[i].base.y = p->y - enemies[i].base.h;
                        enemies[i].base.vy = 0;
                    }
                }
            }

        } else if (enemies[i].base.type == ENTITY_ENEMY_TANK) {
            /* Boss坦克 */
            if (enemies[i].base.x - gameState.cameraX < 350) {
                gameState.bossActive = 1;
            }

            if (gameState.bossActive) {
                enemies[i].timer++;

                /* 移动 */
                if (enemies[i].timer % 200 < 100) {
                    enemies[i].base.x -= 1;
                } else {
                    enemies[i].base.x += 1;
                }

                /* 攻击 */
                if (enemies[i].timer % 90 == 0) {
                    createBullet(enemies[i].base.x, enemies[i].base.y + 15, -6, 0);
                }

                if (enemies[i].timer % 120 == 0) {
                    createBullet(enemies[i].base.x + 10, enemies[i].base.y - 5, -4, 0);
                    createBullet(enemies[i].base.x + 10, enemies[i].base.y + 5, -5, 0);
                }
            }
        }

        if (enemies[i].base.flashTimer > 0) enemies[i].base.flashTimer--;
    }
}

/* 更新子弹 */
void updateBullets(void) {
    int i;

    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        bullets[i].x += bullets[i].vx;
        bullets[i].life--;

        if (bullets[i].life <= 0 || bullets[i].x < -50 || bullets[i].x > currentLevelData.mapWidth + 50) {
            bullets[i].active = 0;
        }

        /* 拖尾粒子 */
        if (gameState.frame % 3 == 0) {
            createParticle(bullets[i].x, bullets[i].y, 0, 0, 5, 0xFFFFFFFF);
        }
    }
}

/* 更新粒子 */
void updateParticles(void) {
    int i;

    for (i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;

        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        particles[i].vy += GRAVITY / 2;
        particles[i].life--;

        if (particles[i].life <= 0) {
            particles[i].active = 0;
        }
    }
}

/* 碰撞检测 */
void checkCollisions(void) {
    int i, j;

    /* 子弹击中敌人 */
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active || !bullets[i].isPlayer) continue;

        for (j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].base.active) continue;

            if (bullets[i].x < enemies[j].base.x + enemies[j].base.w &&
                bullets[i].x + 5 > enemies[j].base.x &&
                bullets[i].y < enemies[j].base.y + enemies[j].base.h &&
                bullets[i].y + 3 > enemies[j].base.y) {

                enemyTakeDamage(j, 10);
                bullets[i].active = 0;
                createExplosion(bullets[i].x, bullets[i].y, 3);
                break;
            }
        }
    }

    /* 敌人子弹击中玩家 */
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active || bullets[i].isPlayer) continue;

        if (bullets[i].x < player.base.x + player.base.w &&
            bullets[i].x + 5 > player.base.x &&
            bullets[i].y < player.base.y + player.base.h &&
            bullets[i].y + 3 > player.base.y) {

            playerTakeDamage(10);
            bullets[i].active = 0;
        }
    }

    /* 玩家碰到敌人 */
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].base.active) continue;

        if (player.base.x < enemies[i].base.x + enemies[i].base.w &&
            player.base.x + player.base.w > enemies[i].base.x &&
            player.base.y < enemies[i].base.y + enemies[i].base.h &&
            player.base.y + player.base.h > enemies[i].base.y) {

            playerTakeDamage(5);
        }
    }
}

/* 检查关卡是否完成 */
void checkLevelComplete(void) {
    int i;
    int enemyCount;

    enemyCount = 0;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].base.active) {
            enemyCount++;
        }
    }

    if (enemyCount == 0) {
        /* 关卡完成 */
        gameState.running = 0;
        if (gameState.currentLevel < gameState.totalLevels) {
            nextLevel();
        } else {
            showVictory();
        }
    }
}

/* 下一关 */
void nextLevel(void) {
    gameState.currentLevel++;
    if (gameState.currentLevel <= gameState.totalLevels) {
        loadLevel(gameState.currentLevel);
        gameState.running = 1;
    }
}

/* 显示胜利 */
void showVictory(void) {
    currentWindow = WINDOW_GAMEOVER;
}

/* 更新游戏 */
void updateGame(void) {
    int16 targetCamX;
    int16 maxCameraX;

    if (!gameState.running) return;

    /* 相机跟随 */
    targetCamX = player.base.x - 100;
    if (targetCamX < 0) targetCamX = 0;
    maxCameraX = currentLevelData.mapWidth - 320;
    if (targetCamX > maxCameraX) targetCamX = maxCameraX;
    gameState.cameraX += (targetCamX - gameState.cameraX) / 10;

    /* 震动衰减 */
    if (gameState.shake > 0) {
        gameState.shake = (gameState.shake * 9) / 10;
        if (gameState.shake < 1) gameState.shake = 0;
    }

    updatePlayer();
    updateEnemies();
    updateBullets();
    updateParticles();
    checkCollisions();
    checkLevelComplete();

    gameState.frame++;
}

/* 绘制玩家 */
void drawPlayer(void) {
    int16 x, y;

    if (player.invincible > 0 && (gameState.frame / 4) % 2 == 0) return;

    x = player.base.x - gameState.cameraX;
    y = player.base.y;

    /* 身体 */
    gl_drawRect_hor(x + 3, y + 8, 9, 10, 0xFFC03928);  /* 马甲 */

    /* 腿 */
    gl_drawRect_hor(x + 4, y + 18, 3, 7, 0xFF5D4037);
    gl_drawRect_hor(x + 9, y + 18, 3, 7, 0xFF5D4037);

    /* 头 */
    gl_drawRect_hor(x + 4, y, 7, 8, 0xFFF1C40F);  /* 脸 */
    gl_drawRect_hor(x + 4, y + 1, 7, 2, 0xFFFFFFFF);  /* 头带 */

    /* 眼睛 */
    if (player.base.facingRight) {
        gl_drawRect_hor(x + 9, y + 4, 2, 1, 0xFF000000);
    } else {
        gl_drawRect_hor(x + 5, y + 4, 2, 1, 0xFF000000);
    }

    /* 枪 */
    if (player.base.facingRight) {
        gl_drawRect_hor(x + 10, y + 10, 10, 3, 0xFF555555);
    } else {
        gl_drawRect_hor(x - 5, y + 10, 10, 3, 0xFF555555);
    }
}

/* 绘制敌人 */
void drawEnemies(void) {
    int i;
    int16 x, y;
    uint32 color;

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].base.active) continue;

        x = enemies[i].base.x - gameState.cameraX;
        y = enemies[i].base.y;

        /* 屏幕外不绘制 */
        if (x < -100 || x > 420) continue;

        color = (enemies[i].base.flashTimer > 0 && (gameState.frame / 2) % 2 == 0) ? 0xFFFFFFFF : 0xFF2C3E50;

        if (enemies[i].base.type == ENTITY_ENEMY_SOLDIER) {
            /* 普通士兵 */
            gl_drawRect_hor(x, y + 5, 15, 20, color);  /* 身体 */
            gl_drawRect_hor(x, y, 15, 6, 0xFF95A5A6);  /* 头盔 */

            /* 枪 */
            if (enemies[i].base.facingRight) {
                gl_drawRect_hor(x + 10, y + 13, 10, 2, 0xFF000000);
            } else {
                gl_drawRect_hor(x - 5, y + 13, 10, 2, 0xFF000000);
            }

        } else if (enemies[i].base.type == ENTITY_ENEMY_ELITE) {
            /* 精英士兵 - 稍大 */
            gl_drawRect_hor(x, y + 6, 20, 19, color);  /* 身体 */
            gl_drawRect_hor(x + 2, y, 16, 7, 0xFF7F8C8D);  /* 头盔 */

            /* 枪 */
            if (enemies[i].base.facingRight) {
                gl_drawRect_hor(x + 15, y + 12, 12, 3, 0xFF000000);
            } else {
                gl_drawRect_hor(x - 7, y + 12, 12, 3, 0xFF000000);
            }

        } else if (enemies[i].base.type == ENTITY_ENEMY_TANK) {
            /* Boss坦克 */
            /* 履带 */
            gl_drawRect_hor(x + 5, y + 32, 50, 8, 0xFF5D6D7E);

            /* 车身 */
            gl_drawRect_hor(x, y + 15, 60, 17, 0xFF2874A6);

            /* 炮塔 */
            gl_drawRect_hor(x + 15, y, 30, 15, 0xFF1B4F72);

            /* 炮管 */
            gl_drawRect_hor(x - 10, y + 5, 25, 5, 0xFF000000);

            /* Boss血条 */
            gl_drawRect_hor(x, y - 10, 60, 5, 0xFF333333);
            gl_drawRect_hor(x, y - 10, (60 * enemies[i].base.hp) / enemies[i].base.maxHp, 5, 0xFFFF0000);
        }
    }
}

/* 绘制子弹 */
void drawBullets(void) {
    int i;
    int16 x, y;
    uint32 color;

    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        x = bullets[i].x - gameState.cameraX;
        y = bullets[i].y;

        if (x < -20 || x > 340) continue;

        color = bullets[i].isPlayer ? 0xFFFFCC00 : 0xFFFF4444;
        gl_drawRect_hor(x, y, 5, 3, color);
    }
}

/* 绘制粒子 */
void drawParticles(void) {
    int i;
    int16 x, y;

    for (i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;

        x = particles[i].x - gameState.cameraX;
        y = particles[i].y;

        if (x < -10 || x > 330) continue;

        gl_drawRect_hor(x, y, 2, 2, particles[i].color);
    }
}

/* 绘制平台 */
void drawPlatforms(void) {
    int i;
    int16 x, y, w, h;
    Platform* p;

    for (i = 0; i < currentLevelData.platformCount; i++) {
        p = &currentLevelData.platforms[i];

        x = p->x - gameState.cameraX;
        y = p->y;
        w = p->w;
        h = p->h;

        if (x + w < 0 || x > 320) continue;

        if (p->type == PLATFORM_SOLID) {
            gl_drawRect_hor(x, y, w, h, 0xFF795548);  /* 泥土 */
            gl_drawRect_hor(x, y, w, 5, 0xFF27AE60);  /* 草皮 */
        } else {
            gl_drawRect_hor(x, y, w, h, 0xFF555555);  /* 钢板 */
        }
    }
}

/* 绘制UI */
void drawUI(void) {
    char buf[32];
    int16 hpWidth;

    /* 分数 */
    mrc_sprintf(buf, "SCORE:%06d", gameState.score);
    uc3_drawTextHor(buf, 5, 5, 255, 255, 255, 0);

    /* 关卡 */
    mrc_sprintf(buf, "LEVEL %d", gameState.currentLevel);
    uc3_drawTextHor(buf, 240, 5, 255, 255, 255, 0);

    /* HP条 */
    gl_drawRect_hor(100, 8, 102, 12, 0xFF555555);
    hpWidth = (player.base.hp * 100) / player.base.maxHp;
    gl_drawRect_hor(101, 9, hpWidth, 10, 0xFFFFCC00);

    /* Boss警告 */
    if (gameState.bossActive) {
        uc3_drawTextHor("WARNING", 120, 30, 255, 0, 0, 0);
    }
}

/* 绘制软键提示 */
void drawSoftKeyHints(char* left, char* right) {
    int16 leftX, rightX, y;
    int16 textW;

    y = 220;

    if (left != NULL) {
        leftX = 5;
        uc3_drawTextHor(left, leftX, y, 255, 255, 255, 0);
    }

    if (right != NULL) {
        textW = uc3_getWidth(right, 0);
        rightX = 315 - textW;
        uc3_drawTextHor(right, rightX, y, 255, 255, 255, 0);
    }
}

/* 绘制游戏 */
void drawGame(void) {
    /* 清屏 - 天空蓝 */
    gl_clearScreen_hor(135, 206, 235);

    /* 绘制游戏元素 */
    drawPlatforms();
    drawParticles();
    drawEnemies();
    drawPlayer();
    drawBullets();

    /* 绘制UI */
    drawUI();
    drawSoftKeyHints(NULL, "返回");

    mrc_refreshScreen_hor(0, 0, 320, 240);
}

/* 绘制游戏结束画面 */
void drawGameOver(void) {
    char buf[64];

    gl_clearScreen_hor(0, 0, 0);

    if (gameState.currentLevel > gameState.totalLevels) {
        /* 胜利 */
        uc3_drawTextHor("任务完成!", 100, 80, 255, 255, 0, 0);
        mrc_sprintf(buf, "最终分数: %d", gameState.score);
        uc3_drawTextHor(buf, 80, 110, 255, 255, 255, 0);
    } else {
        /* 失败 */
        uc3_drawTextHor("任务失败", 100, 90, 255, 0, 0, 0);
        mrc_sprintf(buf, "分数: %d", gameState.score);
        uc3_drawTextHor(buf, 100, 120, 255, 255, 255, 0);
    }

    uc3_drawTextHor("按确认重新开始", 80, 160, 255, 255, 255, 0);

    drawSoftKeyHints("重玩", "退出");
    mrc_refreshScreen_hor(0, 0, 320, 240);
}

/* 窗口切换 */
void switchWindow(int windex, int level) {
    if (windex == 1) {
        /* 进入游戏 */
        currentWindow = WINDOW_GAME;
        initGame();
        drawGame();
    } else if (windex == 0) {
        /* 返回菜单 - 暂时直接退出 */
        mrc_exit();
    }
}

/* 游戏事件处理 - 横屏按键映射 */
void gameEvent(int32 code, int32 param0, int32 param1) {
    if (code == KY_DOWN) {
        /* 按键按下 - 横屏模式：上下键控制左右移动 */
        if (param0 == _UP || param0 == _2) keyLeft = 1;        /* 上键 -> 左移 */
        if (param0 == _DOWN || param0 == _8) keyRight = 1;     /* 下键 -> 右移 */
        if (param0 == _RIGHT || param0 == _6) keyUp = 1;       /* 右键 -> 上（未使用）*/
        if (param0 == _LEFT || param0 == _4) keyDown = 1;      /* 左键 -> 下蹲 */
        if (param0 == _SELECT || param0 == _5) {
            keyJump = 1;
            keyJumpPressed = 1;
        }
        if (param0 == _0 || param0 == _STAR) {
            keyShoot = 1;
            keyShootPressed = 1;
        }
    } else if (code == KY_UP) {
        /* 按键松开 */
        if (param0 == _UP || param0 == _2) keyLeft = 0;
        if (param0 == _DOWN || param0 == _8) keyRight = 0;
        if (param0 == _RIGHT || param0 == _6) keyUp = 0;
        if (param0 == _LEFT || param0 == _4) keyDown = 0;
        if (param0 == _SELECT || param0 == _5) keyJump = 0;
        if (param0 == _0 || param0 == _STAR) keyShoot = 0;

        /* 右软键返回 */
        if (param0 == _SRIGHT) {
            switchWindow(0, 0);
        }
    }
}

/* 游戏结束事件处理 */
void gameOverEvent(int32 code, int32 param0, int32 param1) {
    if (code == KY_UP) {
        if (param0 == _SLEFT || param0 == _SELECT) {
            /* 重新开始 */
            currentWindow = WINDOW_GAME;
            initGame();
        } else if (param0 == _SRIGHT) {
            /* 退出 */
            mrc_exit();
        }
    }
}

/* 定时器回调 */
void timer_run(int32 id) {
    if (currentWindow == WINDOW_GAME) {
        if (gameState.running) {
            updateGame();
        }

        if (gameState.gameOver) {
            currentWindow = WINDOW_GAMEOVER;
            drawGameOver();
        } else {
            drawGame();
        }
    }
}

/* ==============================
 * MRC生命周期函数
 * ============================== */

/* 初始化 */
int32 mrc_init(void) {
    mrc_printf("[INIT] Metal Slug (Horizontal) Starting...\n");
    mpc_init();
    /* 初始化字体 */
    uc3_init();

    /* 初始化随机数种子 */
    mrc_sand(mrc_getUptime());

    /* 启动游戏 */
    currentWindow = WINDOW_GAME;
    initGame();
    drawGame();

    /* 创建定时器 */
    timer_cd = mrc_timerCreate();
    mrc_timerStart(timer_cd, TIMER_INTERVAL, 0, timer_run, 1);

    mrc_printf("[INIT] Done!\n");
    return 0;
}

/* 事件处理 */
int32 mrc_event(int32 code, int32 param0, int32 param1) {
    if (currentWindow == WINDOW_GAME) {
        gameEvent(code, param0, param1);
    } else if (currentWindow == WINDOW_GAMEOVER) {
        gameOverEvent(code, param0, param1);
    }
    return 0;
}

/* 暂停 */
int32 mrc_pause(void) {
    gameState.running = 0;
    return 0;
}

/* 恢复 */
int32 mrc_resume(void) {
    gameState.running = 1;
    return 0;
}

/* 退出 */
int32 mrc_exitApp(void) {
    /* 停止定时器 */
    mrc_timerStop(timer_cd);
    mrc_timerDelete(timer_cd);

    /* 释放字体 */
    uc3_free();

    return 0;
}


int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
  mrc_printf("mrc_extRecvAppEvent");
  return 0;
}
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3,
                            int32 p4, int32 p5)
{
  mrc_printf("mrc_extRecvAppEventEx");
  return 0;
}
