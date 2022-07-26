#pragma once
#include "xorstr.hpp"
#include <string>
#include <time.h>

std::string RandomString(int len) {
    srand(time(NULL));
    std::string str = _xor_("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    std::string newstr;
    int pos;
    while (newstr.size() != len) {
        pos = ((rand() % (str.size() - 1)));
        newstr += str.substr(pos, 1);
    }
    return newstr;
}