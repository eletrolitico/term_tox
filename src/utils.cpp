#include "include/utils.h"

#include <iostream>
#include <fstream>

void log(const std::string &msg)
{
    std::fstream Log;
    Log.open("log.txt", std::ios_base::out | std::ios_base::in);
    if (!Log.is_open())
    {
        Log.clear();
        Log.open("log.txt", std::ios_base::out);
    }

    Log << msg << std::endl;
}
