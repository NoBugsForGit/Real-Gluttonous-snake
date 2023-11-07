// 生成模块没有设置特殊实体的消失时间

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#define int_1 char
#define bool char
#define true 1
#define false 0

#define ID_NUM 16
#define SIZE_X 50
#define SIZE_Y 23

#define ENPTY_DIC 4
#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3
void gotoxy(int x, int y) // 定位光标位置到指定坐标
{
    HANDLE hOut;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(hOut, pos);
}

void clearScreen()
{
    gotoxy(0, 0);
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(console, ' ', screen.dwSize.X * screen.dwSize.Y, screen.dwCursorPosition, &written);
    FillConsoleOutputAttribute(console, screen.wAttributes, screen.dwSize.X * screen.dwSize.Y, screen.dwCursorPosition, &written);
    SetConsoleCursorPosition(console, screen.dwCursorPosition);
}

#define CL clearScreen()

struct Snake
{
    unsigned short x, y;
};

char *id_dict[ID_NUM] = {
    "⚫", // 0:空实体
    "🟥", // 1:墙
    "🟢", // 2:蛇头
    "🟩", // 3:蛇身
    "🟨", // 4:障碍物
    "🍎", // 5:1分
    "🥥", // 6:2分
    "🍈", // 7:4分
    "🈹", // 8:减去10长度，并减去1分
    "❤️",  // 9:增加1生命。死亡后复活
    "🚨", // 10:8个游戏刻内不生成任何实体。生成64游戏刻后消失
    "🚆", // 11:允许撞碎一次障碍物，并得2分.只能储存一次
    "🈳", // 12:直接重置游戏，但保留一半分数。生成32游戏刻后消失
    "💊", // 13:碰触蛇身不死亡，改为清除之，并根据减少的长度，减少1/2的分数.只能储存一次
    "🆙", // 14:分数翻倍
    "☢️"   // 15:结束游戏，清除记录，然后让你的计算机在10秒内关机。生成16游戏刻后消失
};

int_1 entity_limit[ID_NUM + 1] = {
    -1, // 0
    -1, // 1
    -1, // 2
    -1, // 3
    40, // 4
    40, // 5
    30, // 6
    20, // 7
    5,  // 8
    5,  // 9
    1,  // 10
    5,  // 11
    1,  // 12
    5,  // 13
    2,  // 14
    1,  // 15
    155};
int_1 map[SIZE_X][SIZE_Y];
struct Snake snake[SIZE_X * SIZE_Y];
char name[200];
int speed;
int score = 0;
int snake_length = 0;
int_1 direction = ENPTY_DIC;
int life = 1;
int id10_use_time = 0;
int id10_time = 0;
unsigned short id10_place[2];
bool id11 = false;
int id12_time = 0;
unsigned short id12_place[2];
bool id13 = false;
int id15_time = 0;
unsigned short id15_place[2];
int game_clock = 0;

void print_map()
{
    char chars[SIZE_X * 5];
    for (int i = 0; i < SIZE_Y; i++)
    {
        strcpy(chars, "");
        for (int j = 0; j < SIZE_X; j++)
        {
            strcat(chars, id_dict[map[j][i]]);
        }
        puts(chars);
    }
    printf("Player:%s\nscore:%d\t❤️x%d\t", name, score, life);
    if (id11)
    {
        printf("%s\t", id_dict[11]);
    }
    if (id13)
    {
        printf("%s\t", id_dict[13]);
    }
    if (id10_use_time > 0)
    {
        printf("%s%d", id_dict[10], id10_use_time);
    }
    printf("\n按wasd或方向键来移动,按 [ 暂停，按 ] 退出");
}
void print_test()
{

    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++)
        {

            printf("%d", map[j][i]);
        }
        putchar('\n');
    }
}
void init()
{
    // 墙体和空白区域初始化
    for (int i = 1; i < SIZE_X - 1; i++)
    {
        for (int j = 1; j < SIZE_Y - 1; j++)
        {
            map[i][j] = 0;
        }
    }
    for (int i = 0; i < SIZE_X; i++)
    {
        map[i][0] = 1;
        map[i][SIZE_Y - 1] = 1;
    }
    for (int i = 1; i < SIZE_Y - 1; i++)
    {
        map[0][i] = 1;
        map[SIZE_X - 1][i] = 1;
    }

    // 蛇初始化
    srand(clock());
    unsigned int ra = abs(rand());
    snake[0].x = ra % (SIZE_X - 2) + 1;
    snake[0].y = ra % (SIZE_Y - 2) + 1;
    snake_length = 1;
    map[snake[0].x][snake[0].y] = 2;
}

