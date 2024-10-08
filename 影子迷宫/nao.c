#include "mrc_base.h"
#include "uc3_font.h"
#include "mrc_graphics.h"
#include "mpc.h"

// 定义颜色常量
#define COLOR_GREEN   0x8000FF00
#define COLOR_BLUE    0x800000FF
#define COLOR_RED     0x80FF0000
#define COLOR_PURPLE  0x80800080
#define COLOR_YELLOW  0x80FFFF00
#define COLOR_ORANGE  0x80FFA500

// 定义图形类型
enum {
    SHAPE_SQUARE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
};

// 定义颜色类型
enum {
    _GREEN,
    _BLUE,
    _RED,
    _PURPLE,
    _YELLOW,
    _ORANGE
};

// 定义游戏状态
enum {
    STATE_SHOW_GRAPHICS,
    STATE_SHOW_INPUT,
    STATE_SHOW_RESULT
};

// 定义图形结构体
typedef struct {
    int shape;
    int x;
    int y;
    uint32 color;
    int32 color_type;
    float angle;
} ShapeInfo;

// 全局变量
int32 timer;
int32 correct_count;
int32 user_input;
int32 current_shape;
int32 current_color;
int32 game_state;
char question[64];
char input_text[10];
ShapeInfo shapes[10];

// 随机生成颜色
uint32 random_color() {
    int color_type = rand() % 6;
    // switch (color_type) {
    //     case _GREEN: return COLOR_GREEN;
    //     case _BLUE: return COLOR_BLUE;
    //     case _RED: return COLOR_RED;
    //     case _PURPLE: return COLOR_PURPLE;
    //     case _YELLOW: return COLOR_YELLOW;
    //     case _ORANGE: return COLOR_ORANGE;
    // }
    return color_type;
}

uint32 getColor(int32 color_type){
    switch (color_type) {
        case _GREEN: return COLOR_GREEN;
        case _BLUE: return COLOR_BLUE;
        case _RED: return COLOR_RED;
        case _PURPLE: return COLOR_PURPLE;
        case _YELLOW: return COLOR_YELLOW;
        case _ORANGE: return COLOR_ORANGE;
    }
}

// 随机生成图形
int random_shape() {
    return rand() % 3;
}

// 绘制图形
void draw_shape(ShapeInfo shape) {
    switch (shape.shape) {
        case SHAPE_SQUARE:
            gl_drawRotatedRect(shape.x, shape.y, 20, 20, 0, 0, shape.angle, shape.color);
            break;
        case SHAPE_CIRCLE:
            gl_drawCir(shape.x, shape.y, 10, shape.color);
            break;
        case SHAPE_TRIANGLE:
            gl_drawTriangle(shape.x-10, shape.y+10, shape.x+10, shape.y+10, shape.x, shape.y-10, shape.color);
            break;
    }
}

// 生成问题
void generate_question() {
    int i;
     char* color_names[6];
     char* shape_names[3] ;
     color_names[0] = "绿色";
     color_names[1] = "蓝色";
     color_names[2] = "红色";
     color_names[3] = "紫色";
     color_names[4] = "黄色";
     color_names[5] = "橙色";
     shape_names[0] = "正方形";
     shape_names[1] = "圆形";
     shape_names[2] = "三角形";
    current_shape = random_shape();
    current_color = rand() % 6;
    correct_count = rand() % 5 + 1; // 1到5个图形

    
correct_count = 0;
    if (rand() % 2 == 0) {
        for(i=0;i<10;i++){
            if(shapes[i].color_type == current_color){
                correct_count++;
            }
        }
        
        // 询问颜色
        sprintf(question, "请输入%s色图形的数量", color_names[current_color]);
    } else {
        for(i=0;i<10;i++){
            if(shapes[i].shape == current_shape){
                correct_count++;
            }
        }
        // 询问图形
        sprintf(question, "请输入%s形状图形的数量", shape_names[current_shape]);
    }
}

