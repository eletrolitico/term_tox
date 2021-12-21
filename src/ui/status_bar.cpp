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
        mvwprintw(win, 1, 2, t_hand->get_status_self().c_str());

        const std::string msg = "Your TOX ID: " + t_hand->m_tox_id;
        mvwprintw(win, 2, 2, msg.c_str());

        wrefresh(win);
    }
}