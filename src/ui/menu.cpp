#include "ui/menu.h"

namespace ui
{
    Menu::Menu(int x_max, int h) : height_(h), width_(x_max - 2)
    {
        win_ = newwin(3, x_max - 2, 1, 1);
        draw();
    }

    BaseWindow *Menu::get_selected_window(int ch)
    {
        switch (ch)
        {
        case KEY_LEFT:
            if (selected_ > (uint8_t)0)
            {
                windows_[selected_--]->on_blur();
                windows_[selected_]->on_focus();
            }
            break;

        case KEY_RIGHT:
            if (selected_ < windows_.size() - 1)
            {
                windows_[selected_++]->on_blur();
                windows_[selected_]->on_focus();
            }
            break;
        }

        return windows_[selected_];
    }

    void Menu::draw()
    {
        box(win_, 0, 0);

        for (size_t i = 0; i < windows_.size(); ++i)
        {
            if (i == selected_)
                wattr_on(win_, A_STANDOUT, nullptr);
            if (i == 0)
                mvwprintw(win_, height_ / 2, 2, "%s", windows_[0]->getTitle().c_str());
            else
                wprintw(win_, "%s", windows_[i]->getTitle().c_str());
            if (i == selected_)
                wattr_off(win_, A_STANDOUT, nullptr);

            if (i < windows_.size() - 1)
                waddch(win_, ' ');
        }

        wrefresh(win_);
    }
} // namespace ui
