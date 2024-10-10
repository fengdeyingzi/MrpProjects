#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>

#define MAX_ROW 10
#define MAX_COL 10

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    bool passable; // 小球是否能通过
} Lattice;

typedef struct {
    int x; // 横坐标
    int y; // 纵坐标
} Ball;

typedef struct {
    Lattice maze[MAX_ROW + 2][MAX_COL + 2];
    Ball ball;
    Point entrance;
    Point exit;
    int rowNumber;
    int colNumber;
} Maze;

// 函数声明
void initializeMaze(Maze* maze);
void printMaze(Maze* maze);
bool isOutOfBorder(Point* p, Maze* maze);
void moveBall(Maze* maze, int direction);
bool isWin(Maze* maze);
void gameOverMessage(Maze* maze);
void createMaze(Maze* maze);

int main() {
    Maze maze;
    initializeMaze(&maze);
    createMaze(&maze);
    printMaze(&maze);

    while (true) {
        if (_kbhit()) { // 检测是否有按键被按下
            char command = _getch(); // 获取按下的键值
            switch (command) {
                case 72: // Up (箭头上)
                    moveBall(&maze, 0);
                    break;
                case 80: // Down (箭头下)
                    moveBall(&maze, 1);
                    break;
                case 75: // Left (箭头左)
                    moveBall(&maze, 2);
                    break;
                case 77: // Right (箭头右)
                    moveBall(&maze, 3);
                    break;
                case 'q': // Quit game
                    exit(0);
                default:
                    continue;
            }
            printMaze(&maze);
            if (isWin(&maze)) {
                gameOverMessage(&maze);
                break;
            }
        }
    }

    return 0;
}

// 初始化迷宫
void initializeMaze(Maze* maze) {
    maze->rowNumber = MAX_ROW;
    maze->colNumber = MAX_COL;
    maze->entrance.x = 0; // 入口
    maze->entrance.y = 1;
    maze->exit.x = maze->colNumber + 1; // 出口
    maze->exit.y = maze->rowNumber;

    // 设置所有格子为不可通行（墙）
    for (int i = 0; i < maze->rowNumber + 2; i++) {
        for (int j = 0; j < maze->colNumber + 2; j++) {
            maze->maze[i][j].passable = false;
        }
    }

    // 打开入口和出口
    maze->maze[maze->entrance.y][maze->entrance.x].passable = true;
    maze->maze[maze->exit.y][maze->exit.x].passable = true;

    // 设置小球的起始位置为入口
    maze->ball.x = maze->entrance.x;
    maze->ball.y = maze->entrance.y;
}

// 打印迷宫
void printMaze(Maze* maze) {
    system("cls"); // 清屏
    for (int i = 0; i < maze->rowNumber + 2; i++) {
        for (int j = 0; j < maze->colNumber + 2; j++) {
            if (j == maze->ball.x && i == maze->ball.y) {
                printf("o "); // 小球
            } else if (i == maze->entrance.y && j == maze->entrance.x) {
                printf("s "); // 入口
            } else if (i == maze->exit.y && j == maze->exit.x) {
                printf("e "); // 出口
            } else if (maze->maze[i][j].passable) {
                printf("  "); // 可通过的空间
            } else {
                printf("* "); // 墙壁
            }
        }
        printf("*\n");
    }
    printf("Ball position: (%d, %d)\n", maze->ball.x, maze->ball.y);
}

// 检查是否出界
bool isOutOfBorder(Point* p, Maze* maze) {
    return (p->x < 0 || p->x > maze->colNumber + 1 || 
            p->y < 0 || p->y > maze->rowNumber + 1);
}

// 移动小球
void moveBall(Maze* maze, int direction) {
    Ball newPosition = maze->ball;

    switch (direction) {
        case 0: // 上
            newPosition.y -= 1;
            break;
        case 1: // 下
            newPosition.y += 1;
            break;
        case 2: // 左
            newPosition.x -= 1;
            break;
        case 3: // 右
            newPosition.x += 1;
            break;
    }

    if (!isOutOfBorder(&newPosition, maze) && maze->maze[newPosition.y][newPosition.x].passable) {
        maze->ball = newPosition;
    } else {
        printf("Can't move there!\n");
    }
}

// 检查是否赢得游戏
bool isWin(Maze* maze) {
    return (maze->ball.x == maze->exit.x && maze->ball.y == maze->exit.y);
}

// 游戏结束消息
void gameOverMessage(Maze* maze) {
    printf("Congratulations on getting out of the maze!\n");
}

// 使用 DFS 算法生成迷宫
void createMaze(Maze* maze) {
    srand(time(NULL));
    Point stack[MAX_ROW * MAX_COL]; // 用于存储路径
    int top = -1;

    // 从一个随机起点开始
    Point start = {2 * (rand() % (maze->colNumber / 2)) + 1, 2 * (rand() % (maze->rowNumber / 2)) + 1};
    maze->maze[start.y][start.x].passable = true;
    stack[++top] = start;

    while (top >= 0) {
        Point currentPoint = stack[top]; // 当前点
        Point nextPoint;
        bool found = false;

        // 随机选择未访问的相邻单元
        int directions[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} }; // 上、下、左、右
        for (int i = 0; i < 4; i++) {
            int randIndex = rand() % 4;
            Point direction = { directions[randIndex][0], directions[randIndex][1] };

            nextPoint.x = currentPoint.x + direction.x * 2;
            nextPoint.y = currentPoint.y + direction.y * 2;

            if (!isOutOfBorder(&nextPoint, maze) && !maze->maze[nextPoint.y][nextPoint.x].passable) {
                maze->maze[currentPoint.y + direction.y][currentPoint.x + direction.x].passable = true; // 打开墙
                maze->maze[nextPoint.y][nextPoint.x].passable = true; // 打开下一个单元
                stack[++top] = nextPoint; // 压入栈中
                found = true;
                break;
            }
        }

        if (!found) {
            top--; // 如果没有找到可走的单元，退回上一个单元
        }
    }
}