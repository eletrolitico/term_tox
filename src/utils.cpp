#include "include/utils.h"

#include <iostream>
#include <fstream>

void log(const std::string &msg, const std::string &file)
{
    std::fstream Log;
    Log.open(file, std::ios_base::app);
    if (!Log.is_open())
    {
        Log.clear();
        Log.open(file, std::ios_base::out);
    }

    Log << msg << std::endl;
}

void log(const std::string &msg)
{
    log(msg, "default.log");
}