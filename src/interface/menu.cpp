#include "interface/menu.h"

namespace interface
{
    Menu::Menu(int h, int w, int y, int x, std::vector<std::string> menus) : menus(menus), heigh(h), width(w)
    {
        win = newwin(h, w, y, x);
        box(win, 0, 0);
        draw();
    }

    int Menu::getSelectedMenu(int ch)
    {
        switch (ch)
        {
        case KEY_LEFT:
            if (selected > (uint8_t)0)
                --selected;
            break;

        case KEY_RIGHT:
            if (selected < menus.size() - 1)
                ++selected;
            break;
        }

        draw();
        return selected;
    }

    void Menu::draw()
    {
        for (size_t i = 0; i < menus.size(); ++i)
        {
            if (i == selected)
                wattr_on(win, A_STANDOUT, NULL);
            if (i == 0)
                mvwprintw(win, heigh / 2, 2, "%s", menus[0].c_str());
            else
                wprintw(win, "%s", menus[i].c_str());
            if (i == selected)
                wattr_off(win, A_STANDOUT, NULL);

            if (i < menus.size() - 1)
                waddch(win, ' ');
        }

        wrefresh(win);
    }
} // namespace interface
