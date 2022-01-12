#include "ui/friends.h"

constexpr int SPACE_LEFT{20};
constexpr int KEY_ESCAPE{27};
constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_DELETE{127};

namespace ui
{
    Friends::Friends() : BaseWindow("Friends")
    {
    }

    void Friends::draw()
    {
        int next_pos;

        werase(win);
        box(win, 0, 0);

        switch (state_)
        {
        case State::LIST:
            draw_list();
            break;

        case State::TYPING_TOX_ID:
            draw_line_with_break(2, 4, 20, "Tox ID: " + adding_tox_id_);
            break;

        case State::TYPING_MESSAGE:
            next_pos = draw_line_with_break(2, 4, 20, "Tox ID: " + adding_tox_id_);
            draw_line_with_break(next_pos, 4, 20, "message: " + adding_message_);
            break;

        case State::FRIEND_ADDED:
            mvwprintw(win, 3, SPACE_LEFT, frnd_added_msg_.c_str());
            mvwprintw(win, 4, SPACE_LEFT, "press <esc> to continue");
            break;
        }

        wrefresh(win);
    }

    void Friends::draw_list()
    {
        auto friends = ToxHandler::get().get_friends();

        if (friends.empty())
            mvwprintw(win, 2, SPACE_LEFT, "You have no friends!");

        if (selected_friend_ == 0)
            wattron(win, A_STANDOUT);
        mvwprintw(win, 2, 2, "Add friend");
        wattroff(win, A_STANDOUT);

        for (size_t i = 0; i < friends.size(); ++i)
        {
            auto f = friends[i];

            if (selected_friend_ == i + 1)
                wattron(win, A_STANDOUT);
            mvwprintw(win, 4 + i, 2, "%s [%s] - %s", f->name_, ToxHandler::connection_enum2text(f->connection_), f->status_message_);
            wattroff(win, A_STANDOUT);
        }
    }

    int Friends::draw_line_with_break(int ini_y, int ini_x, int width, const std::string &str)
    {
        unsigned int stride = get_width() - SPACE_LEFT, offset = 0;

        if (str.size() > stride)
        {
            while (offset < str.length())
            {
                auto tmp = str.substr(offset, stride);
                mvwprintw(win, ini_y, SPACE_LEFT, tmp.c_str());
                offset += stride;
                ++ini_y;
            }
            return ini_y;
        }

        mvwprintw(win, ini_y, SPACE_LEFT, str.c_str());
        return ini_y + 1;
    }

    bool Friends::update(const int &ch)
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
        case KEY_DELETE:
        case KEY_BACKSPACE:
            do_erase();
            break;
        case KEY_ESCAPE:
            do_esc_key();
            break;
        default:
            do_default(ch);
        }

        return true;
    }

    void Friends::do_go_up()
    {
        if (state_ == State::LIST)
        {
            if (selected_friend_ > 0)
                --selected_friend_;
            else
                selected_friend_ = ToxHandler::get().get_friends().size();
        }
    }

    void Friends::do_go_down()
    {
        if (state_ == State::LIST)
        {
            if (selected_friend_ < ToxHandler::get().get_friends().size())
                ++selected_friend_;
            else
                selected_friend_ = 0;
        }
    }

    void Friends::do_enter()
    {
        switch (state_)
        {

        case State::LIST:
            if (selected_friend_ == 0)
            {
                curs_set(1);
                state_ = State::TYPING_TOX_ID;
            }
            else
            {
                talking_to_ = ToxHandler::get().get_friends()[selected_friend_ - 1];
            }
            break;

        case State::TYPING_TOX_ID:
            state_ = State::TYPING_MESSAGE;
            break;

        case State::TYPING_MESSAGE:
        {
            auto err = ToxHandler::get().add_friend(adding_tox_id_, adding_message_);
            frnd_added_msg_ = std::string(ToxHandler::add_friend_err_enum2text(err));
            curs_set(0);
            state_ = State::FRIEND_ADDED;
            break;
        }
        case State::FRIEND_ADDED:
            break;
        }
    }

    void Friends::do_erase()
    {
        if (state_ == State::TYPING_TOX_ID && adding_tox_id_.size())
            adding_tox_id_.pop_back();

        if (state_ == State::TYPING_MESSAGE && adding_message_.size())
            adding_message_.pop_back();
    }

    void Friends::do_esc_key()
    {
        adding_tox_id_ = "";
        adding_message_ = "";
        state_ = State::LIST;
        curs_set(0);
    }

    void Friends::do_default(const int &ch)
    {
        if (isprint(ch))
        {
            if (state_ == State::TYPING_TOX_ID)
                adding_tox_id_ += (char)ch;

            else if (state_ == State::TYPING_MESSAGE)
                adding_message_ += (char)ch;
        }
    }
}