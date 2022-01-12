#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>
#include <memory>

#include <stdio.h>
#include <fcntl.h>
#include "unistd.h"

#include "ui/screen.h"
#include "tox_handler.h"

inline void start_program()
{
    ui::Screen::get().update(0);

    while (int ch = getch())
    {
        if (ch == KEY_F(4))
            break;
        ui::Screen::get().update(ch);
    }
}

int main()
{
    start_program();
    return 0;
}