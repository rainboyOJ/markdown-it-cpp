//hr

#pragma once

#include "state_block.hpp"
//#include "../preset/token.hpp"

namespace markdownItCpp {


//template<typename MarkdownIt>
bool hr(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{
    if( state.isCodeBlock(startLine)) return false;

    auto pos = state.lineInfo[startLine].first_nospace_pos();
    auto end = state.lineInfo[startLine].eMarks;

    auto marker = state.charCodeAt(pos);

    if( NoneOneOf(marker, STAR_CHAR,minus_char,UNDERSCORE_CHAR))
        return false;

    // markers can be mixed with spaces, but there should be at least 3 of them
    int cnt = 1;
    for( ; pos < end ; ++ pos){
        auto ch = state.charCodeAt(pos);
        if( ch != marker && !isSpace(ch)) return false;
        if( ch == marker) cnt++;
    }
    if( cnt < 3) return false;
    if( silent ) return true;

    state.line = startLine + 1;

    state.push(hr_type,hr_type,0);
    state.tokens_back().map = { startLine,state.line};
    //state.tokens_back().markup == TODO
    return true;

}

}
