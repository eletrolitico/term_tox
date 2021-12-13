#include "interface/friends.h"

namespace interface
{
    Friends::Friends(int xMax, int height, int start_y) : ContentWindow(xMax, height, start_y)
    {
    }

    void Friends::draw()
    {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "You have no friends!");
        wrefresh(win);
    }

    void Friends::update(int ch)
    {
    }
}