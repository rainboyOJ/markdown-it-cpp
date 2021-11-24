
#pragma once
#include <string>
#include <sstream>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"
#include <cassert>
#include "state_block.hpp"

namespace markdownItCpp {


bool blockquote(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{
    //是不是应该为 codeblock
    if( !state.lineInfo[startLine].line_sCount_less(4) ) return false;

    auto pos = state.lineInfo[startLine].first_nospace_pos();
    if(state.charCodeAt(pos++) != GREATER_CHAR) // >
        return false;

    //不需要真正的去执行
    if( silent ) return true;

    auto changeLineInfo = [&state](int startLine,int pos,bool & lastLineEmpty) {

        bool adjustTab,spaceAfterMarker;
        int initial,offset;
        initial = offset = state.lineInfo[startLine].sCount + 1; // 到达 > 后面

        //略过> 后面的空白字符
        if( state.charCodeAt(pos) == SPACE_CHAR ){
            // ' >   test '
            //     ^ -- position start of line here:
            pos++;
            initial++;
            offset++;
            adjustTab = false;
            spaceAfterMarker = true;
        }
        else if( state.charCodeAt(pos) == TAB_CHAR ){
            spaceAfterMarker = true;
            if( ( state.lineInfo[startLine].bsCount + offset) % 4 == 3){
                // '  >\t  test '
                //       ^ -- position start of line here (tab has width===1)
                pos++;
                initial++;
                offset++;
                adjustTab = false;
            }
            else {
                // ' >\t  test '
                //    ^ -- position start of line here + shift bsCount slightly
                //         to make extra space appear 具体原因不知道
                adjustTab = true;
            }
        }
        else
            spaceAfterMarker = false;

        state.lineInfo[startLine].bMarks = pos; // change bMarks
        for( ;pos < state.lineInfo[startLine].eMarks; pos++){
            if( state.charCodeAt(pos) == TAB_CHAR )
                offset += 4 - (offset + state.lineInfo[startLine].bsCount + (adjustTab ? 1 : 0)) % 4; // 改值
            else if( state.charCodeAt(pos) == SPACE_CHAR)
                offset++;
            else break;
        }

        lastLineEmpty = pos >= state.lineInfo[startLine].eMarks;

        state.lineInfo[startLine].bsCount = state.lineInfo[startLine].sCount +1 + ( spaceAfterMarker ? 1 : 0);
        state.lineInfo[startLine].sCount = offset - initial;
        state.lineInfo[startLine].tShift = pos - state.lineInfo[startLine].bMarks;
    };

    bool lastLineEmpty{false};
    changeLineInfo(startLine, pos,lastLineEmpty);
    state.parentType = parentType_blockquote;

    // Search the end of the block
    //
    // Block ends with either:
    //  1. an empty line outside:
    //     ```
    //     > test
    //
    //     ```
    //  2. an empty line inside:
    //     ```
    //     >
    //     test
    //     ```
    //  3. another tag:
    //     ```
    //     > test
    //      - - -
    //     ```
    auto & terminatorRules = state.md.getBlockRules("blockquote");
    bool terminate{false};
    auto nextLine = startLine+1;
    for( ; nextLine < endLine; nextLine++) {
        auto pos = state.lineInfo[nextLine].first_nospace_pos();
        if( state.lineInfo[nextLine].isEmpty() ) break; // case 1:
        if( state.charCodeAt(pos++) == GREATER_CHAR
                && !state.isOutdented(nextLine) )
        {
            changeLineInfo(nextLine, pos,lastLineEmpty);
            continue;
        }
        //case 2
        if( lastLineEmpty ) break; // case 2:

        //case 3
        for (const auto& e : terminatorRules) {
            if( e(state,nextLine,endLine,true) ){
                terminate = true;
                break;
            }
        }

        if( terminate ){
            state.lineMax = nextLine;
            if( state.blkIndent != 0){
                state.lineInfo[nextLine].sCount -= state.blkIndent;
            }
            break;
        }
        state.lineInfo[nextLine].sCount  = -1; // ??
    }

    Token t(blockquote_open,blockquote_tag,1);
    t.markup = blockquote_markup;
    t.map = {startLine,0};
    state.push(t);

    state.md.blockTokenize(state,startLine,nextLine);

    Token t2(blockquote_close,blockquote_tag,1);
    t2.markup = blockquote_markup;
    state.push(t);

    return true;
}

};
