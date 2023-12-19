// 生成模块没有设置特殊实体的消失时间

#include "cmdgame.h"
#include "crypto.h"

#define ID_NUM 32
#define SIZE_X 50
#define SIZE_Y 23

struct Portal
{
    coor x[2], y[2];
    bool type; // 1为🔟
    bool exist;
};

struct Snake
{
    coor x, y;
};

char *id_dict[ID_NUM] = {
    "  ", // 0:空实体
    "🟥", // 1:墙
    "🟢", // 2:蛇头
    "🟩", // 3:蛇身
    "🟨", // 4:障碍物
    "🍎", // 5:1分
    "🥥", // 6:2分
    "🍈", // 7:4分
    "🈹", // 8:减去10长度，并减去1分
    "❤️",  // 9:增加1生命，至多叠加至5。超出上限的部分转化为1分
    "🚨", // 10:64个游戏刻内不生成任何实体。生成256游戏刻后消失
    "🚆", // 11:允许撞碎一次障碍物，并得2分.只能储存一次
    "🈳", // 12:直接重置游戏，但保留一半分数。生成32游戏刻后消失
    "💊", // 13:碰触蛇身不死亡，改为清除之，并根据减少的长度，减少3/4分数.只能储存一次
    "🆙", // 14:分数翻倍
    "☢️",  // 15:立即结束游戏,不保存记录，然后让你的计算机在5秒内关机。生成64游戏刻后消失
    "⚪", // 16
    "🩸", // 17
    "🩹", // 18
    "🧲", // 19
    "❓", // 20
    "💜", // 21
    "💖", // 22
    "🔟", // 23
    "♈", // 24
    "☦️",  // 25
    "⛎", // 26
    "♉", // 27
    "♊", // 28
    "♏", // 29
    "❄️",  // 30
    "⚡"  // 31
};

const int entity_limit_init[ID_NUM] = {
    100, // 0
    -1,  // 1
    -1,  // 2
    -1,  // 3
    40,  // 4
    40,  // 5
    30,  // 6
    20,  // 7
    5,   // 8
    5,   // 9
    1,   // 10
    5,   // 11
    1,   // 12
    5,   // 13
    2,   // 14
    1,   // 15
    5,   // 16
    10,  // 17
    1,   // 18
    2,   // 19
    5,   // 20
    9,   // 21
    1,   // 22
    1,   // 23[23,29]为传送门，按组生成，上限均为1组
    1,   // 24
    1,   // 25
    1,   // 26
    1,   // 27
    1,   // 28
    1,   // 29
    3,   // 30
    3    // 31

};
int entity_limit[ID_NUM];

const int gen_odd[ID_NUM] = {
    // 生成概率
    0,  // 0:储存随机数的生成范围。由计算得到。
    2,  // 1:储存不生成实体的概率的倒数
    0,  // 2
    0,  // 3:必须置0
    50, // 🟨4:障碍物
    50, // 🍎5:1分
    25, // 🥥6:2分
    15, // 🍈7:4分
    10, // 🈹8:减去10长度，并减去1分
    10, // ❤️9:增加1生命，至多叠加至5。超出上限的部分转化为1分
    8,  // 🚨10:64个游戏刻内不生成任何实体。生成256游戏刻后消失
    5,  // 🚆11:允许撞碎一次障碍物，并得2分.只能储存一次
    2,  // 🈳12:直接重置游戏，但保留一半分数。生成32游戏刻后消失
    5,  // 💊13:碰触蛇身不死亡，改为清除之，并根据减少的长度，减少3/4分数.只能储存一次
    2,  // 🆙14:分数翻倍
    1,  // ☢️15:立即结束游戏,不保存记录，然后让你的计算机在5秒内关机。
    3,  // 16
    8,  // 17
    8,  // 18
    2,  // 19
    6,  // 20
    9,  // 21
    1,  // 22
    1,  // 23[23,29]为传送门，按组生成，上限均为1组
    1,  // 24
    1,  // 25
    1,  // 26
    1,  // 27
    1,  // 28
    1,  // 29
    6,  // 30
    6   // 31

};

