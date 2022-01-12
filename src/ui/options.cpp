#include "ui/options.h"

constexpr int SPACE_LEFT{20};
constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_ESCAPE{27};
constexpr int KEY_DELETE{127};

namespace ui
{
    Options::Options() : BaseWindow("Options")
    {
    }

    void Options::draw()
    {
        werase(win);
        box(win, 0, 0);

        switch (state)
        {
        case State::LIST:
            draw_list();
            break;
        case State::EDITING_OPTION:
            draw_option();
            break;
        }

        wrefresh(win);
    }

    void Options::draw_list()
    {

        for (size_t i = 0; i < options_.size(); ++i)
        {
            auto opt = options_[i];

            if (selected_option_ == i)
                wattron(win, A_STANDOUT);
            mvwprintw(win, i + 2, SPACE_LEFT, opt.c_str());
            wattroff(win, A_STANDOUT);
        }
    }

    void Options::draw_option()
    {
        mvwprintw(win, 2, 1, (options_[selected_option_] + ": " + typing_).c_str());
    }

    bool Options::update(const int &ch)
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
        case KEY_ESCAPE:
            do_esc_key();
            break;
        case KEY_BACKSPACE:
        case KEY_DELETE:
            do_backspace();
            break;
        default:
            do_default(ch);
            break;
        }

        return true;
    }

    void Options::do_go_up()
    {
        if (state == State::LIST && selected_option_ > 0)
            --selected_option_;
    }

    void Options::do_go_down()
    {
        if (state == State::LIST && selected_option_ < options_.size() - 1)
            ++selected_option_;
    }

    void Options::do_default(int ch)
    {
        if (state == State::EDITING_OPTION && isprint(ch))
            typing_ += (char)ch;
    }

    void Options::do_backspace()
    {
        if (typing_.size())
            typing_.pop_back();
    }

    void Options::do_enter()
    {
        switch (state)
        {
        case State::LIST:
            state = State::EDITING_OPTION;
            curs_set(1);
            break;
        case State::EDITING_OPTION:
            curs_set(0);
            save_option();
            break;

        default:
            break;
        }
    }

    void Options::save_option()
    {
        switch (selected_option_)
        {
        case 0:
            ToxHandler::get().set_name(typing_);
            break;
        case 1:
            ToxHandler::get().set_status_message(typing_);
            break;
        }
        state = State::LIST;
        typing_ = "";
    }

    void Options::do_esc_key()
    {
        curs_set(0);
        state = State::LIST;
        typing_ = "";
    }

} // namespace ui
