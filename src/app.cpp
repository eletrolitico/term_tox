#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>

#include "interface/main.h"

int main()
{
    auto iFace = interface::Main::init();

    while (int ch = getch())
    {
        if (ch == KEY_F(1))
            break;
        iFace->updateInterface(ch);
    }

    endwin();

    return 0;
}