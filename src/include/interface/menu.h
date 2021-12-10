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
        Menu(int h, int w, int y, int x, std::vector<std::string> menus);
        int getSelectedMenu(int ch);

    private:
        WINDOW *win;
        std::vector<std::string> menus;
        uint8_t selected = 0, heigh, width;
        void draw();
    };
}

#endif
