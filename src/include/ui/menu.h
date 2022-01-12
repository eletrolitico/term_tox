#ifndef MENU_H
#define MENU_H

#include <vector>
#include <memory>
#include <ncurses.h>

#include "ui/base_window.h"

namespace ui
{
    class Menu
    {
    public:
        Menu(int x_max, int h);

        BaseWindow* get_selected_window(int key);
        
        void draw();

        void add_window(BaseWindow* window);
        BaseWindow* get_window(size_t pos);

    private:
        std::shared_ptr<WINDOW> win_;
        std::vector< BaseWindow* > windows_;
        uint8_t selected_ = 0, height_, width_;
    };
}

#endif
