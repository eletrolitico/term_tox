#include "ui/requests.h"

constexpr int SPACE_LEFT{20};
constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_ESCAPE{27};

namespace ui
{
    Requests::Requests() : BaseWindow("Requests")
    {
    }

    void Requests::draw()
    {
        wclear(win);
        box(win, 0, 0);
        auto requests = t_hand->get_requests();

        if (requests.empty())
            mvwprintw(win, 2, SPACE_LEFT, "You have no requests!");

        for (size_t i = 0; i < requests.size(); ++i)
        {
            auto r = requests[i];

            if (selected_request == i)
                wattron(win, A_STANDOUT);
            mvwprintw(win, i, 2, "[%s] - %s", r.get_pub_key().c_str(), r.msg);
            wattroff(win, A_STANDOUT);
        }

        wrefresh(win);
    }

    void Requests::update(const int &ch)
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
        default:
            break;
        }
    }

    void Requests::do_go_up()
    {
        if (state == State::LIST)
        {
            if (selected_request > 0)
                --selected_request;
            else
                selected_request = t_hand->get_requests().size();
        }
    }

    void Requests::do_go_down()
    {
        if (state == State::LIST)
        {
            if (selected_request < t_hand->get_requests().size())
                ++selected_request;
            else
                selected_request = 0;
        }
    }

    void Requests::do_enter()
    {
        if (selected_request == 0)
        {
            switch (state)
            {
            case State::LIST:
                state = State::VIEWING_REQUEST;
                break;

            default:
                break;
            }
        }
    }

    void Requests::do_esc_key()
    {
        state = State::LIST;
    }
} // namespace ui
