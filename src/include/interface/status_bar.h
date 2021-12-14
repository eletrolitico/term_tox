#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include "interface/content_window.h"

namespace interface
{
    class StatusBar : public ContentWindow
    {
    public:
        StatusBar(int, int, int);
        void update();
        void draw();
    };
}

#endif