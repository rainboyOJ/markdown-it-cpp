#pragma once
#include <string>

#include "../preset/token.hpp"
#include "./state_inline.hpp"

namespace markdownItCpp {

/**
 * 作用: 处理转义字符 \
 *
 */

inline bool ESCAPED(char c){
    return AnyOf(c,'\\','!','"','#','$','%','&','\'','(',')','*','+',',','.','/',':',';','<','=','>','?','@','[',']','^','_','`','{','|','}','~','-');
}

bool escape(StateInline& state,bool silent) {
    auto pos = state.pos;
    auto end = state.posMax;

    //检查 是不是 backslash 
    if(state.charCodeAt(pos) != BACKSLASH_CHAR ) return  false;

    ++pos ;
    if(  pos< end ){
        auto ch = state.charCodeAt(pos);
        if( ch < 256 && ESCAPED(state.src[pos]) ){ //可以被escaped的char
            if( !silent )
                state.pending += state.src[pos];
            state.pos = pos+1;
            return true;
        }

        if( ch == newlinen_char ){ //如后是换行符
            if( !silent ) 
                state.push(hardbreak_type, break_tag, 0); //加入<br>

            //skip 下一行的开头的空格
            pos++;
            while ( pos< end ) {
                if( !isSpace(state.src[pos])) break;
                pos++;
            }
            state.pos = pos;
            return true;
        }
    }

    if( !silent) state.pending += '\\';
    state.pos++;
    return true;
}

}
