
#pragma once
#include <string>

namespace markdownItCpp {

bool isSpace(int code) {
    if( code == 0x09 || code == 0x20)
        return true;
    return false;
}


std::string escapeHtml(std::string_view str){
    std::string result{};
    for (const auto& e : str) {
        switch(e){
            case '&': 
                result += "&amp;";
                break;
            case    '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '"':
                result += "&quot;";
                break;
            default:
                result += e;
        }
    }
    return result;
}

} // end namespace markdownItCpp
