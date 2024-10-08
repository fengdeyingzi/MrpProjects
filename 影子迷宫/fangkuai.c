#include "mrc_base.h"
#include "mpc.h"
#include "mrc_graphics.h"
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define PADDLE_WIDTH 40
#define PADDLE_HEIGHT 10

// ��Ϸ״̬
typedef struct {
    int32 paddle_x;
    int32 paddle_y;
    int32 paddle_isleft;
    int32 paddle_isright;
    int32 ball_x;
    int32 ball_y;
    int32 ball_dx; // ���x�����ٶ�
    int32 ball_dy; // ���y�����ٶ�
    int32 bricks[BRICK_ROWS][BRICK_COLS]; // ש��״̬
    int32 score; // �÷�
    int32 is_running; // ��Ϸ�Ƿ�������
} GameState;

GameState state;

// ��������
void mrc_initGame(void);
void mrc_drawBricks(void);
void mrc_drawPaddle(void);
void mrc_drawBall(void);
void mrc_updateBall(void);
void mrc_checkCollision(int32 param0);
void mrc_gameLoop(int32 data);
int32 timer;
// ��Ϸ��ʼ��
int32 mrc_init(void) {
    mpc_init();

    mrc_clearScreen(0, 0, 0); // ����Ϊ��ɫ
    mrc_initGame();

    // ������ʱ��
     timer = mrc_timerCreate();
    mrc_timerStart(timer, 30, 0, mrc_gameLoop, 1); // ÿ30�������һ��

    return 0;
}

// ��ʼ����Ϸ״̬
void mrc_initGame(void) {
    int32 row, col;
    state.paddle_x = (SCRW - PADDLE_WIDTH) / 2;
    state.paddle_y = SCRH - PADDLE_HEIGHT - 10;
    state.ball_x = SCRW / 2;
    state.ball_y = SCRH - PADDLE_HEIGHT - 20;
    state.ball_dx = 1; // �����ƶ�
    state.ball_dy = -1; // �����ƶ�

    // ��ʼ��ש��
    for ( row = 0; row < BRICK_ROWS; row++) {
        for ( col = 0; col < BRICK_COLS; col++) {
            state.bricks[row][col] = 1; // 1��ʾ����ש��
        }
    }
    state.score = 0;
    state.is_running = 1;
}

// ����ש��
void mrc_drawBricks(void) {
    int32 row, col;
    for ( row = 0; row < BRICK_ROWS; row++) {
        for ( col = 0; col < BRICK_COLS; col++) {
            if (state.bricks[row][col]) {
                mrc_drawRect(col * (SCRW / BRICK_COLS),
                              row * 15,
                              SCRW / BRICK_COLS - 1,
                              10,
                              255, 0, 0); // ��ɫש��
            }
        }
    }
}

// ���Ƶ���
void mrc_drawPaddle(void) {
    mrc_drawRect(state.paddle_x, state.paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, 0, 255, 0); // ��ɫ����
}

// ������
void mrc_drawBall(void) {
    gl_drawCir(state.ball_x, state.ball_y, 4, 0xffffffff); // ��ɫ��
}

// �������λ��
void mrc_updateBall(void) {
    state.ball_x += state.ball_dx;
    state.ball_y += state.ball_dy;

    // �߽���ײ����
    if (state.ball_x <= 0 || state.ball_x >= SCRW - 1) {
        state.ball_dx = -state.ball_dx; // ˮƽ����
    }
    if (state.ball_y <= 0) {
        state.ball_dy = -state.ball_dy; // ��ֱ����
    }

    // ���뵲����ײ
    if (state.ball_y >= state.paddle_y && 
        state.ball_x >= state.paddle_x && 
        state.ball_x <= state.paddle_x + PADDLE_WIDTH) {
        state.ball_dy = -state.ball_dy; // ����
        state.ball_y = state.paddle_y - 1; // ȷ���򲻴�͸����
    }

    // ����ש����ײ
    mrc_checkCollision(0);

    // �������䵽��Ļ�ײ�����Ϸ����
    if (state.ball_y >= SCRH) {
        state.is_running = 0; // ��Ϸ����
    }
}

// �������ש�����ײ
void mrc_checkCollision(int32 param0) {
    int32 row, col, brick_x, brick_y;
    for ( row = 0; row < BRICK_ROWS; row++) {
        for ( col = 0; col < BRICK_COLS; col++) {
            if (state.bricks[row][col]) {
                 brick_x = col * (SCRW / BRICK_COLS);
                 brick_y = row * 15;

                if (state.ball_x >= brick_x && state.ball_x < brick_x + (SCRW / BRICK_COLS) &&
                    state.ball_y >= brick_y && state.ball_y < brick_y + 10) {
                    state.bricks[row][col] = 0; // �Ƴ�ש��
                    state.ball_dy = -state.ball_dy; // ����
                    state.score++; // ���ӷ���
                }
            }
        }
    }
}

// ��Ϸ��ѭ��
void mrc_gameLoop(int32 data) {
    if (!state.is_running) {
        mrc_clearScreen(0, 0, 0); // ����
        return;
    }

    mrc_clearScreen(0, 0, 0); // ����
    mrc_updateBall(); // ������λ��
    mrc_updateBall(); // ������λ��
    if (state.paddle_isleft && state.paddle_x > 0) {
            state.paddle_x -= 5; // �����ƶ�����
            if(state.paddle_x < 0) state.paddle_x = 0;
        } else if (state.paddle_isright && state.paddle_x < SCRW - PADDLE_WIDTH) {
            state.paddle_x += 5; // �����ƶ�����
            if(state.paddle_x >= SCRW-PADDLE_WIDTH) state.paddle_x = SCRW-PADDLE_WIDTH;
        }
    mrc_drawBricks(); // ����ש��
    mrc_drawPaddle(); // ���Ƶ���
    mrc_drawBall(); // ������

    mrc_refreshScreen(0, 0, SCRW, SCRH); // ˢ����Ļ
}

// �˳��¼�����
int32 mrc_exitApp() {
    // ��������ͷ��ڴ�ȣ���ǰ�޶�̬����
    return 0;
}

// �¼�����
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