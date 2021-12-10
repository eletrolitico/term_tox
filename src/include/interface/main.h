#ifndef INTERFACE_H
#define INTERFACE_H

#include <memory>
#include "interface/menu.h"

namespace interface
{
    class Main
    {
    public:
        static Main *init();
        void updateInterface(int ch);

    private:
        Main() {}

        int yMax, xMax;
        std::unique_ptr<Menu> menu;
        static Main *_main;
    };

}

#endif