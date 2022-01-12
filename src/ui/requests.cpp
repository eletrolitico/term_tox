#include "ui/requests.h"

constexpr int SPACE_LEFT{20};
constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_ESCAPE{27};

namespace ui
{
    Requests::Requests() : Window("Requests")
    {
    }

    void Requests::draw()
    {
        werase(win);
        box(win, 0, 0);

        switch (state_)
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

    void Requests::draw_list()
    {
        auto requests = ToxHandler::get().get_requests();

        if (requests.empty())
            mvwprintw(win, 2, SPACE_LEFT, "You have no requests!");

        for (size_t i = 0; i < requests.size(); ++i)
        {
            auto r = requests[i];

            if (selected_request_ == i)
                wattron(win, A_STANDOUT);
            mvwprintw(win, i + 2, SPACE_LEFT, "[%s] - %s", r.get_pub_key().c_str(), std::string(r.msg_).substr(0, 20).c_str());
            wattroff(win, A_STANDOUT);
        }
    }

    void Requests::draw_request()
    {
        auto requests = ToxHandler::get().get_requests();
        auto r = requests[selected_request_];

        mvwprintw(win, 3, SPACE_LEFT, "Request from: %s", r.get_pub_key().c_str());
        int next_y = draw_line_with_break(4, SPACE_LEFT, 20, "Message: " + std::string(r.msg_));
        mvwprintw(win, next_y + 1, SPACE_LEFT, "<enter> to accept");
        mvwprintw(win, next_y + 2, SPACE_LEFT, "<del> to reject");
    }

    bool Requests::update(const int &ch)
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
        return true;
    }

    void Requests::do_go_up()
    {
        if (state_ == State::LIST)
        {
            if (selected_request_ > 0)
                --selected_request_;
            else
                selected_request_ = ToxHandler::get().get_requests().size();
        }
    }

    void Requests::do_go_down()
    {
        if (state_ == State::LIST)
        {
            if (selected_request_ < ToxHandler::get().get_requests().size())
                ++selected_request_;
            else
                selected_request_ = 0;
        }
    }

    void Requests::do_enter()
    {
        auto requests = ToxHandler::get().get_requests();
        switch (state_)
        {
        case State::LIST:
            if (requests.size())
                state_ = State::VIEWING_REQUEST;
            break;
        case State::VIEWING_REQUEST:
            ToxHandler::get().accept_request(requests[selected_request_]);
            state_ = State::LIST;
            break;

        default:
            break;
        }
    }

    void Requests::do_esc_key()
    {
        state_ = State::LIST;
    }

    int Requests::draw_line_with_break(int ini_y, int ini_x, int width, const std::string &str)
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
