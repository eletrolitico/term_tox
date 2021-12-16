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
        mvwprintw(win, 1, 2, "Status aqui!");

        const std::string msg = "Your TOX ID:" + t_hand->m_toxID;
        mvwprintw(win, 2, 2, msg.c_str());

        wrefresh(win);
    }
}