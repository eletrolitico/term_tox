#include "interface/main.h"

#include <ncurses.h>
#include <stdlib.h>
#include <vector>
#include <string>

namespace interface
{
    Main *Main::_main = NULL;

    Main *Main::init()
    {
        if (_main != NULL)
            return _main;

        //Start
        initscr();
        if (has_colors() == FALSE)
        {
            endwin();
            printf("Your terminal does not support color\n");
            exit(1);
        }
        start_color();
        raw();
        noecho();
        cbreak();
        curs_set(0);

        keypad(stdscr, TRUE);

        _main = new Main();

        getmaxyx(stdscr, _main->yMax, _main->xMax);

        printw("Aperte F1 para sair");
        refresh();

        std::vector<std::string> menus = {
            "Add",
            "Del",
            "Invite",
            "Contact",
        };

        _main->menu = std::make_unique<Menu>(3, _main->xMax - 2, 1, 1, menus);

        return _main;
    }

    void Main::updateInterface(int ch)
    {
        menu->getSelectedMenu(ch);
        refresh();
    }

}