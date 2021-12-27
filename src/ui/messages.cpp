#include "ui/messages.h"

constexpr int SPACE_LEFT{20};
constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_ESCAPE{27};

namespace ui
{
    Messages::Messages() : BaseWindow("Messages")
    {
    }

    void Messages::draw()
    {
        wclear(win);
        box(win, 0, 0);

        switch (state)
        {
        case State::LIST:
            draw_list();
            break;
        case State::VIEWING_REQUEST:
            draw_request();
            break;
        }

        wrefresh(win);
    }

    void Messages::draw_list()
    {
        auto requests = t_hand->get_requests();

        if (requests.empty())
            mvwprintw(win, 2, SPACE_LEFT, "You have no requests!");

        for (size_t i = 0; i < requests.size(); ++i)
        {
            auto r = requests[i];

            if (selected_request == i)
                wattron(win, A_STANDOUT);
            mvwprintw(win, i + 2, SPACE_LEFT, "[%s] - %s", r.get_pub_key().c_str(), std::string(r.msg).substr(0, 20).c_str());
            wattroff(win, A_STANDOUT);
        }
    }

    void Messages::draw_request()
    {
        auto requests = t_hand->get_requests();
        auto r = requests[selected_request];

        mvwprintw(win, 3, SPACE_LEFT, "Request from: %s", r.get_pub_key().c_str());
        int next_y = draw_line_with_break(4, SPACE_LEFT, 20, "Message: " + std::string(r.msg));
        mvwprintw(win, next_y + 1, SPACE_LEFT, "<enter> to accept");
        mvwprintw(win, next_y + 2, SPACE_LEFT, "<del> to reject");
    }

    void Messages::update(const int &ch)
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

    void Messages::do_go_up()
    {
        if (state == State::LIST)
        {
            if (selected_request > 0)
                --selected_request;
            else
                selected_request = t_hand->get_requests().size();
        }
    }

    void Messages::do_go_down()
    {
        if (state == State::LIST)
        {
            if (selected_request < t_hand->get_requests().size())
                ++selected_request;
            else
                selected_request = 0;
        }
    }

    void Messages::do_enter()
    {
        auto requests = t_hand->get_requests();
        switch (state)
        {
        case State::LIST:
            if (requests.size())
                state = State::VIEWING_REQUEST;
            break;
        case State::VIEWING_REQUEST:
            t_hand->accept_request(requests[selected_request]);
            state = State::LIST;
            break;

        default:
            break;
        }
    }

    void Messages::do_esc_key()
    {
        state = State::LIST;
    }

    int Messages::draw_line_with_break(int ini_y, int ini_x, int width, const std::string &str)
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
} // namespace ui
