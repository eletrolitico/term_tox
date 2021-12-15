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
        void update(const int& ch) override;

    private:
        uint8_t selected_friend = 0;
        bool adding_friend = false, tox_id_done = false;
        std::string adding_tox_id = "", adding_message = "";

        void do_go_up();
        void do_go_down();
        void do_enter();
        void do_erase();
        void do_esc_key();
        void do_default(const int& ch);
    };
}
#endif