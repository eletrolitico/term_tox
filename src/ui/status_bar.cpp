#include <string>
#include "ui/status_bar.h"

namespace ui
{
    StatusBar::StatusBar(int a, int b, int c) : BaseWindow("", a, b, c)
    {
    }

    void StatusBar::draw()
    {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, (t_hand->get_self_name() + " - " + t_hand->get_self_status()).c_str());
        wprintw(win, " Avg TST(ms): %d", t_hand->get_avg_tox_sleep_time());

        const std::string msg = "Your Tox Address: " + t_hand->get_self_tox_address();
        mvwprintw(win, 2, 2, msg.c_str());

        wrefresh(win);
    }
}