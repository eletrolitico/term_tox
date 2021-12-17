#include "ui/friends.h"

constexpr int SPACE_LEFT{20};
constexpr int KEY_ESCAPE{27};
constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_BACKSPACE_VSCODE{127};

namespace ui
{
    Friends::Friends() : BaseWindow("Friends")
    {
    }

    void Friends::draw()
    {
        wclear(win);
        box(win, 0, 0);

        switch (state)
        {
        case State::LIST:
            draw_list();
            break;

        case State::TYPING_TOX_ID:
            mvwprintw(win, 2, SPACE_LEFT, "Tox ID: %s", adding_tox_id.c_str());
            break;

        case State::TYPING_MESSAGE:
            mvwprintw(win, 2, SPACE_LEFT, "Tox ID: %s", adding_tox_id.c_str());
            mvwprintw(win, 3, SPACE_LEFT, "message: %s", adding_message.c_str());
            break;

        case State::FRIEND_ADDED:
            mvwprintw(win, 3, SPACE_LEFT, frnd_added_msg.c_str());
            mvwprintw(win, 4, SPACE_LEFT, "press <esc> to continue");
            break;
        }

        wrefresh(win);
    }

    void Friends::draw_list()
    {
        auto friends = t_hand->get_friends();

        if (friends.empty())
            mvwprintw(win, 2, SPACE_LEFT, "You have no friends!");

        if (selected_friend == 0)
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
    }

    void Friends::update(const int &ch)
    {
        switch (ch)
        {
        case KEY_UP:
            do_go_up();
            break;
        case KEY_DOWN:
            do_go_down();
            break;
        case KEY_END_LINE:
            do_enter();
            break;
        case KEY_BACKSPACE_VSCODE:
        case KEY_BACKSPACE:
            do_erase();
            break;
        case KEY_ESCAPE:
            do_esc_key();
            break;
        default:
            do_default(ch);
        }
    }

    void Friends::do_go_up()
    {
        if (state == State::LIST)
        {
            if (selected_friend > 0)
                --selected_friend;
            else
                selected_friend = t_hand->get_friends().size();
        }
    }

    void Friends::do_go_down()
    {
        if (state == State::LIST)
        {
            if (selected_friend < t_hand->get_friends().size())
                ++selected_friend;
            else
                selected_friend = 0;
        }
    }

    void Friends::do_enter()
    {
        if (selected_friend == 0)
        {
            switch (state)
            {
            case State::LIST:
                curs_set(1);
                state = State::TYPING_TOX_ID;
                break;

            case State::TYPING_TOX_ID:
                state = State::TYPING_MESSAGE;
                break;

            case State::TYPING_MESSAGE:
                auto err = t_hand->add_friend(adding_tox_id, adding_message);
                frnd_added_msg = std::string(ToxHandler::add_friend_err_enum2text(err));
                curs_set(0);
                state = State::FRIEND_ADDED;
                break;
            }
        }
    }

    void Friends::do_erase()
    {
        if (state == State::TYPING_TOX_ID && adding_tox_id.size())
            adding_tox_id.pop_back();

        if (state == State::TYPING_MESSAGE && adding_message.size())
            adding_message.pop_back();
    }

    void Friends::do_esc_key()
    {
        adding_tox_id = "";
        adding_message = "";
        state = State::LIST;
        curs_set(0);
    }

    void Friends::do_default(const int &ch)
    {
        if (isprint(ch))
        {
            if (state == State::TYPING_TOX_ID)
                adding_tox_id += (char)ch;

            else if (state == State::TYPING_MESSAGE)
                adding_message += (char)ch;
        }
    }
}