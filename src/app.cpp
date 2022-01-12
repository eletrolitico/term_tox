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

int ui::BaseWindow::x_max_, ui::BaseWindow::height_, ui::BaseWindow::start_y_;
Friend *ui::BaseWindow::talking_to_ = nullptr;

//void draw_stuff() { screen.update('\0'); }

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