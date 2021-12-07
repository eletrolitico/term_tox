#include <iostream>
#include <tox/tox.h>
#include <ncurses.h>

int main()
{
    TOX_ERR_NEW err_new;
    Tox *tox = tox_new(NULL, &err_new);
    if (err_new != TOX_ERR_NEW_OK)
    {
        fprintf(stderr, "tox_new failed with error code %d\n", err_new);
        exit(1);
    }

    initscr();
    printw("Hello World !!!");
    refresh();
    getch();
    endwin();

    return 0;
}