// 初始化图形信息
void init_shapes() {
    int i;
    for ( i = 0; i < 10; i++) {
        shapes[i].shape = random_shape();
        shapes[i].x = (rand() % (SCRW-20))+15;
        shapes[i].y = (rand() % (SCRH-20))+15;
        shapes[i].color_type = random_color();
        shapes[i].color = getColor(shapes[i].color_type);
        shapes[i].angle = (float)(rand() % 360);
    }
}

// 定时器回调函数
void timer_callback(int32 data) {
    int i, index;
    char temp[50];
    mrc_clearScreen(0, 0, 0); // 清屏

    if (game_state == STATE_SHOW_GRAPHICS) {
        // 绘制所有图形
        for ( i = 0; i < 10; i++) {
            draw_shape(shapes[i]);
        }

        // 绘制正确数量的图形
        for ( i = 0; i < correct_count; i++) {
            //  index = rand() % 10;
            // shapes[index].shape = current_shape;
            // shapes[index].color = random_color();
            draw_shape(shapes[i]);
        }

        // 显示问题
        uc3_drawText(question, 10, 10, 255, 255, 255, 0);
    } else if (game_state == STATE_SHOW_INPUT) {
        // 显示输入的数字
        uc3_drawText(input_text, 10, 30, 255, 255, 255, 0);
    } else if (game_state == STATE_SHOW_RESULT) {
        mrc_sprintf(temp, "正确答案:%d", correct_count);
        // 显示结果
        if (user_input == correct_count) {
            uc3_drawText("正确", 10, 30, 0, 255, 0, 0);

        } else {
            uc3_drawText("错误", 10, 30, 255, 0, 0, 0);
            uc3_drawText(temp, 10, 50, 255, 255, 255, 0);
        }
    }

    mrc_refreshScreen(0, 0, SCRW, SCRH); // 刷新屏幕
}

// 初始化函数
int32 mrc_init(void) {
    mpc_init();
    uc3_init();
    sand(getuptime()); // 初始化随机数种子

    timer = mrc_timerCreate();
    if (timer != 0) {
        mrc_timerStart(timer, 1000, 0, timer_callback, 1); // 每秒更新一次
    }

    init_shapes();
    generate_question();
    game_state = STATE_SHOW_GRAPHICS;
    input_text[0] = '\0'; // 初始化输入文本
    return 0;
}

// 事件处理函数
int32 mrc_event(int32 code, int32 param0, int32 param1) {
    if (code == KY_DOWN) {
        switch (param0) {
            case _0:
            case _1:
            case _2:
            case _3:
            case _4:
            case _5:
            case _6:
            case _7:
            case _8:
            case _9:
                if (game_state == STATE_SHOW_GRAPHICS) {
                    game_state = STATE_SHOW_INPUT;
                    input_text[0] = param0 + '0';
                    input_text[1] = '\0';
                } else if (game_state == STATE_SHOW_INPUT) {
                    int len = strlen(input_text);
                    if (len < 9) {
                        input_text[len] = param0 + '0';
                        input_text[len + 1] = '\0';
                    }
                }
                break;
            case _SELECT: // 确认键
                if (game_state == STATE_SHOW_INPUT) {
                    user_input = atoi(input_text);
                    game_state = STATE_SHOW_RESULT;
                } else if (game_state == STATE_SHOW_RESULT) {
                    init_shapes();
                    generate_question();
                    game_state = STATE_SHOW_GRAPHICS;
                    input_text[0] = '\0'; // 清空输入文本
                }
                break;
        }
    }
    return 0;
}

// 退出函数
int32 mrc_exitApp() {
    if (timer != 0) {
        mrc_timerDelete(timer);
    }
    return 0;
}

int32 mrc_pause(void) {
    return 0;
}

int32 mrc_resume(void) {
    return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5) {
    return 0;
}

void switchWindow(int windex, int leve) {}