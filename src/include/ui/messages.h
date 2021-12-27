#ifndef MESSAGES_H
#define MESSAGES_H

#include "ui/base_window.h"

namespace ui
{
    class Messages : public BaseWindow
    {
    public:
        Messages();
        ~Messages() {}

        void draw() override;
        void update(const int &ch) override;

    private:
        enum class State
        {
            LIST,
            VIEWING_REQUEST
        };

        uint8_t selected_request{0};
        State state{State::LIST};

        void do_go_up();
        void do_go_down();
        void do_enter();
        void do_esc_key();

        void draw_list();
        void draw_request();

        int draw_line_with_break(int ini_y, int ini_x, int width, const std::string &msg);
    };
} // namespace ui

#endif