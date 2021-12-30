#include "ui/options.h"

constexpr int SPACE_LEFT{20};
constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_ESCAPE{27};
constexpr int KEY_BACKSPACE_VSCODE{127};

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

        for (size_t i = 0; i < options.size(); ++i)
        {
            auto opt = options[i];

            if (selected_option == i)
                wattron(win, A_STANDOUT);
            mvwprintw(win, i + 2, SPACE_LEFT, opt.c_str());
            wattroff(win, A_STANDOUT);
        }
    }

    void Options::draw_option()
    {
        mvwprintw(win, 2, 1, (options[selected_option] + ": " + typing).c_str());
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
        case KEY_BACKSPACE_VSCODE:
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
        if (state == State::LIST && selected_option > 0)
            --selected_option;
    }

    void Options::do_go_down()
    {
        if (state == State::LIST && selected_option < options.size() - 1)
            ++selected_option;
    }

    void Options::do_default(int ch)
    {
        if (state == State::EDITING_OPTION && isprint(ch))
            typing += (char)ch;
    }

    void Options::do_backspace()
    {
        if (typing.size())
            typing.pop_back();
    }

    void Options::do_enter()
    {
        switch (state)
        {
        case State::LIST:
            state = State::EDITING_OPTION;
            break;
        case State::EDITING_OPTION:
            save_option();
            break;

        default:
            break;
        }
    }

    void Options::save_option()
    {
        switch (selected_option)
        {
        case 0:
            t_hand->set_name(typing);
            break;
        case 1:
            t_hand->set_status_message(typing);
            break;
        }
        state = State::LIST;
        typing = "";
    }

    void Options::do_esc_key()
    {
        state = State::LIST;
        typing = "";
    }

} // namespace ui
