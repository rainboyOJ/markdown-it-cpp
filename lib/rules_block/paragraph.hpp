//paragraph

#pragma once

#include "state_block.hpp"

namespace markdownItCpp {


//template<typename MarkdownIt>
bool paragraph(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{

    endLine      = state.lineMax;
    endLine      = state.lineMax;
    int nextLine = startLine + 1;

    auto terminatorRules = state.md.getBlockRules("paragraph");
    
    bool terminate;
    auto oldParentType = state.parentType;
    state.parentType = "paragraph"; //改变 parentType

    // jump line-by-line until empty one or EOF
    for( ; nextLine < endLine && ! state.lineInfo[nextLine].isEmpty() ; nextLine++){
        // code block normally
        if(  state.isCodeBlock(nextLine) /*state.sCount[nextLine] - state.blkIndent > 3*/ )
            continue;

        // quirk for blockquotes, this line should already be checked by that rule
        if( state.lineInfo[nextLine].sCount < 0 ) 
            continue;

        // Some tags can terminate paragraph without empty line.
        terminate = false;
        //其它的处理 "paragraph"的rule 但不是这个
        for(int i =0;i < terminatorRules.size() ;i++){
            if( terminatorRules[i](state,nextLine,endLine,true) ){
                terminate = true;
                break;
            }
        }
        if(terminate) break;
    }
    std::string content = state.getLines(startLine, nextLine, state.blkIndent, false);
    //trim
    while ( content.length() && isSpace(content.front()) ) content.erase(content.begin());
    while ( content.length() && isSpace(content.back()) ) content.pop_back();
    state.line = nextLine;
    //state.push(std::string type, std::string tag, int nesting)
    state.push("paragraph_open","p",1);
    state.tokens.back().map  = {startLine,state.line};

    state.push("inline","",0);
    state.tokens.back().content = std::move(content);
    state.tokens.back().map = {startLine,state.line};
    state.tokens.back().children.clear();

    state.push("paragraph_close","p",-1);
    state.parentType = oldParentType;
    return true;
}



} // end namespace markdownItCpp
