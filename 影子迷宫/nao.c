#include "mrc_base.h"
#include "uc3_font.h"
#include "mrc_graphics.h"
#include "mpc.h"
#include "string_buffer.h"

// 定义颜色常量
#define COLOR_GREEN 0x8000FF00
#define COLOR_BLUE 0x800000FF
#define COLOR_RED 0x80FF0000
#define COLOR_PURPLE 0x80ff00ff
#define COLOR_YELLOW 0x80FFFF00
#define COLOR_ORANGE 0x80FF9500

// 定义图形类型
enum
{
    SHAPE_SQUARE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
};

// 定义颜色类型
enum
{
    _NONE_COLOR,
    _GREEN,
    _BLUE,
    _RED,
    _PURPLE,
    _YELLOW,
    _ORANGE
};

// 定义游戏状态
enum
{
    STATE_SHOW_GRAPHICS,
    STATE_SHOW_INPUT,
    STATE_SHOW_RESULT
};

// 定义图形结构体
typedef struct
{
    int shape;
    int x;
    int y;
    //随机坐标
    int move_x;
    int move_y;
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
int32 state_count;

// 随机生成颜色
uint32 random_color()
{
    int color_type = rand() % 6;
    // switch (color_type) {
    //     case _GREEN: return COLOR_GREEN;
    //     case _BLUE: return COLOR_BLUE;
    //     case _RED: return COLOR_RED;
    //     case _PURPLE: return COLOR_PURPLE;
    //     case _YELLOW: return COLOR_YELLOW;
    //     case _ORANGE: return COLOR_ORANGE;
    // }
    return color_type + 1;
}

void uc3_drawRectText( char *text, int x, int y, uint8 r, uint8 g, uint8 b, int is_unicode){
    //绘制黑色边框
    uc3_drawText(text, x - 1, y, 0, 0, 0, is_unicode);
    uc3_drawText(text, x + 1, y, 0, 0, 0, is_unicode);
    uc3_drawText(text, x, y - 1, 0, 0, 0, is_unicode);
    uc3_drawText(text, x, y + 1, 0, 0, 0, is_unicode);
    // 绘制白色文字
    uc3_drawText(text, x, y, r, g, b, is_unicode);
}

uint32 getColor(int32 color_type)
{
    switch (color_type)
    {
    case _GREEN:
        return COLOR_GREEN;
    case _BLUE:
        return COLOR_BLUE;
    case _RED:
        return COLOR_RED;
    case _PURPLE:
        return COLOR_PURPLE;
    case _YELLOW:
        return COLOR_YELLOW;
    case _ORANGE:
        return COLOR_ORANGE;
    }
    return 0xff909090;
}

// 随机生成图形
int random_shape()
{
    return rand() % 3;
}

// 绘制图形
void draw_shape(ShapeInfo shape)
{
    switch (shape.shape)
    {
    case SHAPE_SQUARE:
        gl_drawRect(shape.x - 11 + shape.move_x, shape.y - 11+shape.move_y, 22, 22, shape.color);
        gl_drawHollowRect(shape.x - 11 + shape.move_x, shape.y - 11 + shape.move_y, 22, 22, 0xff202020);
        break;
    case SHAPE_CIRCLE:
        gl_drawCir(shape.x+shape.move_x, shape.y+shape.move_y, 10, shape.color);
        gl_drawHollowCir(shape.x+shape.move_x, shape.y+shape.move_y, 10, 0xff202020);
        break;
    case SHAPE_TRIANGLE:
        gl_drawTriangle(shape.x - 10 + shape.move_x, shape.y + 10+ shape.move_y, shape.x + 10+ shape.move_x, shape.y + 10+ shape.move_y, shape.x+ shape.move_x, shape.y - 10+ shape.move_y, shape.color);
        gl_drawHollowTriangle(shape.x - 10+ shape.move_x, shape.y + 10+ shape.move_y, shape.x + 10+ shape.move_x, shape.y + 10+ shape.move_y, shape.x+ shape.move_x, shape.y - 10+ shape.move_y, 0xff202020);
        break;
    default:
        uc3_drawText("未知图形", shape.x, shape.y, 20, 20, 20, 0);
    }
    /*
    switch (shape.color_type)
    {
    case _BLUE:
        uc3_drawRectText("蓝色", shape.x, shape.y, 200, 200, 200, 0);
        break;
    case _GREEN:
        uc3_drawRectText("绿色", shape.x, shape.y, 200, 200, 200, 0);
        break;
    case _RED:
        uc3_drawRectText("红色", shape.x, shape.y, 200, 200, 200, 0);
        break;
    case _PURPLE:
        uc3_drawRectText("紫色", shape.x, shape.y, 200, 200, 200, 0);
        break;
    case _YELLOW:
        uc3_drawRectText("黄色", shape.x, shape.y, 200, 200, 200, 0);
        break;
    case _ORANGE:
        uc3_drawRectText("橙色", shape.x, shape.y, 200, 200, 200, 0);
        break;

    default:
        break;
    }
    */
    
}

// 判断图形中是否存在指定颜色
int get_color_count(int current_color)
{
    int i;
    int count = 0;
    for (i = 0; i < 10; i++)
    {
        if (shapes[i].color_type == current_color)
        {
            count++;
        }
    }
    return count;
}

// 判断图形中是否存在指定形状

int get_shape_count(int current_shape)
{
    int i;
    int count = 0;
    for (i = 0; i < 10; i++)
    {
        if (shapes[i].shape == current_shape)
        {
            count++;
        }
    }
    return count;
}

// 生成问题
void generate_question()
{
    int i;
    int count1;
    int count2;
    char *color_names[7];
    char *shape_names[3];
    color_names[0] = "未知色";
    color_names[1] = "绿色";
    color_names[2] = "蓝色";
    color_names[3] = "红色";
    color_names[4] = "紫色";
    color_names[5] = "黄色";
    color_names[6] = "橙色";

    shape_names[0] = "正方形";
    shape_names[1] = "圆形";
    shape_names[2] = "三角形";

    correct_count = 0;
    count1 = 0;
    count2 = 0;
    while (count1 == 0 || count2 == 0)
    {
        if (count1 == 0)
        {
            current_color = rand() % 6+1;
            count1 = get_color_count(current_color);
        }

        if (count2 == 0)
        {
            current_shape = random_shape();
            count2 = get_shape_count(current_shape);
        }
    }

    if (rand() % 2 == 0)
    {

        correct_count = count1;
        // 询问颜色
        sprintf(question, "请输入%s图形的数量", color_names[current_color]);
    }
    else
    {
        correct_count = count2;
        // 询问图形
        sprintf(question, "请输入%s的数量", shape_names[current_shape]);
    }
    mrc_printf("........... count %d\n", correct_count);
}

// 判断图形碰撞
int is_collect(ShapeInfo shape, int x, int y)
{
    if (x > shape.x - 10 && y > shape.y - 10 && x < shape.x + 10 && y < shape.y + 10)
    {
        return 1;
    }
    return 0;
}

// 初始化图形信息
void init_shapes()
{
    int i, j;
    int tx, ty;
    for (i = 0; i < 10; i++)
    {
        shapes[i].shape = random_shape();

    // 判断是否和之前的图形碰撞
    CHECK_COLLECT:
        tx = (rand() % (SCRW - 20)) + 15;
        ty = (rand() % (SCRH - 20)) + 15;
        for (j = 0; j < i; j++)
        {
            if (is_collect(shapes[j], tx, ty))
            {
                goto CHECK_COLLECT;
            }
        }
        shapes[i].x = (rand() % (SCRW - 20)) + 15;
        shapes[i].y = (rand() % (SCRH - 20)) + 15;
        shapes[i].color_type = random_color();
        shapes[i].color = getColor(shapes[i].color_type);
        shapes[i].angle = (float)(rand() % 360);
        shapes[i].move_x = 0;
        shapes[i].move_y = 0;
    }
}

//让形状随机移动

void move_shapes()
{
    int i;
    int mx,my;
    
    for (i = 0; i < 10; i++)
    {
        mx = (rand() % 3)-1;
    my = (rand() % 3)-1;
        shapes[i].move_x += mx;
        shapes[i].move_y += my;

        if(shapes[i].move_x > 10){
            shapes[i].move_x -=2;
        }
        if(shapes[i].move_x < -10){
            shapes[i].move_x +=2;
        }
        if(shapes[i].move_y > 10){
            shapes[i].move_y -=2;
        }
        if(shapes[i].move_y < -10){
            shapes[i].move_y +=2;
        }
    }
}

// 定时器回调函数
void timer_callback(int32 data)
{
    int i, index;
    int32 start_time;
    
    char temp[50];
    mrc_clearScreen(250, 250, 250); // 清屏
    move_shapes();
    
    start_time = getuptime();
    if (game_state == STATE_SHOW_GRAPHICS || game_state == STATE_SHOW_INPUT)
    {
        // 绘制所有图形
        for (i = 0; i < 10; i++)
        {
            draw_shape(shapes[i]);
        }
        


        // 显示问题
        uc3_drawText(question, 10, 10, 20, 20, 20, 0);
        mrc_sprintf(temp, "%d", correct_count);
        // uc3_drawText(temp, SCRW - 30, 10, 20, 20, 20, 0);
        if (game_state == STATE_SHOW_INPUT)
        {
            // 显示输入的数字
            uc3_drawRectText(input_text, SCRW/2-16, SCRH/2-16, 240, 240, 240, 0);
            if(state_count == 0){
                state_count = 3;
            }else{
                state_count--;
                if (state_count == 0)
                {
                    game_state = STATE_SHOW_RESULT;
                    return;
                }
            }
        }
    }

    else if (game_state == STATE_SHOW_RESULT)
    {
        if (state_count == 0)
        {
            state_count = 3;
        }
        else
        {
            state_count--;
            if (state_count == 0)
            {
                game_state = STATE_SHOW_GRAPHICS;
                init_shapes();
                generate_question();
                mrc_refreshScreen(0, 0, SCRW, SCRH); // 刷新屏幕
                return;
            }
        }
        mrc_sprintf(temp, "正确答案:%d", correct_count);
        // 显示结果
        if (user_input == correct_count)
        {
            uc3_drawText("正确", 10, 30, 0, 255, 0, 0);
        }
        else
        {
            uc3_drawText("错误", 10, 30, 255, 0, 0, 0);
            uc3_drawText(temp, 10, 50, 20, 20, 20, 0);
        }
    }
    
    mrc_sprintf(temp, "%d ms", getuptime()-start_time);
    uc3_drawText(temp, SCRW-10-8*mrc_strlen(temp), 8, 0, 0, 0, 0);
    mrc_refreshScreen(0, 0, SCRW, SCRH); // 刷新屏幕
}

// 初始化函数
int32 mrc_init(void)
{
    mpc_init();
    uc3_init();
    state_count = 0;
    sand(getuptime()); // 初始化随机数种子

    timer = mrc_timerCreate();
    if (timer != 0)
    {
        mrc_timerStart(timer, 300, 0, timer_callback, 1); // 每秒更新一次
    }

    init_shapes();
    generate_question();
    game_state = STATE_SHOW_GRAPHICS;
    input_text[0] = '\0'; // 初始化输入文本
    return 0;
}

// 事件处理函数
int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    if (code == KY_DOWN)
    {
        switch (param0)
        {
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
            if (game_state == STATE_SHOW_GRAPHICS)
            {
                game_state = STATE_SHOW_INPUT;
                input_text[0] = param0 + '0';
                input_text[1] = '\0';
                user_input = atoi(input_text);
            }
            else if (game_state == STATE_SHOW_INPUT)
            {
                int len = strlen(input_text);
                if (len < 9)
                {
                    input_text[len] = param0 + '0';
                    input_text[len + 1] = '\0';
                    user_input = atoi(input_text);
                }
            }
            break;
        case _SELECT: // 确认键
            if (game_state == STATE_SHOW_INPUT)
            {
                user_input = atoi(input_text);
                game_state = STATE_SHOW_RESULT;
            }
            else if (game_state == STATE_SHOW_RESULT)
            {
                init_shapes();
                generate_question();
                game_state = STATE_SHOW_GRAPHICS;
                input_text[0] = '\0'; // 清空输入文本
            }
            break;
            case _SRIGHT:
            mrc_exit();
        }
    }
    return 0;
}

// 退出函数
int32 mrc_exitApp()
{
    if (timer != 0)
    {
        mrc_timerDelete(timer);
    }
    uc3_free();
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

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5)
{
    return 0;
}

void switchWindow(int windex, int leve) {}