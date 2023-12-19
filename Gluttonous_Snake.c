#include "snake.h"

const char *help_first_line = "++Version0.4,DevelopByNoBugsForGit(Github)++url=https://github.com/NoBugsForGit/Real-Gluttonous-snake;\n";
const char *Github_url = "Github:https://github.com/NoBugsForGit/Real-Gluttonous-snake\0";
/*
    预期更新：

        添加新实体：⚪闪光弹、🩸流血、🩹绷带、🧲磁铁、❓随机幸运实体、💜毒心、💖5心 、传送门~0.4


        加入难度系统~0.5
        加入围攻障碍物~0.5
        加入敌人~0.5

        添加解密模式~0.6

        添加关卡编辑功能~0.7

        界面大幅度美化~0.8

        引入对战功能~0.9

        引入联网系统~0.10

        新实体⭐🔑🔒🧱🔥💧🛡️⚔️💣🔦💲🔶⬛🚪/腐肉🍖~？？？

        游戏平衡调整:
            削弱流血效果

*/

void Game(int_1 mode)
{
    switch (mode)
    {
    case 1:
        init();
        start_game(mode);
        start_time = clock();
        clock_t while_start_time;
        while (true)
        {
            while_start_time = clock();
            generate();
            put_Entity_into_map();
            CL;
            print_map();
            if (get_input())
                break;
            move();
            clock_count();
            clock_t sleep_time = speed - (clock() - while_start_time);
            if (sleep_time < 0)
                sleep_time = 0;
            Sleep(sleep_time);
            if (life == 0)
                break;
        };
        CL;
        printf("%s:%d\n", name, score);

        if (score > max_score)
        {
            printf("要存储记录吗?\n任意键 存储\nESC 放弃");
            fresh_stdin();
            Sleep(1500);
            wait_for_kbhit();
            if (getch() != ESC)
            {
                CL;
                puts("存储中，请稍候......");
                store_score();
                puts("存储成功!按任意键返回主界面");
                wait_for_kbhit();
            }
        }
        else
        {
            fresh_stdin();
            Sleep(1500);
            puts("游戏结束！按任意键返回主界面");
            wait_for_kbhit();
        }

        break;

    case 2:
        printf("该模式预计在0.6版本加入,敬请期待!\n按任意键返回");
        wait_for_kbhit();
        break;

    case 3:
        printf("该模式预计在0.7版本加入,敬请期待!\n按任意键返回");
        wait_for_kbhit();
        break;
    case 4:
        printf("该模式预计在0.9版本加入,敬请期待!\n按任意键返回");
        wait_for_kbhit();
        break;
        break;
    default:
        break;
    }
}

void help()
{
    const int STR_SIZE = 200;
    FILE *fp_help = fopen("./help.txt", "rt");
    char str[STR_SIZE];

    if (fp_help == NULL)
    {
        printf("打开帮助文档失败\n请检查程序所在文件夹中是否有help.snake文件\n");
        puts("按任意键返回");
        wait_for_kbhit();
    }
    else
    {
        fgets(str, STR_SIZE - 1, fp_help);
        if (strcmp(str, help_first_line) != 0)
        {
            puts("警告:帮助文档已失效或者被意外改动,建议您重新下载帮助文档以获取正确帮助!");
            puts(Github_url);
            Sleep(1000);
        }
        while (fgets(str, STR_SIZE - 1, fp_help) != NULL)
        {
            printf("%s", str);
        }

        fclose(fp_help);
        putchar('\n');
        putchar('\n');
        puts("按任意键返回");
        wait_for_kbhit();
    }
}
int main()
{
    main_init("Gluttonous_Snake");
    puts("开源程序,不得用于非法用途和商业用途");
    puts(Github_url);
    Sleep(2000);
    while (true)
    {
        CL;
        int_1 command_num;
        fresh_stdin();
        printf("欢迎来到贪吃蛇小游戏!\n输入数字进入相应模式:\n");
        printf("0:获取基础帮助\n"
               "1:经典模式\n"
               "2:迷宫模式\n"
               "3:解谜模式\n"
               "4:多人模式\n"
               "ESC:退出\n");

        command_num = getch();
        CL;
        if (command_num == '0')
            help();
        else if ('0' < command_num && command_num < '5')
        {
            Game(command_num - 0x30);
        }
        else if (command_num == ESC)
        {
            return 0;
        }
        else
        {
            printf("Error!请在1秒后重输");
            Sleep(1000);
        }
    }
    return 0;
}