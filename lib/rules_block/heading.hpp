//heading (#,## ...)
#pragma once

#include "state_block.hpp"

namespace markdownItCpp {

bool heading(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{
    auto [pos,end] = state.lineInfo[startLine].getBeginEnd();

    if( state.isCodeBlock(startLine) ) return false;

    auto ch = state.charCodeAt(pos);
    if( ch != SHARP_CHAR || pos >= end) return false;

    int level{1};
    for( ++pos ; state.charCodeAt(pos) == SHARP_CHAR && pos < end; pos++) {
        level++;
        if( level > 6 ) return  false;
    }
    if(silent) return true;

    //去除末尾的空白与#
    end = state.skipSpacesBack(end,pos);
    auto tmp = state.skipCharsBack(end,'#',pos);
    if( tmp  > pos && isSpace(state.charCodeAt(tmp-1)))
        end = tmp;

    state.line = startLine +1;
    state.push(heading_open_type,heading_type(level),1);
    state.tokens_back().map = {startLine,state.line};

    state.push(inline_type,emptyLine,0);
    state.tokens_back().content = trim(state.slice(pos,end));
    state.tokens_back().map = {startLine,state.line};
    state.tokens_back().children.clear();

    state.push(heading_close_type,heading_type(level),-1);

    return true;
}

}
