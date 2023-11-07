#include "snake.h"
/*
    预期更新：
        显示时间
        存储记录
        优化运行Sleep
        bug:定时消失的实体不能正确消失
*/
int main()
{
    clock_t start_time = clock();
#pragma warning(disable : 4996) // 消除警告
    system("title 贪吃蛇");     // 设置cmd窗口的名字
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE; // 隐藏光标
    SetConsoleCursorInfo(console, &cursorInfo);

    init();
    start_game();
    while (true)
    {
        body_generate();
        generate();
        CL;
        print_map();
        get_input();
        move();
        clock_count();
        Sleep(speed);
        if (life == 0)
            break;
    };
    CL;
    printf("%s:%d\n", name, score);
    system("pause");
    return 0;
}