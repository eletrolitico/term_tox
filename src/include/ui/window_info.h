#ifndef WINDOW_INFO
#define WINDOW_INFO

#include "tox_handler.h"

namespace ui
{
    class WindowInfo
    {
    public:
        static WindowInfo& get()
        {
            static WindowInfo window_info;
            return window_info;
        }
        ~WindowInfo() 
        {
            delete talking_to_;
        }

        int x_max_, height_, start_y_;
        Friend *talking_to_ {nullptr};
    private:
        WindowInfo() = default;
        WindowInfo(const WindowInfo&) = delete;
        void operator=(const WindowInfo&) = delete;
    };
}

#endif