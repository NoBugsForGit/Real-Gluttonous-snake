#include "cmdgame.h"
#include "crypto.h"

#define ID_NUM 48
#define SIZE_X 50
#define SIZE_Y 23

#define SCORE_FILE "../data/scoreRecords.dat"

const char *help_first_line = "++Version0.5.0,DevelopByNoBugsForGit(Github)++url=https://github.com/NoBugsForGit/Real-Gluttonous-snake;\n";
const char *Github_url = "Github:https://github.com/NoBugsForGit/Real-Gluttonous-snake\0";


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

struct Enemy
{
    int cd_time;        // 作为子弹时储存实体id
    int remaining_time; // 技能还剩多长时间。作为子弹时不储存数据
    int speed;          // 实际上是速度的倒数，标记每多少clock移动1格
    int direction;
    coor x;
    coor y;
    int life; // 小于-5视为无敌
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
    "⚡", // 31
    "🍖", // 32
    "🔥", // 33
    "💧", // 34
    "🛡️",  // 35
    "⚔️",  // 36
    "💣", // 37
    "🌀", // 38
    "➕", // 39
    "💙", // 40
    "💚", // 41
    "💛", // 42
    "🧡", // 43
    "🩷", // 44
    "🤍", // 45
    "🤎", // 46
    "💔"  // 47
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
    2,   // 17
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
    3,   // 31
    5,   // 32
    2,   // 33
    2,   // 34
    3,   // 35
    10,  // 36
    5,   // 37
    5,   // 38
    5,   // 39
    1,   // 40
    1,   // 41
    1,   // 42
    1,   // 43
    1,   // 44
    1,   // 45
    1,   // 46
    1,   // 47

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
    6,  // 31
    6,  // 32
    2,  // 33
    2,  // 34
    5,  // 35
    6,  // 36
    3,  // 37
    3,  // 38
    3,  // 39
    1,  // 40
    1,  // 41
    1,  // 42
    1,  // 43
    1,  // 44
    1,  // 45
    1,  // 46
    1,  // 47

};

#define gain_entity_num 10
const int gain_entity_id[gain_entity_num] = {5, 6, 7, 8, 9, 11, 12, 13, 14, 22}; // 标记增益实体id

const int id19_dict[] = {1, 2, 3, 4, 23, 24, 25, 26, 27, 28, 29}; // 标记磁铁不能吸引的实体id

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

int id13_num = 0;

int id15_time = 0;
coor id15_place[2];

int id16_use_time = 0;

int bleed_ready_time = 512; // 储存流血的倒计时

bool bleed = false;
int_1 bleed_count;

int id19_use_time = 0;
bool id19_flag = false;

int id22_time = 0;
coor id22_place[2];

int difficulty = 1;

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

bool hungry = false;
int hungry_count = 0;
bool poison = false;
int poison_count = 0;
bool burn = false;
int burn_count = 0;
bool wet = false;
int id35_use_time = 0;
int id36_use_time = 0;
int id38_use_time = 0;

#define ENEMY_NUM 8
struct Enemy enemy[ENEMY_NUM] = {
    {16, 16, 0, 4, 0, 0, 1},
    {512, 512, 0, 4, 0, 0, 1},
    {0, 0, 0, 4, 0, 0, 1},
    {0, 0, 0, 4, 0, 0, 1},
    {520, 520, 0, 4, 0, 0, 1},
    {10, 10, 0, 4, 0, 0, 1},
    {100, 100, 0, 4, 0, 0, 1},
    {10, 10, 0, 4, 0, 0, 1}};

byte check_map[SIZE_X][SIZE_Y] = {8};

int game_clock = 0;


void help();
void print_map();
void print_test();
void init();
bool judge(coor x, coor y);
void generate();
int search_body(coor  x, coor y);
void gen_snakebody(int n);
bool isAbsorbable(int_1 id);
bool interactive(int_1 id, coor x, coor y);
void move();
void put_Entity_into_map();
bool isEnemy(int id);
void attack(int range);
void attack_surrounded();
void clock_count();
void start_game(int_1 mode);
bool get_input();
void store_score();
void read_score();

