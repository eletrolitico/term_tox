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

        void draw();
        void update(int ch);

    private:
        uint8_t selected_friend = 0;
        bool adding_friend = false, tox_id_done = false;
        std::string adding_tox_id = "", adding_message = "";
    };
}
#endif