bool judge(unsigned short x, unsigned short y)
{
    if ((map[x][y] == 0) && ((abs(x - snake[0].x)) > 2 && (abs(y - snake[0].y)) > 2))
        return true;
    else
        return false;
}

void generate()
{
    srand(clock());

    unsigned int ra = abs(rand());
    unsigned short x = ra % (SIZE_X - 2) + 1;
    unsigned short y = ra % (SIZE_Y - 2) + 1;
    if (id10_use_time > 0)
        return;
    if (!judge(x, y))
        return;
    unsigned short id = ra % 200;
    if (0 <= id && id < 30)
    {
    ID4:
        id = 4;
        if (entity_limit[id] == 0)
            goto ID5;
    }
    else if (30 <= id && id < 80)
    {
    ID5:
        id = 5;
        if (entity_limit[id] == 0)
            goto ID6;
    }
    else if (80 <= id && id < 110)
    {
    ID6:
        id = 6;
        if (entity_limit[id] == 0)
            goto ID7;
    }
    else if (110 <= id && id < 130)
    {
    ID7:
        id = 7;
        if (entity_limit[id] == 0)
            goto ID8;
    }
    else if (130 <= id && id < 145)
    {
    ID8:
        id = 8;
        if (entity_limit[id] == 0)
            goto ID9;
    }
    else if (145 <= id && id < 155)
    {
    ID9:
        id = 9;
        if (entity_limit[id] == 0)
            goto ID10;
    }
    else if (155 <= id && id < 163)
    {
    ID10:
        id = 10;
        if (entity_limit[id] == 0)
            goto ID11;

        id10_place[0] = x;
        id10_place[1] = y;
    }
    else if (163 <= id && id < 178)
    {
    ID11:
        id = 11;
        if (entity_limit[id] == 0)
            goto ID12;
    }
    else if (178 <= id && id < 182)
    {
    ID12:
        id = 12;
        if (entity_limit[id] == 0)
            goto ID13;

        id12_place[0] = x;
        id12_place[1] = y;
    }
    else if (182 <= id && id < 192)
    {
    ID13:
        id = 13;
        if (entity_limit[id] == 0)
            goto ID14;
    }
    else if (192 <= id && id < 196)
    {
    ID14:
        id = 14;
        if (entity_limit[id] == 0)
            goto ID15;
    }
    else if (196 <= id && id < 200)
    {
    ID15:
        id = 15;
        if (entity_limit[id] == 0)
            id = 0;

        id15_place[0] = x;
        id15_place[1] = y;
    }

    map[x][y] = id;
    if (id != 0)
        entity_limit[id]--;
}

int search_body(unsigned short x, unsigned short y)
{
    for (int i = 0; i < SIZE_X * SIZE_Y; i++)
    {
        if (x == snake[i].x && y == snake[i].y)
        {
            return i;
        }
    }
    return -1;
}

void gen_snakebody(int n)
{
    if (snake_length == 1)
    {
        snake[snake_length].x = snake[0].x + 1;
        snake[snake_length].y = snake[0].y;
        snake_length++;
        for (int i = 1; i < n; i++)
        {
            snake[snake_length].x = snake[snake_length - 1].x * 2 - snake[snake_length - 2].x;
            snake[snake_length].y = snake[snake_length - 1].y * 2 - snake[snake_length - 2].y;
            snake_length++;
        }
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            snake[snake_length].x = snake[snake_length - 1].x * 2 - snake[snake_length - 2].x;
            snake[snake_length].y = snake[snake_length - 1].y * 2 - snake[snake_length - 2].y;
            snake_length++;
        }
    }
}
bool interactive(int_1 id, unsigned short x, unsigned short y)
// 返回值代表是否有改变方向的行为
{
    switch (id)
    {
    case 1:
        if (--life != 0)
        {
            direction = ENPTY_DIC;
            return true;
        }
        break;
    case 3:
        if (!id13)
        {
            if (--life != 0)
            {
                direction = ENPTY_DIC;
                return true;
            }
        }
        else
        {
            id13 = false;
            int index = search_body(x, y);
            score -= ((snake_length - index) / 2);
            snake_length = index;
        }
        break;
    case 4:
        if (!id11)
        {
            if (--life != 0)
            {
                direction = ENPTY_DIC;
                return true;
            }
        }
        else
        {
            id11 = false;
            map[x][y] = 2;
            score += 2;
        }
        break;
    case 5:
        score += 1;
        gen_snakebody(1);
        break;
    case 6:
        score += 2;
        gen_snakebody(2);
        break;
    case 7:
        score += 4;
        gen_snakebody(4);
        break;
    case 8:
        if (snake_length > 10)
        {
            snake_length -= 10;
        }
        else
        {
            snake_length = 1;
        }
        score -= 1;
        break;
    case 9:
        life++;
        break;
    case 10:
        id10_use_time = 8;
        break;
    case 11:
        id11 = true;
        break;
    case 12:
        score /= 2;
        snake_length = 1;
        init();
        return true;
        break;
    case 13:
        id13 = true;
        break;
    case 14:
        score *= 2;
        break;
    case 15:
        CL;
        printf("Silly B!!!Your computer will shutdown in 5 seconds!\n");
        system("shutdown -s -t 5");
        Sleep(6000);
        printf("You find my Easter egg!Congratulations!\nScreenshot to get a price.");
        Sleep(6000);
        system("shutdown -s -t 1");
        break;
    default:
        break;
    }
    return false;
}

