#pragma once
#include <string>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"
#include "./state_inline.hpp"

namespace markdownItCpp {
/**
 * 作用: 处理换行符
 * pending 的末尾
 *  "  "(2 space ) hardbreak
 *  " " (1 space) softbreak
 *  不是空格 softbreak
 *
 */
bool newline(StateInline& state,bool silent) {
    auto pos = state.pos;
    if( state.src[pos] != '\n') return false;

    int pmax = state.pending.length() ?  state.pending.length() -1 : -1;
    int max = state.posMax;

    // '  \n' -> hardbreak
    if( !silent){
        if( pmax >= 0 && state.pending[pmax] == ' ' ){
            if( pmax >= 1 && state.pending[pmax-1] == ' ' ){
                while ( state.pending.back() == ' ' )  state.pending.pop_back();
                state.push("hardbreak", "br", 0);
            }
            else {
                state.pending.pop_back();
                state.push("softbreak", "br", 0);
            }
        }
        else{
            state.push("softbreak", "br", 0);
        }
    }

    pos++;
    while(pos < state.posMax && isSpace(state.src[pos]))
        pos++;
    state.pos = pos;
    return true;
};

}