int sum_gen_odd[ID_NUM];

int_1 map[SIZE_X][SIZE_Y];
int_1 map_print[SIZE_X][SIZE_Y];
struct Snake snake[SIZE_X * SIZE_Y];
char name[200];
clock_t speed;
clock_t start_time = 0;
int score = 0;
int max_score = 0;
char max_name[200] = "";
int snake_length = 0;

int_1 direction = EMPTY_DIC;
int life = 1;

int id10_use_time = 0;
int id10_time = 0;
coor id10_place[2];

bool id11 = false;

int id12_time = 0;
coor id12_place[2];

bool id13 = false;

int id15_time = 0;
coor id15_place[2];

int id16_use_time = 0;
bool id16_flag = false;

int bleed_ready_time = 512; // 储存流血的倒计时

bool bleed = false;
int_1 bleed_count;

int id19_use_time = 0;

int id22_time = 0;
coor id22_place[2];

struct Portal portals[7] = {
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}};
int id30_use_time = 0;

int id31_use_time = 0;

int game_clock = 0;

void read_score();

void print_map()
{

    char temp[200];
    char chars[SIZE_X * 5];
    for (int i = 0; i < SIZE_Y; i++)
    {
        strcpy(chars, "");
        for (int j = 0; j < SIZE_X; j++)
        {
            strcat(chars, id_dict[map_print[j][i]]);
        }
        puts(chars);
    }
    strcpy(chars, "");
    sprintf(chars, "玩家:%s\t游戏速度:%dclock/s\t当前游戏刻:%dclock", name, speed, game_clock);
    puts(chars);

    strcpy(chars, "");
    sprintf(chars, "最高纪录:%d\t纪录保持者:%s", max_score, max_name);
    puts(chars);

    strcpy(chars, "");
    sprintf(chars, "当前得分:%d\t❤️x%d\t", score, life);
    if (id11)
    {
        sprintf(temp, "%s\t", id_dict[11]);
        strcat(chars, temp);
    }
    if (id13)
    {
        sprintf(temp, "%s\t", id_dict[13]);
        strcat(chars, temp);
    }
    if (bleed)
    {
        sprintf(temp, "你已经流血%s,请寻找🩹\t", id_dict[17]);
        strcat(chars, temp);
    }
    puts(chars);

    strcpy(chars, "");
    if (bleed_ready_time > 0)
    {
        sprintf(temp, "强制流血%s倒计时:%d\t", id_dict[17], bleed_ready_time);
        strcat(chars, temp);
    }
    if (id10_use_time > 0)
    {
        sprintf(temp, "%s:%d\t", id_dict[10], id10_use_time);
        strcat(chars, temp);
    }
    if (id19_use_time > 0)
    {
        sprintf(temp, "%s:%d\t", id_dict[19], id19_use_time);
        strcat(chars, temp);
    }
    puts(chars);

    puts("按wasd或方向键来移动,按\" [ \"暂停，按\" ] \"退出");
}
void print_test()
{

    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++)
        {

            printf("%2x", map[j][i]);
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
    memcpy(map_print, map, SIZE_X * SIZE_Y);

    // 蛇初始化
    srand(SEED);
    unsigned int ra = abs(rand());
    snake[0].x = ra % (SIZE_X - 2) + 1;
    snake[0].y = ra % (SIZE_Y - 2) + 1;
    snake_length = 1;
    map[snake[0].x][snake[0].y] = 2;
    life = 1;
    direction = EMPTY_DIC;

    // 实体上限初始化
    memcpy(entity_limit, entity_limit_init, sizeof(entity_limit_init));

    // 实体生成概率初始化
    int temp_sum = 0;
    for (int i = 1; i < 4; i++)
    {
        sum_gen_odd[i] = gen_odd[i];
    }
    for (int i = 4; i < ID_NUM; i++)
    {
        temp_sum += gen_odd[i];
        sum_gen_odd[i] = temp_sum;
    }

    sum_gen_odd[0] = sum_gen_odd[ID_NUM - 1] * sum_gen_odd[1];
    max_score = 0;
    strcpy(max_name, "");
    id10_use_time = 0;
    id10_time = 0;
    id11 = false;
    id12_time = 0;
    id13 = false;
    id15_time = 0;
    id16_use_time = 0;
    id16_flag = false;
    bleed_ready_time = 512; // 储存流血的倒计时
    bleed = false;
    id19_use_time = 0;
    for (int i = 0; i < 7; i++)
    {
        portals[i].exist = false;
    }
    id30_use_time = 0;
    id31_use_time = 0;
    read_score();
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
    srand(SEED);
    unsigned int ra = abs(rand());
    coor x = ra % (SIZE_X - 2) + 1;
    coor y = ra % (SIZE_Y - 2) + 1;

    if (id10_use_time > 0)
        return;
    if (!judge(x, y))
        return;
    if (entity_limit[0] <= 0)
        return;

    Sleep(1);
    srand(SEED);
    ra = abs(rand());
    unsigned short gen_num = ra % sum_gen_odd[0];
    unsigned short id = 0;

    for (int i = 4; i < ID_NUM; i++)
    {
        if (sum_gen_odd[i - 1] <= gen_num && gen_num < sum_gen_odd[i])
        {
            id = i;
            if (entity_limit[id] <= 0)
                if (id == ID_NUM - 1)
                    id = 0;
                else
                {
                    gen_num += gen_odd[i];
                    continue;
                }
            else
                break;
        }
    }

    // 特殊实体处理
    switch (id)
    {
    case 0:
        return;
    case 10:
        id10_place[0] = x;
        id10_place[1] = y;
        id10_time = 256;
        break;
    case 12:
        id12_place[0] = x;
        id12_place[1] = y;
        id12_time = 32;
        break;
    case 15:
        id15_place[0] = x;
        id15_place[1] = y;
        id15_time = 64;
        break;
    case 22:
        id22_place[0] = x;
        id22_place[1] = y;
        id22_time = 32;

    default:
        break;
    }
    if (id < 23 || id > 29)
    {
        map[x][y] = id;
    }
    else
    {
        portals[id - 23].x[0] = x;
        portals[id - 23].y[0] = y;
        Sleep(1);
        srand(SEED);
        ra = rand();
        x = ra % (SIZE_X - 2) + 1;
        y = ra % (SIZE_Y - 2) + 1;
        if (!judge(x, y))
            return;

        portals[id - 23].x[1] = x;
        portals[id - 23].y[1] = y;

        portals[id - 23].exist = true;
    }
    if (id > 3)
    {
        entity_limit[id]--;
        entity_limit[0]--;
    }
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
    int_1 count = 0;
    int_1 iv[4][2] = {
        {1, 1},
        {0, -2},
        {-2, 0},
        {0, 2}}; // 偏移量数组，控制蛇身撞墙时的生成机制
    if (snake_length == 1)
    {
        if (direction == RIGHT)
            snake[snake_length].x = snake[0].x - 1;
        else
            snake[snake_length].x = snake[0].x + 1;
        snake[snake_length].y = snake[0].y;
        while ((snake[snake_length].x >= SIZE_X) || (snake[snake_length].y >= SIZE_Y) || (map[snake[snake_length].x][snake[snake_length].y] == 1))
        {
            snake[snake_length].x += iv[count][0];
            snake[snake_length].y += iv[count][1];
            count++;
        }
        snake_length++;
        n--;
    }

    for (int i = 0; i < n; i++)
    {
        count = 0;
        snake[snake_length].x = snake[snake_length - 1].x * 2 - snake[snake_length - 2].x;
        snake[snake_length].y = snake[snake_length - 1].y * 2 - snake[snake_length - 2].y;

        while ((snake[snake_length].x >= SIZE_X) || (snake[snake_length].y >= SIZE_Y) || (map[snake[snake_length].x][snake[snake_length].y] == 1))
        {
            snake[snake_length].x += iv[count][0];
            snake[snake_length].y += iv[count][1];
            count++;
        }
        snake_length++;
    }
}

