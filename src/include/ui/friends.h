#ifndef FRIENDS_H
#define FRIENDS_H

#include "ui/base_window.h"

namespace ui
{

    class Friends : public BaseWindow
    {
    public:
        Friends();
        ~Friends() {}

        void draw() override;
        void update(const int &ch) override;

    private:
        enum class State
        {
            LIST,
            TYPING_TOX_ID,
            TYPING_MESSAGE,
            FRIEND_ADDED
        };

        uint8_t selected_friend{0};
        State state{State::LIST};
        std::string adding_tox_id{""}, adding_message{""}, frnd_added_msg{""};

        // Interface drawing
        void draw_list();
        int draw_line_with_break(int ini_y, int ini_x, int width, const std::string &msg);

        // Actions
        void do_go_up();
        void do_go_down();
        void do_enter();
        void do_erase();
        void do_esc_key();
        void do_default(const int &ch);
    };
}
#endif