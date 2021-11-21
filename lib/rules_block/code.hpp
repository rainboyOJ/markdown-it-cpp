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

bool code(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{
    if( !state.isCodeBlock(startLine)) return false;

    auto last = startLine + 1, nextLine = last;

    for( ; nextLine < endLine;) {
        if( state.isEmpty(nextLine) ){
            ++nextLine;
            continue;
        }
        if( state.isCodeBlock(nextLine) ){
            last = ++nextLine;// 过滤末尾空行
            continue;
        }
        break;
    }

    state.line = last;
    state.push(codeblock_type,code_type,NESTING::self_close);
    state.tokens_back().map = {startLine,last};
    state.tokens_back().content = state.md.push_content_cache( state.getLines(startLine,last,4+state.blkIndent,false));
    return true;
}

}
