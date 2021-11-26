#pragma once
#include <string>
#include <cstring>
#include "../preset/token.hpp"

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

void trim_string(std::string& str){
    while(str.length() && isSpace(str.back())) str.pop_back();
    while(str.length() && isSpace(str.front())) str.erase(str.begin());
}

inline uint32_t uCodeChar(char c){
    return static_cast<uint32_t>(c) & 0xff;
}

//https://stackoverflow.com/a/19826808
constexpr char ascii_hex_4bit[23] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15};
//std::string url_decode()
std::string url_decode(std::string_view str)
{
    size_t i, j, len = str.length();
    char c, d, url_hex;
    std::string s(len+1,0);
    char *decoded = s.data();

    if (decoded == NULL) return NULL;

    auto TO_UPPER = [](char c){
        if ( c >='a' && c <='z')
            return char(c-'a' +'A');
        return c;
    };

    i = 0;
    j = 0;
    do
    {
        c = str[i];
        d = 0;

        if (c == '%')
        {
            url_hex = TO_UPPER(str[++i]);
            if (((url_hex >= '0') && (url_hex <= '9')) || ((url_hex >= 'A') && (url_hex <= 'F')))
            {
                d = ascii_hex_4bit[url_hex - 48] << 4;

                url_hex = TO_UPPER(str[++i]);
                if (((url_hex >= '0') && (url_hex <= '9')) || ((url_hex >= 'A') && (url_hex <= 'F')))
                {
                    d |= ascii_hex_4bit[url_hex - 48];
                }
                else
                {
                    d = 0;
                }
            }
        }
        else if (c == '+')
        {
            d = ' ';
        }
        else d = c;
        //else if ((c == '*') || (c == '-') || (c == '.') || ((c >= '0') && (c <= '9')) ||
        //((c >= 'A') && (c <= 'Z')) || (c == '_') || ((c >= 'a') && (c <= 'z')))
        //{
            //d = c;
        //}

        decoded[j++] = d;
        ++i;
    } while ((i < len) && (d != 0));

    decoded[j] = 0;
    return s;
}

//函数的别名
constexpr auto &unescapeAll = url_decode;


// Parse linke title

struct ParseResult {
    bool ok{false};  //解析是否成功
    int pos{0};      //完成后解析了多少个字符
    int lines{0};    //完成后解析了多少行
    std::string str; //解析的结果

    ParseResult() = default;
    ParseResult(ParseResult&& p)
    {}
    void operator=(ParseResult&& p){
        ok=p.ok;
        pos=p.pos;
        lines=p.lines;
        str=std::move(p.str);
    }

};

std::string_view str_slice(std::string_view src,size_t start,size_t end){
    return std::string_view(&src[start],end-start);
}


ParseResult parseLinkTitle(std::string_view str,int start,int end){
    ParseResult res;

    auto start_bak = start;
    if( start >= end) return res;

    auto marker = str[start];
    if( NoneOneOf(marker, QUOTE_CHAR ,SINGLE_QUOTE_CHAR,OPEN_CHAR) )
        return res;
    if( marker == '(' ) marker = ')';

    for(++start ; start < end; ++start){
        auto ch = str[start];
        if( ch == marker) { //结束符 ) " '
            res.pos = start+1;
            res.str = unescapeAll(str_slice(str,start_bak+1,start));
            res.ok = true;
            return res;
        }
        else if ( ch == '(' && marker == ')')
            return  res;
        else if(ch == '\\' && start + 1 < end){
            start++;
            if(  uCodeChar(str[start]) == newlinen_char ) res.lines++;
        }
    }
    return res;
}


ParseResult parseLinkDestination(std::string_view str,int start,int end){
    ParseResult res;
    auto start_bak = start;
    if( uCodeChar(str[start]) == LESS_CHAR ) {
        for( ++start; start < end; ++start) {
            auto ch = uCodeChar(str[start]);

            if( AnyOf(ch, newlinen_char,LESS_CHAR )) return res;
            if( ch == GREATER_CHAR ) { // >
                res.pos = start+1;
                res.str = unescapeAll(str_slice(str,start_bak+1,start));
                res.ok = true;
                return res;
            }
            if( ch == backslash_char && start+1 < end) ++start;
        }
        return res;
    }
    // this should be ... } else { ... branch
    auto &pos = start;
    int level = 0;
    for( ; start < end; ++start) {
        auto ch = uCodeChar(str[start]);
        if( ch == SPACE_CHAR ) break;

        // ascii control characters
        if( ch < 0x20 || ch ==0x7f ) break;

        if (ch == 0x5C /* \ */ && pos + 1 < end) {
            if (uCodeChar(str[pos+1]) == 0x20) { break; }
            ++pos;
            continue;
        }

        if (ch == 0x28 /* ( */) {
            level++;
            if (level > 32) { return res; }
        }

        if (ch == 0x29 /* ) */) {
            if (level == 0) { break; }
            level--;
        }
    }
    if(start_bak == start) return res;
    if( level !=0) return res;

    res.str = unescapeAll(str_slice(str,start_bak,start));
    res.pos = start;
    res.ok = true;
    return res;
}

} // end namespace markdownItCpp
