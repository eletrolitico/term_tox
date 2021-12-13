#ifndef FRIENDS_H
#define FRIENDS_H

#include "interface/content_window.h"

namespace interface
{

    class Friends : public ContentWindow
    {
    public:
        Friends(int xMax, int height, int start_y);
        ~Friends() {}

        void draw();
        void update(int ch);
    };
}
#endif