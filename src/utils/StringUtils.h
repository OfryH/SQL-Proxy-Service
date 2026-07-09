#pragma once

#include <string>

inline void trim(std::string &s)
{
    while (!s.empty() &&
           (s.back() == '\r' ||
            s.back() == '\n' ||
            s.back() == ' ' ||
            s.back() == '\t'))
    {
        s.pop_back();
    }
}