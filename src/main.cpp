#include <iostream>
#include <tox/tox.h>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>

const std::vector<std::string> options = {"Add", "Del", "Contact", "Accept", "Help"};

size_t UpdateMove(int move, size_t chosen)
{
    if (move == KEY_BACKSPACE)
        exit(0);

    switch(move)
    {
        case KEY_UP:
            --chosen; 
            if (chosen < 0) chosen = 0;
            break;
        case KEY_DOWN:
            ++chosen;
            if (chosen >= options.size()) chosen = options.size() - 1;
            break;
        default:
            std::cout << "go up or down" << '\n';
            break;
    }

    return chosen;
}

int main()
{
    // TOX_ERR_NEW err_new;
    // Tox *tox = tox_new(NULL, &err_new);
    // if (err_new != TOX_ERR_NEW_OK)
    // {
    //     fprintf(stderr, "tox_new failed with error code %d\n", err_new);
    //     exit(1);
    // }

    //Start
    initscr();
    noecho();
    cbreak();

    //Screen size
    int Width, Height;
    getmaxyx(stdscr, Height, Width);  

    //Window
    WINDOW* menu = newwin(10, Width-10, Height-10, 5);
    box(menu, 0, 0);
    refresh();
    wrefresh(menu);

    //Arrow key
    keypad(menu, true);

    int move{ 0 };
    size_t chosen{ 0 };

    while(true)
    {
        for (size_t i = 0; i < options.size(); ++i)
        {
            if (i == chosen)
                wattron(menu, A_REVERSE);
            mvwprintw(menu, i + 1, 1, options[i].c_str());
            wattroff(menu, A_REVERSE);
        }

        move = wgetch(menu);
        chosen = UpdateMove(move, chosen);
        
        //printw("Option chosen: %s", options[chosen].c_str());
    }

    printw("Option chosen: %s", options[chosen].c_str());
    refresh();
    endwin();

    return 0;
}