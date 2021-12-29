#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include "ui/menu.h"
#include "ui/status_bar.h"

#include "tox_handler.h"

namespace ui
{
    class Controller
    {
    public:
        static Controller *get_instance(ToxHandler *);
        void update(int ch);

        ~Controller();

    private:
        static Controller *_main;
        Controller(ToxHandler *);

        int yMax, xMax;
        std::unique_ptr<Menu> menu;
        StatusBar *status_bar;
        std::vector<BaseWindow *> windows;
    };

}

#endif