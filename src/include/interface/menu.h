#ifndef MENU_H
#define MENU

#include <string>
#include <vector>
#include <ncurses.h>
#include "interface/content_window.h"

namespace interface
{
    class Menu
    {
    public:
        Menu(int xMax, int h);
        ContentWindow *getSelectedMenu(int ch);
        void draw();

        inline void addWindow(ContentWindow *win)
        {
            windows.push_back(win);
        }

        inline ContentWindow *getWindow(size_t i)
        {
            return windows[i];
        }

    private:
        WINDOW *win;
        std::vector<ContentWindow *> windows;
        uint8_t selected = 0, height, width;
    };
}

#endif