void move()
{
    struct Snake temp_r = snake[0];
    struct Snake temp_l;
    switch (direction)
    {
    case LEFT:
        if (interactive(map[snake[0].x - 1][snake[0].y], snake[0].x - 1, snake[0].y))
            return;
        snake[0].x--;

        for (int i = 1; i < snake_length; i++)
        {
            temp_l = snake[i];
            snake[i] = temp_r;
            temp_r = temp_l;
        }
        break;

    case RIGHT:
        if (interactive(map[snake[0].x + 1][snake[0].y], snake[0].x + 1, snake[0].y))
            return;

        snake[0].x++;

        for (int i = 1; i < snake_length; i++)
        {
            temp_l = snake[i];
            snake[i] = temp_r;
            temp_r = temp_l;
        }
        break;

    case UP:
        if (interactive(map[snake[0].x][snake[0].y - 1], snake[0].x, snake[0].y - 1))
            return;

        snake[0].y--;

        for (int i = 1; i < snake_length; i++)
        {
            temp_l = snake[i];
            snake[i] = temp_r;
            temp_r = temp_l;
        }

        break;

    case DOWN:
        if (interactive(map[snake[0].x][snake[0].y + 1], snake[0].x, snake[0].y + 1))
            return;

        snake[0].y++;

        for (int i = 1; i < snake_length; i++)
        {
            temp_l = snake[i];
            snake[i] = temp_r;
            temp_r = temp_l;
        }

        break;

    default:
        return;
    }
}

void body_generate()
{
    for (int i = 1; i < SIZE_X - 1; i++)
    {
        for (int j = 1; j < SIZE_Y - 1; j++)
        {
            if (map[i][j] == 2 || map[i][j] == 3)
                map[i][j] = 0;
        }
    }
    map[snake[0].x][snake[0].y] = 2;
    for (int i = 1; i < snake_length; i++)
    {
        map[snake[i].x][snake[i].y] = 3;
    }
}

void clock_count()
{
    game_clock++;
    if (id10_use_time > 0)
    {
        id10_use_time--;
    }
    if (id10_time > 0)
    {
        if (--id10_time == 0)
            map[id10_place[0]][id10_place[1]] = 0;
    }

    if (id12_time > 0)
    {
        if (--id12_time == 0)
            map[id12_place[0]][id12_place[1]] = 0;
    }
    if (id15_time > 0)
    {
        if (--id15_time == 0)
            map[id15_place[0]][id15_place[1]] = 0;
    }
}

void start_game()
{
    int sp;
    printf("Please input your name:");
    scanf("%199s", name);
    CL;
    printf("Hello,%s!\nPlease choose your level(1~6):", name);
    scanf("%d", &sp);
    switch (sp)
    {
    case 1:
        speed = 1000;
        break;
    case 2:
        speed = 700;
        break;
    case 3:
        speed = 500;
        break;
    case 4:
        speed = 300;
        break;
    case 5:
        speed = 150;
        break;
    case 6:
        speed = 50;
        break;
    case -1:
        speed = 20;
        break;
    default:
        speed = 150;
        break;
    }
    CL;
}

void get_input()
{
    char ch = 0;
    if (kbhit())
        ch = getch();
    if (ch == -32)
        ch = getch();
    switch (ch)
    {
    case 97:
    case 52:
    case 75:
        direction = LEFT;
        break;
    case 100:
    case 56:
    case 77:
        direction = RIGHT;
        break;
    case 119:
    case 50:
    case 72:
        direction = UP;
        break;
    case 115:
    case 54:
    case 80:
        direction = DOWN;
        break;
    default:
        break;
    }
}