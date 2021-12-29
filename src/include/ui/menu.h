#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "ui/base_window.h"

namespace ui
{
    class Menu
    {
    public:
        Menu(int xMax, int h);
        BaseWindow *get_selected_window(int ch);
        void draw();

        inline void add_window(BaseWindow *win)
        {
            windows.push_back(win);
        }

        inline BaseWindow *get_window(size_t i)
        {
            return windows[i];
        }

    private:
        WINDOW *win;
        std::vector<BaseWindow *> windows;
        uint8_t selected = 0, height, width;
    };
}

#endif
