#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>

#include "interface/main.h"
#include "tox_handler.h"

int main()
{
    auto tHand = new ToxHandler();
    auto iFace = interface::Main::getInstance(tHand);

    while (int ch = getch())
    {
        if (ch == KEY_F(1))
            break;
        iFace->updateInterface(ch);
    }

    delete iFace;
    delete tHand;

    return 0;
}