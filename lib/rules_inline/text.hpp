#pragma once
#include <string>
#include <sstream>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"
#include "./state_inline.hpp"

namespace markdownItCpp {

// Rule to skip pure text
// '{}$%@~+=:' reserved for extentions

// !, ", #, $, %, &, ', (, ), *, +, ,, -, ., /, :, ;, <, =, >, ?, @, [, \, ], ^, _, `, {, |, }, or ~

// !!!! Don't confuse with "Markdown ASCII Punctuation" chars
// http://spec.commonmark.org/0.15/#ascii-punctuation-character
bool isTerminatorChar(int ch) {
  switch (ch) {
    case 0x0A/* \n */:
    case 0x21/* ! */:
    case 0x23/* # */:
    case 0x24/* $ */:
    case 0x25/* % */:
    case 0x26/* & */:
    case 0x2A/* * */:
    case 0x2B/* + */:
    case 0x2D/* - */:
    case 0x3A/* : */:
    case 0x3C/* < */:
    case 0x3D/* = */:
    case 0x3E/* > */:
    case 0x40/* @ */:
    case 0x5B/* [ */:
    case 0x5C/* \ */:
    case 0x5D/* ] */:
    case 0x5E/* ^ */:
    case 0x5F/* _ */:
    case 0x60/* ` */:
    case 0x7B/* { */:
    case 0x7D/* } */:
    case 0x7E/* ~ */:
      return true;
    default:
      return false;
  }
}

/**
 * 作用: 把所有不是TerminatorChar 的char 加入到state.pending
 */

bool text(StateInline& state,bool silent){
    int pos = state.pos;
    while ( pos < state.posMax && !isTerminatorChar(state.src[pos]) ) { //不是终止字符
        pos++;
    }

    if( pos == state.pos)//没有捕获到任何东西
        return false;
    if( !silent ) // 加入pending 
        state.pending += std::string(state.src.begin()+state.pos,state.src.begin()+pos);
    state.pos = pos;
    return true;
}

}
