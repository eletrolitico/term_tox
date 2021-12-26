#ifndef REQUESTS_H
#define REQUESTS_H

#include "ui/base_window.h"

namespace ui
{
    class Requests : public BaseWindow
    {
    public:
        Requests();
        ~Requests() {}

        void draw() override;
        void update(const int &ch) override;

    private:
        enum class State
        {
            LIST,
            VIEWING_REQUEST,
            ACCEPTED_REQUEST,
            DENIED_REQUEST
        };

        uint8_t selected_request{0};
        State state{State::LIST};

        void do_go_up();
        void do_go_down();
        void do_enter();
        void do_esc_key();
    };

} // namespace ui

#endif