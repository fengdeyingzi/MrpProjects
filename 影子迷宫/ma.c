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
    bool passable; // С���Ƿ���ͨ��
} Lattice;

typedef struct {
    int x; // ������
    int y; // ������
} Ball;

typedef struct {
    Lattice maze[MAX_ROW + 2][MAX_COL + 2];
    Ball ball;
    Point entrance;
    Point exit;
    int rowNumber;
    int colNumber;
} Maze;

// ��������
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
        if (_kbhit()) { // ����Ƿ��а���������
            char command = _getch(); // ��ȡ���µļ�ֵ
            switch (command) {
                case 72: // Up (��ͷ��)
                    moveBall(&maze, 0);
                    break;
                case 80: // Down (��ͷ��)
                    moveBall(&maze, 1);
                    break;
                case 75: // Left (��ͷ��)
                    moveBall(&maze, 2);
                    break;
                case 77: // Right (��ͷ��)
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

// ��ʼ���Թ�
void initializeMaze(Maze* maze) {
    maze->rowNumber = MAX_ROW;
    maze->colNumber = MAX_COL;
    maze->entrance.x = 0; // ���
    maze->entrance.y = 1;
    maze->exit.x = maze->colNumber + 1; // ����
    maze->exit.y = maze->rowNumber;

    // �������и���Ϊ����ͨ�У�ǽ��
    for (int i = 0; i < maze->rowNumber + 2; i++) {
        for (int j = 0; j < maze->colNumber + 2; j++) {
            maze->maze[i][j].passable = false;
        }
    }

    // ����ںͳ���
    maze->maze[maze->entrance.y][maze->entrance.x].passable = true;
    maze->maze[maze->exit.y][maze->exit.x].passable = true;

    // ����С�����ʼλ��Ϊ���
    maze->ball.x = maze->entrance.x;
    maze->ball.y = maze->entrance.y;
}

// ��ӡ�Թ�
void printMaze(Maze* maze) {
    system("cls"); // ����
    for (int i = 0; i < maze->rowNumber + 2; i++) {
        for (int j = 0; j < maze->colNumber + 2; j++) {
            if (j == maze->ball.x && i == maze->ball.y) {
                printf("o "); // С��
            } else if (i == maze->entrance.y && j == maze->entrance.x) {
                printf("s "); // ���
            } else if (i == maze->exit.y && j == maze->exit.x) {
                printf("e "); // ����
            } else if (maze->maze[i][j].passable) {
                printf("  "); // ��ͨ���Ŀռ�
            } else {
                printf("* "); // ǽ��
            }
        }
        printf("*\n");
    }
    printf("Ball position: (%d, %d)\n", maze->ball.x, maze->ball.y);
}

// ����Ƿ����
bool isOutOfBorder(Point* p, Maze* maze) {
    return (p->x < 0 || p->x > maze->colNumber + 1 || 
            p->y < 0 || p->y > maze->rowNumber + 1);
}

// �ƶ�С��
void moveBall(Maze* maze, int direction) {
    Ball newPosition = maze->ball;

    switch (direction) {
        case 0: // ��
            newPosition.y -= 1;
            break;
        case 1: // ��
            newPosition.y += 1;
            break;
        case 2: // ��
            newPosition.x -= 1;
            break;
        case 3: // ��
            newPosition.x += 1;
            break;
    }

    if (!isOutOfBorder(&newPosition, maze) && maze->maze[newPosition.y][newPosition.x].passable) {
        maze->ball = newPosition;
    } else {
        printf("Can't move there!\n");
    }
}

// ����Ƿ�Ӯ����Ϸ
bool isWin(Maze* maze) {
    return (maze->ball.x == maze->exit.x && maze->ball.y == maze->exit.y);
}

// ��Ϸ������Ϣ
void gameOverMessage(Maze* maze) {
    printf("Congratulations on getting out of the maze!\n");
}

// ʹ�� DFS �㷨�����Թ�
void createMaze(Maze* maze) {
    srand(time(NULL));
    Point stack[MAX_ROW * MAX_COL]; // ���ڴ洢·��
    int top = -1;

    // ��һ�������㿪ʼ
    Point start = {2 * (rand() % (maze->colNumber / 2)) + 1, 2 * (rand() % (maze->rowNumber / 2)) + 1};
    maze->maze[start.y][start.x].passable = true;
    stack[++top] = start;

    while (top >= 0) {
        Point currentPoint = stack[top]; // ��ǰ��
        Point nextPoint;
        bool found = false;

        // ���ѡ��δ���ʵ����ڵ�Ԫ
        int directions[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} }; // �ϡ��¡�����
        for (int i = 0; i < 4; i++) {
            int randIndex = rand() % 4;
            Point direction = { directions[randIndex][0], directions[randIndex][1] };

            nextPoint.x = currentPoint.x + direction.x * 2;
            nextPoint.y = currentPoint.y + direction.y * 2;

            if (!isOutOfBorder(&nextPoint, maze) && !maze->maze[nextPoint.y][nextPoint.x].passable) {
                maze->maze[currentPoint.y + direction.y][currentPoint.x + direction.x].passable = true; // ��ǽ
                maze->maze[nextPoint.y][nextPoint.x].passable = true; // ����һ����Ԫ
                stack[++top] = nextPoint; // ѹ��ջ��
                found = true;
                break;
            }
        }

        if (!found) {
            top--; // ���û���ҵ����ߵĵ�Ԫ���˻���һ����Ԫ
        }
    }
}