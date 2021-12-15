#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>
#include <memory>

#include "ui/controller.h"
#include "tox_handler.h"

// declaração de variáveis estáticas da BaseWindow
ToxHandler *ui::BaseWindow::t_hand;
int ui::BaseWindow::xMax, ui::BaseWindow::height, ui::BaseWindow::start_y;

int main()
{
    static std::unique_ptr<ToxHandler> tox_handler = std::make_unique<ToxHandler>();
    static std::unique_ptr<ui::Controller> controller = std::make_unique<ui::Controller>(ui::Controller::get_instance(tox_handler.get()));

    auto draw_stuff = []()
    {
        controller->update('\0');
    };

    tox_handler->set_update_callback(draw_stuff);

    while (int ch = getch())
    {
        if (ch == KEY_F(4))
            break;
        controller->update(ch);
    }

    return 0;
}