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
        BaseWindow(const std::string &title) : title_(title)
        {
            win = newwin(height_, x_max_ - 2, start_y_, 1);
            box(win, 0, 0);
        }

        BaseWindow(const std::string &title, int x_max, int height, int start_y)
            : title_(title), cust_x_max_(x_max), cust_height_(height), cust_start_y_(start_y), has_cust_dims(true)
        {
            win = newwin(height, x_max - 2, start_y, 1);
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

        inline std::string getTitle() const
        {
            return title_;
        }

        inline static void set_dimensions(int x_max, int height, int start_y)
        {
            x_max_ = x_max;
            height_ = height;
            start_y_ = start_y;
        }

        inline int get_width()
        {
            if (has_cust_dims)
                return cust_x_max_ - 4;
            else
                return x_max_ - 4;
        }

        inline int get_height()
        {
            if (has_cust_dims)
                return cust_height_;
            else
                return height_;
        }

        virtual void on_focus() {}
        virtual void on_blur() {}

    protected:
        static Friend *talking_to_;
        WINDOW *win;

    private:
        std::string title_;
        static int x_max_, height_, start_y_;
        int cust_x_max_, cust_height_, cust_start_y_;
        bool has_cust_dims = false;
    };
}

#endif