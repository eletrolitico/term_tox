#include "interface/main.h"

#include "interface/friends.h"
#include "interface/requests.h"

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
        set_escdelay(50);

        init_pair(1, COLOR_YELLOW, COLOR_BLUE);

        keypad(stdscr, TRUE);

        getmaxyx(stdscr, this->yMax, this->xMax);

        printw("Aperte F1 para sair");

        refresh();

        ContentWindow::setToxHandler(t_hand);
        ContentWindow::setDims(this->xMax, this->yMax - 8, 8);

        this->menu = std::make_unique<Menu>(this->xMax, 3);

        this->menu->addWindow(new Friends());
        this->menu->addWindow(new Requests());
        this->menu->addWindow(new Friends());
        this->menu->addWindow(new Friends());

        this->menu->draw();
        this->menu->getWindow(0)->draw();

        this->status_bar = new StatusBar(this->xMax, 4, 4);
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
        auto sel = menu->getSelectedMenu(ch);

        this->status_bar->draw();

        sel->update(ch);
        sel->draw();
    }

    Main::~Main()
    {
        for (auto e : windows)
            delete e;

        endwin();
    }

}