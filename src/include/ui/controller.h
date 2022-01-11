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
        Controller(const Controller&) = delete;
        
        static Controller *get_instance(ToxHandler *);
        void update(int ch);

        ~Controller();

    private:
        Controller();
        Controller(ToxHandler *);

        static Controller *main_;
        
        int y_max_, x_max_;
        std::unique_ptr<Menu> menu_;
        StatusBar *status_bar_;
        std::vector<BaseWindow *> windows_;
    };

}

#endif