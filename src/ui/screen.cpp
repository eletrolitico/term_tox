#include <ncurses.h>

#include "ui/screen.h"
#include "ui/friends.h"
#include "ui/requests.h"
#include "ui/messages.h"
#include "ui/options.h"

namespace ui
{
    Screen& Screen::get()
    {
        static Screen controller_;
        return controller_;
    }

    Screen::Screen()
    {
        //Start ncurses
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

        int y_max, x_max;
        getmaxyx(stdscr, y_max, x_max);

        printw("Press F4 to exit");

        refresh();

        BaseWindow::set_dimensions(x_max, y_max - 8, 8);
        
        //Start menu
        menu_ = std::make_unique<Menu>(x_max, 3);

        // menu_->add_window(std::make_shared<Friends>());
        // menu_->add_window(std::make_shared<Requests>());
        // menu_->add_window(std::make_shared<Messages>());
        // menu_->add_window(std::make_shared<Options>());

        menu_->add_window(new Friends());
        menu_->add_window(new Requests());
        menu_->add_window(new Messages());
        menu_->add_window(new Options());

        menu_->draw();
        menu_->get_window(0)->draw();

        //Start status bar
        status_bar_ = std::make_unique<StatusBar>(x_max, 4, 4);
        status_bar_->draw();
    }

    void Screen::update(int ch)
    {
        BaseWindow* sel = menu_->get_selected_window(ch);
        menu_->draw();

        sel->update(ch);

        status_bar_->draw();
        sel->draw();
    }

    Screen::~Screen()
    {
        endwin();
    }

}