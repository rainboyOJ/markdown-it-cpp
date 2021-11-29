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

    if(state.charCodeAt(pos) != BACKSLASH_CHAR ) return  false;

    for( ++pos ; pos< end ; ){
        auto ch = state.charCodeAt(pos);
        if( ch < 256 && ESCAPED(state.src[pos]) ){
            if( !silent )
                state.pending += state.src[pos];
            state.pos = pos+1;
            return true;
        }

        if( ch == newlinen_char ){
            if( !silent ) 
                state.push(hardbreak_type, break_tag, 0);

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
