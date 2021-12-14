#include "interface/main.h"

#include "interface/friends.h"

#include <ncurses.h>
#include <stdlib.h>
#include <vector>
#include <string>

namespace interface
{
    Main *Main::_main = NULL;

    Main::Main(ToxHandler *t_hand)
    {
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

        init_pair(1, COLOR_YELLOW, COLOR_BLUE);

        keypad(stdscr, TRUE);

        getmaxyx(stdscr, this->yMax, this->xMax);

        printw("Aperte F1 para sair");

        refresh();

        std::vector<std::string> menus = {
            "Add",
            "Del",
            "Invite",
            "Contact",
        };

        this->menu = std::make_unique<Menu>(this->xMax, 3, menus);

        ContentWindow::setToxHandler(t_hand);

        this->windows.push_back(new Friends(this->xMax, this->yMax - 7, 7));
        this->windows.push_back(new Friends(this->xMax, this->yMax - 7, 7));
        this->windows.push_back(new Friends(this->xMax, this->yMax - 7, 7));
        this->windows.push_back(new Friends(this->xMax, this->yMax - 7, 7));

        this->windows[0]->draw();

        this->status_bar = new StatusBar(this->xMax, 3, 4);
        this->status_bar->draw();
    }

    Main *Main::getInstance(ToxHandler *t)
    {
        if (_main == NULL)
            _main = new Main(t);

        return _main;
    }

    void Main::updateInterface(int ch)
    {
        int sel = menu->getSelectedMenu(ch);

        this->windows[sel]->update(ch);
        this->windows[sel]->draw();
    }

    Main::~Main()
    {
        for (auto e : windows)
            delete e;

        endwin();
    }

}