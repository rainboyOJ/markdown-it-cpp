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

template<typename T,typename ... Args>
bool AnyOf(T&& t,Args&&... args){
    if constexpr ( sizeof...(args) == 0){
        return false;
    }
    else {
        return ( (t== args) || ... );
    }
}
template<typename T,typename ... Args>
bool NoneOneOf(T&& t,Args&&... args){
    if constexpr ( sizeof...(args) == 0){
        return true;
    }
    else
        return ! AnyOf(std::forward<T>(t),std::forward<Args>(args)...);
}

std::string_view trim(std::string_view str){
    while ( str.length() and isSpace(str.front()))  str.remove_prefix(1);
    while ( str.length() and isSpace(str.back()))  str.remove_suffix(1);
    return str;
}

inline uint32_t uCodeChar(char c){
    return static_cast<uint32_t>(c) & 0xff;
}

} // end namespace markdownItCpp
