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

#define coor unsigned short

#define EMPTY_DIC 4
#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3

#define ESC 27
#define SPACE 32

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
#define SEED clock()

void main_init(char *title)
{
    char command[100];
    strcpy(command, "title ");
    system("chcp 65001");
    CL;
    clock_t start_time = clock();
    // #pragma warning(disable : 4996) // 消除警告
    system(strncat(command, title, 90)); // 设置cmd窗口的名字
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE; // 隐藏光标
    SetConsoleCursorInfo(console, &cursorInfo);
    CL;
}

void wait_for_kbhit()
{
    while (!kbhit())
        Sleep(100);
    getch();
}

void fresh_stdin()
{
    fflush(stdin);
}

void print_persent(int persent)
{
    printf("%d%%\n",persent);
}
