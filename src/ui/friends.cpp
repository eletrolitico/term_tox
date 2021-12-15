#include "ui/friends.h"

#define SPACE_LEFT 20

namespace ui
{
    Friends::Friends() : BaseWindow("Friends")
    {
    }

    void Friends::draw()
    {
        wclear(win);
        box(win, 0, 0);

        auto friends = t_hand->get_friends();

        if (friends.size() == 0 && !adding_friend)
            mvwprintw(win, 2, SPACE_LEFT, "You have no friends!");

        if (selected_friend == 0 && !adding_friend)
            wattron(win, A_STANDOUT);
        mvwprintw(win, 2, 2, "Add friend");
        wattroff(win, A_STANDOUT);

        for (size_t i = 0; i < friends.size(); ++i)
        {
            auto f = friends[i];

            if (selected_friend == i + 1)
                wattron(win, A_STANDOUT);
            mvwprintw(win, i, 2, "%s [%s] - %s", f->name, ToxHandler::connection_enum2text(f->connection), f->status_message);
            wattroff(win, A_STANDOUT);
        }

        if (adding_friend)
        {
            mvwprintw(win, 2, SPACE_LEFT, "Tox ID: %s", adding_tox_id.c_str());
            if (tox_id_done)
            {
                mvwprintw(win, 3, SPACE_LEFT, "message: %s", adding_message.c_str());
            }
        }

        wrefresh(win);
    }

    void Friends::update(int ch)
    {
        switch (ch)
        {
        case KEY_UP:
            if (!adding_friend)
            {
                if (selected_friend > 0)
                    --selected_friend;
                else
                    selected_friend = t_hand->get_friends().size();
            }
            break;

        case KEY_DOWN:
            if (!adding_friend)
            {
                if (selected_friend < t_hand->get_friends().size())
                    ++selected_friend;
                else
                    selected_friend = 0;
            }
            break;

        case '\n':
            if (selected_friend == 0)
            {
                if (adding_friend)
                {
                    if (tox_id_done)
                    {
                        adding_friend = false;
                        curs_set(0);
                    }
                    else
                    {
                        tox_id_done = true;
                    }
                }
                else
                {
                    adding_friend = true;
                    curs_set(1);
                }
            }
            break;

        case 127:
        case KEY_BACKSPACE: // backspace
            if (tox_id_done)
            {
                if (adding_message.size())
                    adding_message.pop_back();
            }
            else
            {
                if (adding_tox_id.size())
                    adding_tox_id.pop_back();
            }
            break;

        case 27: // escape
            adding_tox_id = "";
            adding_message = "";
            adding_friend = false;
            tox_id_done = false;
            curs_set(0);

            break;

        default:
            if (isprint(ch) && adding_friend)
            {
                if (tox_id_done)
                    adding_message += (char)ch;
                else
                    adding_tox_id += (char)ch;
            }
        }
    }
}