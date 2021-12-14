#ifndef CONTENT_WINDOW_H
#define CONTENT_WINDOW_H

#include <ncurses.h>
#include <string>
#include "tox_handler.h"

namespace interface
{
    class ContentWindow
    {
    public:
        ContentWindow(const std::string &title) : title(title)
        {
            win = newwin(height, xMax - 2, start_y, 1);
            box(win, 0, 0);
        }

        ContentWindow(const std::string &title, int xMax, int height, int start_y)
            : title(title), cust_xMax(xMax), cust_height(height), cust_start_y(start_y), hasCustDims(true)
        {
            win = newwin(height, xMax - 2, start_y, 1);
            box(win, 0, 0);
        }

        virtual ~ContentWindow() {}

        virtual void draw()
        {
            wclear(win);
            box(win, 0, 0);
            wrefresh(win);
        }
        virtual void update(int ch) {}

        inline static void setToxHandler(ToxHandler *t)
        {
            t_hand = t;
        }

        inline std::string getTitle()
        {
            return title;
        }

        inline static void setDims(int _xMax, int _height, int _start_y)
        {
            xMax = _xMax;
            height = _height;
            start_y = _start_y;
        }

    protected:
        static ToxHandler *t_hand;
        static int xMax, height, start_y;

        WINDOW *win;
        std::string title;
        int cust_xMax, cust_height, cust_start_y;
        bool hasCustDims = false;
    };

#ifdef CONTENT_WINDOW_IMPL
    ToxHandler *ContentWindow::t_hand;
    int ContentWindow::xMax, ContentWindow::height, ContentWindow::start_y;
#endif

}

#endif