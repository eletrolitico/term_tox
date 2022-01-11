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
    Controller *Controller::main_ = nullptr;

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

        getmaxyx(stdscr, this->y_max_, this->x_max_);

        //attr_on(COLOR_PAIR(1), nullptr);
        printw("Press F4 to exit");
        //attr_off(COLOR_PAIR(1), nullptr);

        refresh();

        BaseWindow::setToxHandler(t_hand);
        BaseWindow::setDims(this->x_max_, this->y_max_ - 8, 8);

        this->menu_ = std::make_unique<Menu>(this->x_max_, 3);

        this->menu_->add_window(new Friends());
        this->menu_->add_window(new Requests());
        this->menu_->add_window(new Messages());
        this->menu_->add_window(new Options());

        this->menu_->draw();
        this->menu_->get_window(0)->draw();

        this->status_bar_ = new StatusBar(this->x_max_, 4, 4);
        this->status_bar_->draw();
    }

    Controller *Controller::get_instance(ToxHandler *t)
    {
        if (main_ == nullptr)
            main_ = new Controller(t);

        return main_;
    }

    void Controller::update(int ch)
    {
        BaseWindow *sel = menu_->get_selected_window(ch);
        menu_->draw();

        bool shouldDraw = sel->update(ch);

        if (shouldDraw)
        {
            this->status_bar_->draw();
            sel->draw();
        }
    }

    Controller::~Controller()
    {
        for (auto e : windows_)
            delete e;

        endwin();
    }

}