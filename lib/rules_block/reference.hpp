//reference
// [foo]: /url "title" 
// https://github.github.com/gfm/#link-reference-definitions
// https://www.markdownguide.org/basic-syntax/#reference-style-links

#pragma once

#include "state_block.hpp"

namespace markdownItCpp {

bool reference(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{

    auto [pos,end] = state.lineInfo[startLine].getBeginEnd();
    if( state.isCodeBlock(startLine)) return false;
    if( state.charCodeAt(pos) != OPEN_BRACKET_CHAR ) return false;


    //在开始的位置 简单的检查 [link](url)
    while( ++pos < end){
        if( state.charCodeAt(pos) == CLOSE_BRACKET_CHAR  &&
                state.charCodeAt(pos-1) != BACKSLASH_CHAR ){
            if( pos+1 == end) return false; //后面什么也没有
            if(state.charCodeAt(pos+1) != colon_char ) // []:
                return false;
            break; 
        }
    }

    endLine = state.lineMax;
    auto terminatorRules = state.md.getBlockRules("reference");

    auto OldParentType = state.parentType;
    state.parentType = reference_paragraph;
    size_t nextLine = startLine+1;
    bool terminate{false};

    for( ; nextLine < endLine && state.isEmpty(nextLine) ; ++ nextLine){
        if( state.isCodeBlock(nextLine) ) continue; // 不当成 codeBlock ?
        if( state.lineInfo[nextLine].sCount < 0 ) continue; // ?
        for (const auto& e : terminatorRules) {
            if( e(state,nextLine,endLine,true)){
                terminate = true;
                break;
            }
        }
        if( terminate ) break;
    }

    auto str = state.getLines(startLine, nextLine, state.blkIndent, false);
    trim_string(str);
    auto str_len = str.length();
    size_t labelEnd{0},lines{0};

    //计算有多少行 得到label 内容
    for( pos = 1; pos < str_len ; ++pos){
        auto ch = uCodeChar(str[pos]);
        if( ch == OPEN_BRACKET_CHAR ) 
            return false;
        else if( ch == CLOSE_BRACKET_CHAR ){
            labelEnd = pos;
            break;
        }
        else if( ch == newlinen_char){
            lines++;
        }
        else if( ch == BACKSLASH_CHAR ){
            ++pos;
            if( pos < str_len && uCodeChar(str[pos]) == newlinen_char )
                lines++;
        }
    }
    if( labelEnd <= 0  || str[labelEnd+1] != ':') return false;

    // [label]:   destination   'title'
    //         ^^^ skip optional whitespace here
    for (pos = labelEnd + 2; pos < str_len; pos++) {
        auto ch = uCodeChar(str[pos]);
        if (ch == 0x0A) { // \n
            lines++;
        } else if (isSpace(ch)) {
            /*eslint no-empty:0*/
        } else {
            break;
        }
    }

    // [label]:   destination   'title'
    //            ^^^^^^^^^^^ parse this

    auto res = parseLinkDestination(str, pos, str_len);
    if( !res.ok ) return false;

    std::string href = std::move(res.str);

    pos = res.pos;
    lines += res.lines;


    auto destEndPos = pos;
    auto destEndLineNo = lines;

    // [label]:   destination   'title'
    //                       ^^^ skipping those spaces
    auto start = pos;
    for (; pos < str_len; pos++) {
        auto ch = uCodeChar(str[pos]);
        if (ch == 0x0A) { // \n
            lines++;
        } else if (isSpace(ch)) {
            /*eslint no-empty:0*/
        } else {
            break;
        }
    }

    // [label]:   destination   'title'
    //                          ^^^^^^^ parse this
    res = parseLinkTitle(str, pos, str_len); //解析 title
    std::string title;
    if (pos < str_len && start != pos && res.ok) {
        title = std::move(res.str);
        pos = res.pos;
        lines += res.lines;
    } else {
        pos = destEndPos;
        lines = destEndLineNo;
    }

    // skip trailing spaces until the rest of the line
    while (pos < str_len) {
        if (!isSpace(str[pos])) { break; }
        pos++;
    }

    if (pos < str_len && str[pos] !='\n') {
        // garbage at the end of the line
        return false;
    }

    std::string label(str_slice(str,1,labelEnd));
    if( label.length() == 0) return false; // empty label wrong

    if( silent) {
        state.parentType = OldParentType;
        return true;
    }
    state.parentType = OldParentType;

    state.md.references[label].title = std::move(title);
    state.md.references[label].href = std::move(href);
    state.line = startLine + lines + 1;
    return true;
}

}
