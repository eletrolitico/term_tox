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
        Menu(int x_max_x, int h);
        BaseWindow *get_selected_window(int ch);
        void draw();

        inline void add_window(BaseWindow *win)
        {
            windows_.push_back(win);
        }

        inline BaseWindow *get_window(size_t i)
        {
            return windows_[i];
        }

    private:
        WINDOW *win_;
        std::vector<BaseWindow *> windows_;
        uint8_t selected_ = 0, height_, width_;
    };
}

#endif
