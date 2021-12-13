#include "interface/content_window.h"

namespace interface
{
    ToxHandler *ContentWindow::t_hand;

    ContentWindow::ContentWindow(int xMax, int height, int start_y)
    {
        win = newwin(height, xMax - 2, start_y, 1);
        box(win, 0, 0);
    }

    ContentWindow::~ContentWindow()
    {
    }

    void ContentWindow::update(int ch)
    {
    }

    void ContentWindow::draw()
    {
    }

} // namespace interface
