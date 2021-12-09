#include <vector>
#include <string>
#include <memory>
#include <ncurses.h>

#ifndef MENU_H
#define MENU

namespace ncurses {
    class Menu
    {
    public:
        int x;
        int y;
        std::string title;
        Menu(int x_, int y_, std::string s) 
                : x{x_}, y{y_}, title{s} {}
    };

    class MenuBar
    {
    private:
        WINDOW* win; 
        std::vector<Menu> menus;
    public:
        MenuBar(WINDOW* w, std::vector<Menu> m) 
            : win{w}, menus{m} 
        {
        }

        void display(size_t selected)
        {
            for (size_t i = 0; i < menus.size(); ++i)
            {
                if (i == selected)
                    wattron(win, A_STANDOUT);
                mvwprintw(win, menus[i].x, menus[i].y, menus[i].title.c_str());
                wattroff(win, A_STANDOUT);
            }   
        }

    };
}

#endif
