#ifndef OPTIONS_H
#define OPTIONS_H

#include "ui/base_window.h"

namespace ui
{
    class Options : public BaseWindow
    {
    public:
        Options();
        ~Options() {}

        void draw() override;
        bool update(const int &ch) override;

    private:
        enum class State
        {
            LIST,
            EDITING_OPTION
        };

        uint8_t selected_option_{0};
        State state{State::LIST};
        std::string typing_{""};

        const std::vector<std::string> options_ = {
            "Name",
            "Status message",
        };

        void do_go_up();
        void do_go_down();
        void do_enter();
        void do_esc_key();
        void do_default(int ch);
        void do_backspace();

        void draw_list();
        void draw_option();

        void save_option();
    };
} // namespace ui

#endif