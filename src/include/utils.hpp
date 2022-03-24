#ifndef UTILS_HPP
#define UTILS_HPP

#endif // UTILS_HPP

#include <iostream>
#include <stdexcept>

inline unsigned int stoui(const std::string& s)
{
    unsigned long lresult = stoul(s, 0, 10);
    unsigned int result = lresult;
    if (result != lresult) throw std::out_of_range("");
    return result;
}
