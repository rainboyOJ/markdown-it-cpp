
// Code block (4 spaces padded)

#pragma once
#include <string>
#include <sstream>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"
#include <cassert>
#include "state_block.hpp"

namespace markdownItCpp {

bool fence(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{
    if( state.isCodeBlock(startLine)) return false;

    auto pos = state.lineInfo[startLine].first_nospace_pos();
    auto end = state.lineInfo[startLine].eMarks;
    if( pos + 3 > end) return false;

    uint32_t marker = state.charCodeAt(pos);
    if(NoneOneOf(marker, GRAVE_CHAR,TILDE_CHAR)) return false;

    auto pos_bak = pos ;
    pos = state.skipChars(pos, marker);
    if( pos - pos_bak < 3) return false;

    std::string_view markup = state.slice(pos_bak,pos);
    std::string_view params = state.slice(pos,end);
    if( marker == GRAVE_CHAR && params.find('`') != std::string_view::npos)
        return false;

    //search end of block
    auto nextLine = startLine;
    bool haveEndMarker{false};
    for(;;){
        if( ++nextLine >= endLine) break;
        auto pos = state.lineInfo[nextLine].first_nospace_pos();
        auto mem = pos;
        auto end = state.lineInfo[nextLine].eMarks;

        // non-empty line with negative indent should stop the list:
        // - ```
        //  test
        //  这说明fenc可以被list包含
        if( !state.lineInfo[nextLine].isEmpty() 
                && state.nest_less_blkIndent(nextLine) )
            break;
        if( state.charCodeAt(pos) != marker ) continue; //开头不是

        // closing fence should be indented less than 4 spaces
        if( state.isCodeBlock(nextLine) ) continue;

        pos = state.skipChars(pos, marker);
        if( pos-mem != markup.length() ) continue; //不一样长

        // make sure tail has spaces only
        pos = state.skipSpaces(pos);
        if( pos < end) continue;
        //found!
        haveEndMarker = true;
        break;
    }

    state.line = nextLine + (haveEndMarker ? 1 : 0);

    state.push(fenc_type,code_type,NESTING::self_close);
    state.tokens_back().info = std::string(params);
    state.tokens_back().content = state.getLines_view(startLine+1, nextLine);
    state.tokens_back().markup = markup;
    state.tokens_back().map = { startLine,state.line};

    return true;

}

}
