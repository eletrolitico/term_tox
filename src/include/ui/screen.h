#ifndef SCREEN_H
#define SCREEN_H

#include <memory>

#include "ui/menu.h"
#include "ui/status_bar.h"

#include "tox_handler.h"

namespace ui
{
    class Screen
    {
    public:
        static Screen& get();
        
        void update(int ch);
        
        ~Screen();
    
    private:
        Screen();
        Screen(const Screen&) = delete;
        void operator=(const Screen&) = delete;
    
        std::unique_ptr<Menu> menu_;
        std::unique_ptr<StatusBar> status_bar_;
    };
}
#endif