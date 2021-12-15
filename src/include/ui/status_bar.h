#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include "ui/base_window.h"

namespace ui
{
    class StatusBar : public BaseWindow
    {
    public:
        StatusBar(int, int, int);
        void update();
        void draw();
    };
}

#endif