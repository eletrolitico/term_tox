#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include <ncurses.h>
#include <string>
#include "tox_handler.h"

namespace ui
{
    class BaseWindow
    {
    public:
        BaseWindow(const std::string &title) : title(title)
        {
            win = newwin(height, xMax - 2, start_y, 1);
            box(win, 0, 0);
        }

        BaseWindow(const std::string &title, int xMax, int height, int start_y)
            : title(title), cust_xMax(xMax), cust_height(height), cust_start_y(start_y), hasCustDims(true)
        {
            win = newwin(height, xMax - 2, start_y, 1);
            box(win, 0, 0);
        }

        virtual ~BaseWindow() {}

        virtual void draw()
        {
            werase(win);
            box(win, 0, 0);
            wrefresh(win);
        }
        virtual bool update(const int &ch)
        {
            return true;
        }

        inline static void setToxHandler(ToxHandler *t)
        {
            t_hand = t;
        }

        inline std::string getTitle() const
        {
            return title;
        }

        inline static void setDims(int _xMax, int _height, int _start_y)
        {
            xMax = _xMax;
            height = _height;
            start_y = _start_y;
        }

        inline int get_width()
        {
            if (hasCustDims)
                return cust_xMax - 4;
            else
                return xMax - 4;
        }

        inline int get_height()
        {
            if (hasCustDims)
                return cust_height;
            else
                return height;
        }

    protected:
        static ToxHandler *t_hand;

        static Friend *talking_to;
        WINDOW *win;

    private:
        std::string title;
        static int xMax, height, start_y;
        int cust_xMax, cust_height, cust_start_y;
        bool hasCustDims = false;
    };
}

#endif