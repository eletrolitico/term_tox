#ifndef INTERFACE_H
#define INTERFACE_H

#include <memory>
#include "interface/menu.h"
#include "interface/status_bar.h"

#include "tox_handler.h"

namespace interface
{
    class Main
    {
    public:
        static Main *getInstance(ToxHandler *);
        void updateInterface(int ch);

        ~Main();

    private:
        static Main *_main;
        Main(ToxHandler *);

        int yMax, xMax;
        std::unique_ptr<Menu> menu;
        std::vector<ContentWindow *> windows;
        StatusBar *status_bar;
    };

}

#endif