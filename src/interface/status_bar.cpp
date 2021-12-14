#include "interface/status_bar.h"

namespace interface
{
    StatusBar::StatusBar(int a, int b, int c) : ContentWindow(a, b, c)
    {
    }

    void StatusBar::draw()
    {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Status aqui!");

        wrefresh(win);
    }
}