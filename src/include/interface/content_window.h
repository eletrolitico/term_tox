#ifndef CONTENT_WINDOW_H
#define CONTENT_WINDOW_H

#include <ncurses.h>
#include "tox_handler.h"

namespace interface
{
    class ContentWindow
    {
    public:
        ContentWindow(int xMax, int height, int start_y);
        virtual ~ContentWindow();

        virtual void draw();
        virtual void update(int ch);

        inline static void setToxHandler(ToxHandler *t)
        {
            t_hand = t;
        }

    protected:
        WINDOW *win;
        static ToxHandler *t_hand;
    };

}

#endif