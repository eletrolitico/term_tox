#ifndef REQUESTS_H
#define REQUESTS_H

#include "ui/base_window.h"

namespace ui
{
    class Requests : public Window
    {
    public:
        Requests();
        ~Requests() {}

        void draw() override;
        bool update(const int &ch) override;

    private:
        enum class State
        {
            LIST,
            VIEWING_REQUEST
        };

        uint8_t selected_request_{0};
        State state_{State::LIST};

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