
#pragma once
#include <string>

namespace markdownItCpp {

bool isSpace(int code) {
    if( code == 0x09 || code == 0x20)
        return true;
    return false;
}

} // end namespace markdownItCpp
