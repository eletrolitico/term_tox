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

void draw_borders(WINDOW* win, int start_y, int start_x, int end_y, int end_x) 
{ 
    // 4 corners 
    mvwprintw(win, start_y, start_x, "+"); 
    mvwprintw(win, end_y - 1, start_x, "+"); 
    mvwprintw(win, start_y, end_x - 1, "+"); 
    mvwprintw(win, end_y - 1, end_x - 1, "+"); 
    
    // sides 
    for (int i = start_y + 1; i < (end_y - 1); i++) 
    { 
        mvwprintw(win, i, start_x, "|"); 
        mvwprintw(win, i, end_x - 1, "|"); 
    } 
    
    // top and bottom 
    for (int i = start_x; i < (end_x - 1); i++) 
    { 
        mvwprintw(win, start_y, i, "-"); 
        mvwprintw(win, end_y - 1, i, "-"); 
    } 
}

int main()
{   
    StartNCurses();

    //Screen size
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);  

    //Window
    WINDOW* win = newwin(yMax, xMax, 0, 0);
    //box(win, 0, 0);

    draw_borders(win, 0, 0, yMax / 6, xMax / 2);
    draw_borders(win, yMax / 8, 0, yMax / 2, xMax / 2);

    //Arrow key
    keypad(win, true);

    std::vector<ncurses::Menu> menus
    {
        ncurses::Menu(0, 1, "Menu"), // title
        ncurses::Menu(yMax / 16, 5, "Add"),
        ncurses::Menu(yMax / 16, 9, "Del"),
        ncurses::Menu(yMax / 16, 13, "Invite"),
        ncurses::Menu(yMax / 16, 21, "Contact")
    };

    ncurses::MenuBar menu_bar{win, menus};

    size_t selected_menu{1};
    menu_bar.display(selected_menu);

    while(int ch = wgetch(win))
    {
        switch(ch)
        {
            case KEY_LEFT:
                if (selected_menu > 1)
                    --selected_menu;
            break;

            case KEY_RIGHT:
                if (selected_menu + 1 < menus.size())
                    ++selected_menu;
            break;
        }

        menu_bar.display(selected_menu);
    }

    endwin();

    return 0;
}