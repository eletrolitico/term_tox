#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include <ncurses.h>
#include <string>
#include "tox_handler.h"
#include "window_info.h"

namespace ui
{
    class Window
    {
    public:
        Window(const std::string &title) : title_(title)
        {
            win = newwin(get_height(), get_x_max() - 2, get_start_y(), 1);
            box(win, 0, 0);
        }

        Window(const std::string &title, int x_max, int height, int start_y)
            : title_(title), cust_x_max_(x_max), cust_height_(height), cust_start_y_(start_y), has_cust_dims(true)
        {
            win = newwin(height, x_max - 2, start_y, 1);
            box(win, 0, 0);
        }

        virtual ~Window() {}

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

        inline int get_x_max() const
        {
            return ui::WindowInfo::get().x_max_;
        }

        inline int get_height() const
        {
            if (has_cust_dims)
                return cust_height_;
            else
                return ui::WindowInfo::get().height_;
        }

        inline int get_start_y() const
        {
            return ui::WindowInfo::get().start_y_;
        }

        inline int get_width()
        {
            if (has_cust_dims)
                return cust_x_max_ - 4;
            else
                return get_x_max() - 4;
        }

        virtual void on_focus() {}
        virtual void on_blur() {}

    protected:
        WINDOW *win;

    private:
        std::string title_;
        int cust_x_max_, cust_height_, cust_start_y_;
        bool has_cust_dims = false;
    };
}



#endif