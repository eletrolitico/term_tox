#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string>
#include <vector>
#include <memory>

#include <stdio.h>
#include <fcntl.h>
#include "unistd.h"

#include "ui/controller.h"
#include "tox_handler.h"

// declaração de variáveis estáticas da BaseWindow
ToxHandler *ui::BaseWindow::t_hand;
int ui::BaseWindow::xMax, ui::BaseWindow::height, ui::BaseWindow::start_y;

auto tox_handler = new ToxHandler();
auto controller = ui::Controller::get_instance(tox_handler);

void draw_stuff() { controller->update('\0'); }

int main()
{
    tox_handler->set_update_callback(draw_stuff);

    while (int ch = getch())
    {
        if (ch == KEY_F(4))
            break;
        controller->update(ch);
    }

    delete controller;
    delete tox_handler;

    return 0;
}