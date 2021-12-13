#include <string>
#include <vector>
#include <ncurses.h>

#ifndef MENU_H
#define MENU

namespace interface
{
    class Menu
    {
    public:
        Menu(int xMax, int h, std::vector<std::string> menus);
        int getSelectedMenu(int ch);
        void draw();

    private:
        WINDOW *win;
        std::vector<std::string> menus;
        uint8_t selected = 0, height, width;
    };
}

#endif
