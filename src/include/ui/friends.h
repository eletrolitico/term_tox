#ifndef FRIENDS_H
#define FRIENDS_H

#include "ui/base_window.h"

namespace ui
{
    enum class State
    {
        LIST,
        TYPING_TOX_ID,
        TYPING_MESSAGE,
        FRIEND_ADDED
    };

    class Friends : public BaseWindow
    {
    public:
        Friends();
        ~Friends() {}

        void draw() override;
        void update(const int &ch) override;

    private:
        uint8_t selected_friend{0};
        State state{State::LIST};
        std::string adding_tox_id{""}, adding_message{""}, frnd_added_msg{""};

        // Interface drawing
        void draw_list();

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