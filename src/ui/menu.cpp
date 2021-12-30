#include "ui/menu.h"

namespace ui
{
    Menu::Menu(int xMax, int h) : height(h), width(xMax - 2)
    {
        win = newwin(3, xMax - 2, 1, 1);
        draw();
    }

    BaseWindow *Menu::get_selected_window(int ch)
    {
        switch (ch)
        {
        case KEY_LEFT:
            if (selected > (uint8_t)0)
            {
                windows[selected--]->on_blur();
                windows[selected]->on_focus();
            }
            break;

        case KEY_RIGHT:
            if (selected < windows.size() - 1)
            {
                windows[selected++]->on_blur();
                windows[selected]->on_focus();
            }
            break;
        }

        return windows[selected];
    }

    void Menu::draw()
    {
        box(win, 0, 0);

        for (size_t i = 0; i < windows.size(); ++i)
        {
            if (i == selected)
                wattr_on(win, A_STANDOUT, nullptr);
            if (i == 0)
                mvwprintw(win, height / 2, 2, "%s", windows[0]->getTitle().c_str());
            else
                wprintw(win, "%s", windows[i]->getTitle().c_str());
            if (i == selected)
                wattr_off(win, A_STANDOUT, nullptr);

            if (i < windows.size() - 1)
                waddch(win, ' ');
        }

        wrefresh(win);
    }
} // namespace ui
