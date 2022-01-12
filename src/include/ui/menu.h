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

        Window* get_selected_window(int key);
        
        void draw();

        void add_window(Window* window);
        Window* get_window(size_t pos);

        ~Menu();
    private:
        WINDOW* win_;
        std::vector< Window* > windows_;
        uint8_t selected_ = 0, height_, width_;
    };
}

#endif
