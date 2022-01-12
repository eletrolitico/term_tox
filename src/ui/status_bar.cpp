#include "ui/status_bar.h"

namespace ui
{
    StatusBar::StatusBar(int x_max, int height, int start_y) : Window("", x_max, height, start_y)
    {
    }

    void StatusBar::draw()
    {
        std::string tk_to = "no one";

        if (ui::WindowInfo::get().talking_to_ != nullptr)
        {
            tk_to = std::string(ui::WindowInfo::get().talking_to_->name_);
        }

        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, (ToxHandler::get().get_self_name() + " - " + ToxHandler::get().get_self_status()).c_str());
        wprintw(win, " | Talking to %s", tk_to.c_str());

        const std::string msg = "Your Tox Address: " + ToxHandler::get().get_self_tox_address();
        mvwprintw(win, 2, 2, msg.c_str());

        wrefresh(win);
    }
}