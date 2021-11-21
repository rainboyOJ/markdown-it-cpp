// GFM table, https://github.github.com/gfm/#tables-extension-

#pragma once
#include <string>
#include <sstream>

#include "../define.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"
#include <cassert>
#include "state_block.hpp"

namespace markdownItCpp {

enum class ALGIN {
    left,
    center,
    right,
    none
};

std::string algin2string(ALGIN al){
    switch(al){
        case ALGIN::center: return "center";
        case ALGIN::right:  return "right";
        case ALGIN::left:   return "left";
        case ALGIN::none:   return "";
    }
}




std::vector<std::string> escapedSplit(std::string_view str){
    std::vector<std::string> result;
    bool isEscaped = false;
    std::string current = "";
    decltype(str)::iterator
        pos     = str.begin() ,
        lastPos = str.begin(),
        end     = str.end();


    for ( ; pos < end ; ++pos){
        auto &e = *pos;
        if( uCodeChar(e) == pip_char ){
            if( !isEscaped ){
                result.emplace_back( current + std::string(lastPos,pos));
                lastPos = pos+1;
                current.clear();
            }
            else { // escaped pip \|
                current += std::string(lastPos,pos-1);
                lastPos = pos;
            }
        }
        isEscaped = ( uCodeChar(e) == backslash_char);
    }
    result.emplace_back( current + std::string(lastPos,pos));
    return result;
}


bool table(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{
    std::vector<ALGIN> algins;

    if( endLine - startLine < 2) return false; //至少3行
    auto nextLine = startLine + 1;
    if( state.nest_less_blkIndent(nextLine) ) return false;
    if( state.should_code_block(nextLine) ) return  false;

    if ( state.lineInfo[nextLine].isEmpty()) return false;

    auto pos = state.lineInfo[nextLine].first_nospace_pos();
    auto CH = state.charCodeAt(pos++);
    if( NoneOneOf(CH, pip_char,minus_char,colon_char) ) return false;

    //第2个字符存在
    if( pos > state.lineInfo[nextLine].eMarks ) return false;
    auto CH2 = state.charCodeAt(pos++);

    if( NoneOneOf(CH2, pip_char,minus_char,colon_char) && !isSpace(CH2) ) return false;

    //如果第一个字符是 - 那么第2个必须不是 space
    //这样就不会和list 混淆
    if( CH == minus_char && isSpace(CH2) ) return false;

    //查检这一行是不是都是符合条件的字符 | - : ' ' '\t'
    for ( ;pos < state.lineInfo[nextLine].eMarks ; pos++ ) {
        auto ch = state.charCodeAt(pos);
        if( NoneOneOf(ch, pip_char,minus_char,colon_char,SPACE_CHAR,TAB_CHAR) ) return false;
    }

    auto LineText = state.getOneLineView_nospc(nextLine);
    auto columns = escapedSplit(LineText);
    for(int i=0;i<columns.size();++i){
        auto t = trim(columns[i]);
        if( t.length() == 0){
            if( i == 0 || i == columns.size()-1) continue;
            else return false;
        }
        //regex /^:?-+:?$/
        for(int j=0;j<t.length();++j){
            auto ch = uCodeChar(t[j]);
            if( ch == colon_char && j != 0 && j != t.length() - 1) return false;
            if( NoneOneOf(ch, colon_char,minus_char)) return false;
        }
        //对齐方式
        if(uCodeChar(t.back()) == colon_char )
            algins.push_back( uCodeChar(t.front()) == colon_char ?  ALGIN::center : ALGIN::right);
        else if( uCodeChar(t.front()) == colon_char)
            algins.push_back(ALGIN::left);
        else algins.push_back(ALGIN::none);
    }

    LineText = trim(state.getOneLineView_nospc(startLine));
    if( LineText.find('|') == std::string_view::npos) return false;
    if( state.should_code_block(startLine) ) return false;

    columns = escapedSplit(LineText);

    auto start = 0;
    auto size = columns.size();
    if( size && columns.front().length() == 0) ++start,--size;
    if( size && columns.back().length() == 0) --size;
    if( size == 0 || size != algins.size() ) return false;

    if( silent) return true; 

    std::string_view oldParentType = state.parentType;
    state.parentType = table_type;

    auto terminatorRules = state.md.getBlockRules("blockquote");
    state.push(table_open_type,table_type,1);
    state.tokens_back().map = {startLine,0};

    state.push(table_thead_open_type,table_thead_type,1); // <thead>
    state.tokens_back().map  = {startLine,startLine+1};

    state.push(tr_open_type,tr_type,1); // <tr>
    state.tokens_back().map  = {startLine,startLine+1};

    for (auto i = start,j = 0; i< start+size; ++i,++j) { // 处理第一个元素 <th>
        state.push(th_open_type, th_type, 1);
        if( algins[j] != ALGIN::none) {
            state.tokens_back().attrPush("style", std::string("text-align:") + algin2string(algins[j]));
        }
        state.push(inline_type,emptyLine,0);
        state.tokens_back().content = state.md.push_content_cache(std::string( trim(columns[i]) ));
        state.tokens_back().children.clear();

        state.push(th_close_type,th_type,-1);

    }

    state.push(tr_close_type,tr_type,-1);   // </tr>
    state.push(table_thead_close_type,table_thead_type,-1);  // </thead>

    bool hasTbody{false};

    bool terminate{false};
    //table body 
    for( nextLine = startLine+2; nextLine < endLine ; ++ nextLine){
        if( state.nest_less_blkIndent(nextLine) ) break;
        terminate = false;
        for (const auto& e : terminatorRules) {
            if( e(state,nextLine,endLine,true)){
                terminate = true;
                break;
            }
        }
        if( terminate ) break; //被其它的rules识别

        auto LineText = trim(state.getOneLineView_nospc(nextLine));
        if( LineText.length() == 0) break;
        if( state.should_code_block(nextLine)) break;
        auto columns = escapedSplit(LineText);

        auto start = 0;
        auto size = columns.size();
        if( size && columns.front().length() == 0) ++start,--size;
        if( size && columns.back().length() == 0) --size;

        if( nextLine == startLine+2){
            state.push(tbody_open_type,tbody_type,1);
            state.tokens_back().map = { startLine+2,0};
            hasTbody = true;
        }

        state.push(tr_open_type, tr_type, 1);
        state.tokens_back().map = {nextLine,nextLine+1};

        for( auto i = start ,j = 0;i < start+size ; ++i,++j) {
            state.push(td_open_type,td_type,1);
            if( algins[j] != ALGIN::none) {
                state.tokens_back().attrPush("style", std::string("text-align:") + algin2string(algins[j]));
            }
            state.push(inline_type,emptyLine,0);
            state.tokens_back().content = state.md.push_content_cache(std::string( trim(columns[i]) ));
            state.tokens_back().children.clear();

            state.push(td_close_type,td_type,-1);
        }
        state.push(tr_close_type,tr_type,-1);
    }

    if( hasTbody) {
        state.push(tbody_close_type,tbody_type,-1);
    }

    state.push(table_close_type,table_type,-1);
    state.parentType = oldParentType;
    state.line = nextLine;

    return true;
}

}
