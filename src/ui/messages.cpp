#include "ui/messages.h"

constexpr int KEY_END_LINE{'\n'};

namespace ui
{
    Messages::Messages() : BaseWindow("Messages")
    {
    }

    void Messages::draw()
    {
        werase(win);
        box(win, 0, 0);

        if (talking_to == nullptr)
        {
            draw_no_friend();
        }
        else
        {
            draw_messages();
        }

        wrefresh(win);
    }

    void Messages::draw_messages()
    {
        auto msgs = t_hand->get_messages(talking_to->friend_num);
        int msgLines = ("message: " + typing).size() / (get_width() - 2) + 1;

        int msgPos = get_height() - 3 - msgLines;

        if (msgs.size())
            for (size_t i = msgs.size() - 1; i >= 0; --i)
            {
                auto prefix = msgs[i].first == MESSAGE::SENT ? "Voce: " : std::string(talking_to->name) + ": ";
                msgPos -= draw_line_with_break(msgPos, 2, get_width() - 2, prefix + msgs[i].second) + 1;
                if (msgPos <= 1)
                    break;
            }

        draw_line_with_break(get_height() - 2, 2, get_width() - 2, "message: " + typing);
    }

    void Messages::draw_no_friend()
    {
        draw_line_with_break((get_height() - 2) / 2, 4, get_width() - 4, "Select a friend first");
    }

    bool Messages::update(const int &ch)
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
        default:
            do_default(ch);
            break;
        }

        return true;
    }
    void Messages::do_default(int ch)
    {
        if (isprint(ch))
            typing += (char)ch;
    }

    void Messages::do_go_up()
    {
        ++scroll;
    }

    void Messages::do_go_down()
    {
        if (scroll > 0)
            --scroll;
    }

    void Messages::do_enter()
    {
        if (typing.size() && talking_to != nullptr)
        {
            t_hand->send_message(talking_to->friend_num, typing);
            typing = "";
        }
    }

    int Messages::draw_line_with_break(int ini_y, int ini_x, int width, const std::string &str)
    {
        unsigned int stride = width - 2;

        size_t lines = str.size() / stride + 1;

        for (size_t i = 0; i < lines; ++i)
        {
            auto tmp = str.substr(i * stride, stride);
            mvwprintw(win, ini_y + (i - lines + 1), ini_x, tmp.c_str());
        }

        return lines;
    }
} // namespace ui
