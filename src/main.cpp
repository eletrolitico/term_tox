#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>
#include "menu.h"

void StartNCurses()
{
    //Start
    initscr();
    raw();
    noecho();
    cbreak();
    curs_set(0);
}

int main()
{   
    StartNCurses();

    //Screen size
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);  

    //Window
    WINDOW* win = newwin(yMax / 2, xMax / 4, yMax / 4, 0);
    box(win, 0, 0);
    
    //Arrow key
    keypad(win, true);

    std::vector<ncurses::Menu> menus
    {
        ncurses::Menu(0, 2, "Options"), // title
        ncurses::Menu(2, 2, "Add"),
        ncurses::Menu(4, 2, "Del"),
        ncurses::Menu(6, 2, "Invite"),
        ncurses::Menu(8, 2, "Contact")
    };

    ncurses::MenuBar menu_bar{win, menus};

    size_t selected_menu{1};
    menu_bar.display(selected_menu);

    while(int ch = wgetch(win))
    {
        switch(ch)
        {
            case KEY_UP:
                if (selected_menu > 1)
                    --selected_menu;
            break;

            case KEY_DOWN:
                if (selected_menu + 1 < menus.size())
                    ++selected_menu;
            break;
        }

        menu_bar.display(selected_menu);
    }

    endwin();

    return 0;
}