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
        bool update(const int &ch) override;

    private:
        std::string typing{""};
        uint32_t scroll{0};

        void do_go_up();
        void do_go_down();
        void do_enter();
        void do_default(int ch);

        void draw_no_friend();
        void draw_messages();

        int draw_line_with_break(int ini_y, int ini_x, int width, const std::string &msg);
    };
} // namespace ui

#endif