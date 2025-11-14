#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // for getch()
#include <string.h>
#include <time.h>

#define MAP_WIDTH 30
#define MAP_HEIGHT 30

typedef struct
{
    char items[10][20];
    int count;
} Inventory;
int playerX = 0, playerY = 0; // Player starts at (0, 0)
/*
1. `0` - ¿ÕµØ
2. `1` - Ç½
3. `e` - µÐÈË
4. `k` - Ô¿³×
5. `h` - Ñª°ü
6. `s` - Ä¾½£
7. `b` - ¹­¼ý
8. `p` - ´«ËÍÃÅ
9. `g` - ±¦Ïä
*/
char *loadMap(int maptype)
{
    switch (maptype)
    {
    case 1:
        return "0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0\n"
               "0,0,e,0,1,1,1,1,1,0,0,0,1,0,k,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,0,0\n"
               "0,0,0,0,0,1,0,0,0,0,0,0,1,0,h,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0\n"
               "1,1,1,1,0,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,0,0\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,s,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,e,e\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n";
    case 2:
        return "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1\n"
               "0,0,0,0,v,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1\n"
               "0,0,e,1,0,0,0,0,0,0,0,0,1,1,0,k,0,0,0,0,h,0,0,0,0,1,0,0,1,1,1,1,0\n"
               "0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0\n"
               "1,1,1,1,0,0,0,0,0,0,0,s,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    case 3:
        return "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "0,0,e,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0\n"
               "0,0,0,0,0,0,0,k,0,h,0,0,0,0,0,0,0,0,0,0,0,0,0,0,s,1,1,0,0,0,0,0,0\n"
               "1,1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1\n"
               "1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,e,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0\n"
               "0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n";
    case 4:
        return "1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,k,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,h,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,s,0,0,e,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1\n";
    case 5:
        return "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n"
               "0,0,0,0,e,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1\n"
               "0,0,0,0,0,k,0,0,0,0,0,0,0,0,0,0,h,s,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0\n"
               "1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1\n"
               "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    default:
        return NULL;
    }
}

void printMap(char map[MAP_HEIGHT][MAP_WIDTH])
{
    printf("map:\r\n");
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            if (playerX == j && playerY == i)
            {
                printf("ÎÒ");
            }
            else
            {
                switch (map[i][j])
                {
                case '0':
                    printf("  ");
                    break;
                case '1':
                    printf("¡ö");
                    break;
                case 'e':
                    printf("µÐ");
                    break;
                case 'k':
                    printf("Ô¿");
                    break;
                case 'h':
                    printf("Ñª");
                    break;
                case 's':
                    printf("½£");
                    break;
                case 'b':
                    printf("¹­");
                    break;
                default:
                    printf("%c ", map[i][j]);
                    break;
                }
            }
        }
        printf("\r\n");
    }
}

void collectItem(char item, Inventory *inv)
{
    switch (item)
    {
    case 'k':
        strcpy(inv->items[inv->count++], "Key");
        break;
    case 'h':
        strcpy(inv->items[inv->count++], "Health Pack");
        break;
    case 's':
        strcpy(inv->items[inv->count++], "Wooden Sword");
        break;
    case 'b':
        strcpy(inv->items[inv->count++], "Bow");
        break;
    }
}

void printInventory(Inventory inv)
{
    printf("Inventory:\n");
    for (int i = 0; i < inv.count; i++)
    {
        printf("%s\n", inv.items[i]);
    }
}

char *strtok2(char *str, char *c)
{
    char *ptr = str;
    while (*ptr)
    {
        if (ptr[0] == c[0])
        {
            return ptr + 1;
        }
        ptr++;
    }
    return NULL;
}

int main()
{
    srand(time(NULL)); // Seed for random number generation

    char map[MAP_HEIGHT][MAP_WIDTH];
    Inventory inventory = {.count = 0};

    // Load random map type between 1 and 5
    int mapType = (rand() % 5) + 1;
    char *mapData = loadMap(mapType);
    for (int ix = 0; ix < MAP_WIDTH; ix++)
    {
        for (int iy = 0; iy < MAP_HEIGHT; iy++)
        {
            map[iy][ix] = '0';
        }
    }
    printf("Map Type: %d\n", mapType);
    printf(mapData);
    // Initialize the map from loadMap
    char *line = strtok2(mapData, "\n");
    for (int i = 0; i < MAP_HEIGHT && line != NULL; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            map[i][j] = line[j * 2]; // Copying only the character
        }
        line = strtok2(line, "\n");
    }
    while (1)
    {
        system("cls"); // Clear the screen
        printMap(map);
        printf("Use WASD to move, Enter to collect item, M to view inventory.\n");

        // Read user input
        char ch = getch();

        // Control character movement
        if (ch == 'w' && playerX > 0)
            playerY--;
        else if (ch == 's' && playerX < MAP_HEIGHT - 1)
            playerY++;
        else if (ch == 'a' && playerY > 0)
            playerX--;
        else if (ch == 'd' && playerY < MAP_WIDTH - 1)
            playerX++;
        else if (ch == '\r')
        { // Enter key pressed
            char item = map[playerX][playerY];
            if (item == 'k' || item == 'h' || item == 's' || item == 'b')
            {
                collectItem(item, &inventory);
                map[playerX][playerY] = '0'; // Remove item from map after collection
            }
        }
        else if (ch == 'm')
        { // 'M' key pressed
            printInventory(inventory);
            getch(); // Wait for a key press to return
        }
        else if (ch == 'q')
        {
            break; // Quit the game
        }

        // Check for enemy encounter
        if (map[playerX][playerY] == 'e')
        {
            printf("An enemy is here! Game Over!\n");
            break;
        }
    }

    return 0;
}
