#ifndef FRIENDS_H
#define FRIENDS_H

#include "interface/content_window.h"

namespace interface
{

    class Friends : public ContentWindow
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