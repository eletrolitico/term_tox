#include "ui/controller.h"

#include "ui/friends.h"
#include "ui/requests.h"
#include "ui/messages.h"
#include "ui/options.h"

#include <ncurses.h>
#include <stdlib.h>
#include <vector>
#include <string>

namespace ui
{
    Controller *Controller::_main = nullptr;

    Controller::Controller(ToxHandler *t_hand)
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

        //attr_on(COLOR_PAIR(1), nullptr);
        printw("Aperte F4 para sair");
        //attr_off(COLOR_PAIR(1), nullptr);

        refresh();

        BaseWindow::setToxHandler(t_hand);
        BaseWindow::setDims(this->xMax, this->yMax - 8, 8);

        this->menu = std::make_unique<Menu>(this->xMax, 3);

        this->menu->add_window(new Friends());
        this->menu->add_window(new Requests());
        this->menu->add_window(new Messages());
        this->menu->add_window(new Options());

        this->menu->draw();
        this->menu->get_window(0)->draw();

        this->status_bar = new StatusBar(this->xMax, 4, 4);
        this->status_bar->draw();
    }

    Controller *Controller::get_instance(ToxHandler *t)
    {
        if (_main == nullptr)
            _main = new Controller(t);

        return _main;
    }

    void Controller::update(int ch)
    {
        BaseWindow *sel = menu->get_selected_window(ch);
        menu->draw();

        this->status_bar->draw();

        if (sel->update(ch))
            sel->draw();

        Friends *v = dynamic_cast<Friends *>(sel);
        if (v)
            this->status_bar->draw();
    }

    Controller::~Controller()
    {
        for (auto e : windows)
            delete e;

        endwin();
    }

}