#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>

// declaração de variáveis estáticas das classes
#define CONTENT_WINDOW_IMPL
#include "interface/content_window.h"
#undef CONTENT_WINDOW_IMPL

#include "interface/main.h"
#include "tox_handler.h"

ToxHandler *tHand = NULL;
interface::Main *iFace = NULL;

void drawStuff()
{
    iFace->updateInterface('\0');
}

int main()
{
    tHand = new ToxHandler();
    iFace = interface::Main::getInstance(tHand);

    tHand->setUpdateCallback(drawStuff);

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