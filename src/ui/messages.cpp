#include "ui/messages.h"

#include <ranges>

constexpr int KEY_END_LINE{'\n'};
constexpr int KEY_DELETE{127};

namespace ui
{
    Messages::Messages() : Window("Messages")
    {
    }

    void Messages::draw()
    {
        werase(win);
        box(win, 0, 0);

        if (ui::WindowInfo::get().talking_to_ == nullptr)
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
        auto msgs = ToxHandler::get().get_messages(ui::WindowInfo::get().talking_to_->friend_num_);
        std::string tmp = "message: " + typing_;
        int msgLines = tmp.size() / (get_width() - 2) + 1;

        int msgPos = get_height() - 3 - msgLines;

        for (auto msg : msgs | std::views::reverse)
        {
            auto prefix = msg.first == MESSAGE::SENT ? "Voce: " : std::string(ui::WindowInfo::get().talking_to_->name_) + ": ";
            msgPos -= draw_line_with_break(msgPos, 2, get_width() - 2, prefix + msg.second) + 1;
            if (msgPos <= 1)
                break;
        }

        draw_line_with_break(get_height() - 2, 2, get_width() - 2, "message: " + typing_);
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
        case KEY_BACKSPACE:
        case KEY_DELETE:
            do_erase();
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
            typing_ += (char)ch;
    }

    void Messages::do_erase()
    {
        if (typing_.size())
            typing_.pop_back();
    }

    void Messages::do_go_up()
    {
        ++scroll_;
    }

    void Messages::do_go_down()
    {
        if (scroll_ > 0)
            --scroll_;
    }

    void Messages::do_enter()
    {
        if (typing_.size() && ui::WindowInfo::get().talking_to_ != nullptr)
        {
            ToxHandler::get().send_message(ui::WindowInfo::get().talking_to_->friend_num_, typing_);
            typing_ = "";
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
} 
