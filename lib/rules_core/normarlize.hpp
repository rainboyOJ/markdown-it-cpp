//Normalize 在render之内,parse之前调用

#pragma once
#include <string>
#include <sstream>

#include "../common/utils.hpp"
#include "../preset/token.hpp"


namespace markdownItCpp {


void normalize(std::string_view src,std::string& out){
    out.clear(); //清空

    decltype(src.size()) i{0};
    for( ;i < src.size() ; ++i){
        if( uCodeChar(src[i]) == newliner_char ){ //'\r'
            if( (i+1) < src.size() && uCodeChar(src[i+1]) == newlinen_char ) // '\n'
                ++i;
            out += newlinen_char;
        }
        //else if() //TODO \0 EFBFBD https://zhuanlan.zhihu.com/p/45695434
        else 
            out += src[i];
    }

}

}
