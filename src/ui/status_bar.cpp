#include <string>
#include "ui/status_bar.h"

namespace ui
{
    StatusBar::StatusBar(int a, int b, int c) : BaseWindow("", a, b, c)
    {
    }

    void StatusBar::draw()
    {
        std::string tk_to = "no one";

        if (talking_to != nullptr)
        {
            tk_to = std::string(talking_to->name);
        }

        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, (t_hand->get_self_name() + " - " + t_hand->get_self_status()).c_str());
        wprintw(win, " | Talking to %s", tk_to.c_str());

        const std::string msg = "Your Tox Address: " + t_hand->get_self_tox_address();
        mvwprintw(win, 2, 2, msg.c_str());

        wrefresh(win);
    }
}