bool isAbsorbable(int_1 id)
{
    if ((id < 5) || (id == 15) || ((id > 22) && (id < 30)))
        return false;
    else
        return true;
}

bool interactive(int_1 id, coor x, coor y)
// 返回值代表是否有改变方向的行为

{
    int length_temp;
    
    if (id16_flag)
    {
        id16_flag = false;
        if (
            interactive(map[x-1][y-1], x-1,y-1) ||
            interactive(map[x-1][y], x-1,y) ||
            interactive(map[x-1][y+1], x-1,y+1) ||

            interactive(map[x][y-1], x,y-1) ||
            interactive(map[x][y], x,y) ||
            interactive(map[x][y+1], x,y+1) ||

            interactive(map[x+1][y-1], x-1,y-1) ||
            interactive(map[x+1][y], x-1,y) ||
            interactive(map[x+1][y+1], x-1,y+1)
        )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    switch (id)
    {
    case 1:
        if (--life != 0)
        {
            direction = EMPTY_DIC;
            return true;
        }
        break;
    case 3:
        if (!id13)
        {
            if (--life != 0)
            {
                direction = EMPTY_DIC;
                return true;
            }
        }
        else
        {
            id13 = false;
            int index = search_body(x, y);
            score -= ((snake_length - index)) * 3 / 4;
            snake_length = index;
        }
        break;
    case 4:
        if (!id11)
        {
            if (--life != 0)
            {
                direction = EMPTY_DIC;
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
        if (life < 5)
            life++;
        else
            score++;
        gen_snakebody(1);
        break;
    case 10:
        id10_use_time = 64;
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
        system("shutdown -s -t 10");
        Sleep(6000);
        printf("You find my Easter egg!Congratulations!\nScreenshot to get a price.");
        Sleep(6000);
        system("shutdown -s -t 1");
        break;
    case 16:
        if (id16_use_time <= 0)
        {
            id16_use_time = 8;
        }
        break;
    case 17:
        bleed = true;
        bleed_count = 0;
        break;
    case 18:
        if (bleed)
        {
            bleed = false;
            bleed_ready_time = 512;
        }
        else
            life--;
        break;
    case 19:
        id19_use_time = 32;
        break;
    case 20:
        srand(SEED);
        return interactive((abs(rand()) % (ID_NUM - 4)) + 4, x, y);
        break;
    case 21:
        if (id13)
        {
            id13 = false;
        }
        else
        {
            life = 1;
            if ((snake_length -= 5) <= 0)
                life = 0;
            score -= 10;
        }
        break;
    case 22:
        life += 5;
        score -= 5;
        break;
    case 23:
        portals[0].exist = false;
        if (portals[0].x[0] == x && portals[0].y[0] == y)
        {
            snake[0].x = portals[0].x[1];
            snake[0].y = portals[0].y[1];
        }
        else
        {
            snake[0].x = portals[0].x[0];
            snake[0].y = portals[0].y[0];
        }
        length_temp = snake_length - 1;
        snake_length = 1;
        break;
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
        portals[id - 23].exist = false;
        if (portals[id - 23].x[0] == x && portals[id - 23].y[0] == y)
        {
            snake[0].x = portals[id - 23].x[1];
            snake[0].y = portals[id - 23].y[1];
        }
        else
        {
            snake[0].x = portals[id - 23].x[0];
            snake[0].y = portals[id - 23].y[0];
        }
        length_temp = snake_length - 1;
        snake_length = 1;
        gen_snakebody(length_temp);
        break;

    case 30:
        speed *= 2;
        id30_use_time = 16;
        break;
    case 31:
        speed /= 2;
        id31_use_time = 64;
        break;

    default:
        break;
    }
    if (id > 3)
    {
        entity_limit[id]++;
        entity_limit[0]++;
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

void put_Entity_into_map()
{
    for (int i = 1; i < SIZE_X - 1; i++)
    {
        for (int j = 1; j < SIZE_Y - 1; j++)
        {
            if (map[i][j] == 2 || map[i][j] == 3 || (map[i][j] >= 23 && map[i][j] <= 29))
                map[i][j] = 0;
        }
    }
    map[snake[0].x][snake[0].y] = 2;
    for (int i = 1; i < snake_length; i++)
    {
        map[snake[i].x][snake[i].y] = 3;
    }
    for (int i = 0; i < 7; i++)
    {
        if (portals[i].exist)
        {
            map[portals[i].x[0]][portals[i].y[0]] = i + 23;
            map[portals[i].x[1]][portals[i].y[1]] = i + 23;
        }
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
        {
            map[id10_place[0]][id10_place[1]] = 0;
            entity_limit[10]++;
            entity_limit[0]++;
        }
    }

    if (id12_time > 0)
    {
        if (--id12_time == 0)
        {
            map[id12_place[0]][id12_place[1]] = 0;
            entity_limit[12]++;
            entity_limit[0]++;
        }
    }
    if (id15_time > 0)
    {
        if (--id15_time == 0)
        {
            map[id15_place[0]][id15_place[1]] = 0;
            entity_limit[15]++;
            entity_limit[0]++;
        }
    }
    if (id16_use_time > 0)
    {
        id16_flag = true;
        memset(map_print, 16, SIZE_X * SIZE_Y);
        if (--id16_use_time == 0)
        {
            id16_flag = false;
            memcpy(map_print, map, SIZE_X * SIZE_Y);
        }
    }
    else
    {
        memcpy(map_print, map, SIZE_X * SIZE_Y);
    }
    if (--bleed_ready_time == 0)
    {
        clock_t seed = SEED;
        bleed = true;
        srand(seed++);
        coor x = abs(rand()) % (SIZE_X - 2) + 1;
        coor y = abs(rand()) % (SIZE_Y - 2) + 1;
        while (!judge(x, y))
        {
            srand(seed++);
            x = abs(rand()) % (SIZE_X - 2) + 1;
            y = abs(rand()) % (SIZE_Y - 2) + 1;
        }
        map[x][y] = 18;
    }
    if (bleed)
    {
        if (id13)
        {
            bleed = false;
            id13 = false;
        }
        else if (life > 1)
        {
            if (bleed_count >= 20)
            {
                life--;
                bleed_count = 0;
            }
            else
            {
                bleed_count++;
            }
        }
        else if (life == 1 && snake_length > 1)
        {
            if (bleed_count >= 5)
            {
                snake_length--;
                bleed_count = 0;
            }
            else
            {
                bleed_count++;
            }
        }
        else
        {
            life--;
        }
    }
    if (id19_use_time > 0)
    {
        id19_use_time--;
    }
    if (id22_time > 0)
    {
        if (--id22_time == 0)
        {
            map[id22_place[0]][id22_place[1]] = 0;
            entity_limit[22]++;
            entity_limit[0]++;
        }
    }
    if (id30_use_time > 0)
    {
        if (--id30_use_time == 0)
        {
            speed /= 2;
        }
    }
    if (id31_use_time > 0)
    {
        if (--id31_use_time == 0)
        {
            speed *= 2;
        }
    }
}

void start_game(int_1 mode)
{
    switch (mode)
    {
        int sp_case1;
    case 1:
        fresh_stdin();
        strcmp(name, "");
        printf("输入你的名字:");
        scanf("%[^\n]198s", name);
        CL;
        fresh_stdin();
        printf("你好,%s!\n选择你的速度等级(1~6级)或输入速度值(clock/s):", name);
        scanf("%d", &sp_case1);
        switch (sp_case1)
        {
        case 0:
            speed = 1000;
        case 1:
            speed = 500;
            break;
        case 2:
            speed = 400;
            break;
        case 3:
            speed = 250;
            break;
        case 4:
            speed = 200;
            break;
        case 5:
            speed = 125;
            break;
        case 6:
            speed = 50;
            break;
        case -1:
            speed = 25;
            break;
        default:
            speed = abs(sp_case1);
            break;
        }
        CL;
        clock_t start_time = 0;
        score = 0;

        game_clock = 0;
        break;
    default:
        break;
    }
}

bool get_input()
{
    fresh_stdin();
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
        if (snake_length != 1)
            if (direction != RIGHT)
                direction = LEFT;
            else
                ;
        else
            direction = LEFT;
        break;
    case 100:
    case 56:
    case 77:
        if (snake_length != 1)
            if (direction != LEFT)
                direction = RIGHT;
            else
                ;
        else
            direction = RIGHT;
        break;
    case 119:
    case 50:
    case 72:
        if (snake_length != 1)
            if (direction != DOWN)
                direction = UP;
            else
                ;
        else
            direction = UP;
        break;
    case 115:
    case 54:
    case 80:
        if (snake_length != 1)
            if (direction != UP)
                direction = DOWN;
            else
                ;
        else
            direction = DOWN;
        break;
    case '[':
    case SPACE:
        wait_for_kbhit();
        break;
    case ']':
    case ESC:
        CL;
        fresh_stdin();
        printf("确定要退出嘛?\n按Y确认退出");
        if (getch() == 'y')
            return true;
        break;
    default:
        break;
    }
    return false;
}

void store_score()
{
    const int SIZE_OF_MESSAGE = 512;
    byte key[SIZE_OF_MESSAGE];
    byte message[SIZE_OF_MESSAGE];

    byte store[SIZE_OF_MESSAGE * 2];

    int seed = time(NULL);

    for (int i = 0; i < SIZE_OF_MESSAGE - 1; i++)
    {
        srand(seed++);
        key[i] = (byte)rand();
    }
    key[SIZE_OF_MESSAGE - 1] = 0;

    memset(message, 0, SIZE_OF_MESSAGE);
    snprintf(message, SIZE_OF_MESSAGE / 2, "@%d:%s\0", score, name);
    for (int i = SIZE_OF_MESSAGE / 2; i < SIZE_OF_MESSAGE - 1; i++)
    {
        srand(seed++);
        message[i] = (byte)rand();
    }
    message[SIZE_OF_MESSAGE - 1] = 0;
    encrypt_xor(message, key, SIZE_OF_MESSAGE);

    memcpy(store, key, SIZE_OF_MESSAGE);

    for (int i = SIZE_OF_MESSAGE; i < SIZE_OF_MESSAGE * 2; i++)
    {
        store[i] = message[i - SIZE_OF_MESSAGE];
    }

    FILE *fp;
    fp = fopen("./scoreRecords.dat", "wb");
    fwrite(store, SIZE_OF_MESSAGE * 2, 1, fp);
    fclose(fp);
}

void read_score()
{
    const int SIZE_OF_MESSAGE = 512;
    byte key[SIZE_OF_MESSAGE];
    byte message[SIZE_OF_MESSAGE];

    byte text[SIZE_OF_MESSAGE * 2];

    FILE *fp;
    if ((fp = fopen("./scoreRecords.dat", "rb")) == NULL)
    {
        max_score = 0;
        strcpy(max_name, "None\0");
        return;
    }

    fread(text, SIZE_OF_MESSAGE * 2, 1, fp);
    fclose(fp);

    for (int i = 0; i < SIZE_OF_MESSAGE; i++)
    {
        key[i] = text[i];
    }

    for (int i = SIZE_OF_MESSAGE; i < SIZE_OF_MESSAGE * 2; i++)
    {
        message[i - SIZE_OF_MESSAGE] = text[i];
    }

    decrypt_xor(message, key, SIZE_OF_MESSAGE);

    char temp_score[50] = "";
    char temp_name[200] = "";

    byte *message_ptr = message;

    for (int i = 0; i < 50; i++)
    {
        if (*++message_ptr == ':')
        {
            temp_score[i] = '\0';
            break;
        }
        else
        {
            temp_score[i] = *message_ptr;
        }
    }

    for (int i = 0; i < 200; i++)
    {
        if (*++message_ptr == '\0')
        {
            temp_name[i] = '\0';
            break;
        }
        else
        {
            temp_name[i] = *message_ptr;
        }
    }

    max_score = atoi(temp_score);
    strcpy(max_name, temp_name);
}