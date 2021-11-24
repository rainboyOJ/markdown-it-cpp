//list
// list 应该是最复杂的block了

#pragma once

#include "state_block.hpp"
//#include "../preset/token.hpp"

namespace markdownItCpp {

constexpr auto NUMBER_ONE = "1"sv;

//辅助函数
/**
 * 检查与略过 bulletListMarker
 * return -1 表示不是 bulletList
 */
int skipBulletListMarker(StateBlock& state,size_t startLine){
    auto [pos ,end ] = state.lineInfo[startLine].getBeginEnd();
    //不是list的marker
    if( NoneOneOf(state.src[pos++], '*','-','+')) return -1;
    if(pos < end &&  !isSpace(state.charCodeAt(pos))){
        // -test -> wrong
        return  -1;
    }
    return pos;
}

// Search `\d+[.)][\n ]`, returns next pos after marker on success
// or -1 on fail.
int skipOrderedListMarker(StateBlock& state, size_t startLine) {
    auto [pos ,end ] = state.lineInfo[startLine].getBeginEnd();
    auto start = pos;
    // 至少两个字符 1.
    if( pos + 1 >= end) return -1;
    auto ch = state.charCodeAt(pos++);
    if(ch < 0x30 /* 0 */ || ch > 0x39 /* 9 */) return -1;

    for(;;){
        if(pos >= end) return -1;
        ch = state.charCodeAt(pos++);
        if(ch >= 0x30 /* 0 */ && ch <= 0x39 /* 9 */) {
            if( pos - start >=7) return -1; //不能是8位以上
            continue;
        }
        if (ch == 0x29/* ) */ || ch == 0x2e/* . */) //发现了结束的
            break;
        return -1;
    }
    //   v-- pos now
    // 1. abc -test -> wrong
    if(pos < end &&  !isSpace(state.charCodeAt(pos))) return  -1;
    return pos;
}

/**
 * 删除不必要的paragraph_open
 */
void markTightParagraphs(StateBlock& state, size_t idx) {
    auto level = state.level+2; //为什么level 会增加?
    for(auto i = idx+2; i < state.tokens.size() ;++i){
        if( state.tokens[i].level == level && 
                state.tokens[i].type == paragraph_open_type ){
            state.tokens[i+2].hidden = true;
            state.tokens[i].hidden = true;
            i+=2;
        }
    }
}

bool list(
        StateBlock& state,
        int startLine,
        int endLine = 0,
        bool silent = false
        )
{

    auto [pos ,end ] = state.lineInfo[startLine].getBeginEnd();
    if( state.isCodeBlock(startLine) ) return false;

    // Special case:
    //  - item 1
    //   - item 2
    //    - item 3
    //     - item 4
    //      - this one is a paragraph continuation
    if (state.listIndent >= 0 &&  // 是list
            state.lineInfo[startLine].sCount - state.listIndent >= 4 && // 超过4
            state.nest_less_blkIndent(startLine)) { //注意这里的blkIndent后面一定会被改
        return false;
    }

    //嵌入到paragraph里的list
    bool isTerminatingParagraph{false};
    if(silent && state.parentType == parentType_paragraph ){
        if( state.lineInfo[startLine].tShift >= state.blkIndent )
            isTerminatingParagraph = true; //终结了一个paragraph
    }

    
    //检查list的类型 与 marker后一个位置
    int posAfterMarker;
    bool isOrdered{false};
    std::string_view markerValue;
    if( posAfterMarker = skipOrderedListMarker(state, startLine); posAfterMarker >= 0){
        isOrdered = true;
        markerValue = state.slice(pos,posAfterMarker-1);
        if(isTerminatingParagraph && markerValue == NUMBER_ONE ) return false;
        //必须从1开始 isTerminatingParagraph 用处1
        // paragraph wrong
        // 2. 1
        // 2. 1
        // paragraph
    }
    else if( posAfterMarker = skipBulletListMarker(state, startLine); posAfterMarker >= 0){
        isOrdered = false;
    }
    else return false; //两种情况都不是

    // If we're starting a new unordered list right after
    // a paragraph, first line should not be empty.
    // 不能为空
    // paragraph
    // - 
    // paragraph 不是list
    if(isTerminatingParagraph && state.skipSpaces(posAfterMarker) >= end)
        return false;
    if( silent ) return true;
    
    std::string_view markerCharCodeView = state.slice(posAfterMarker-1,posAfterMarker);

    auto listTokIdx = state.tokens.size();

    //list open token
    if(isOrdered) {
        state.push(ordered_list_open_type,ol_type,1);
        if( markerValue != NUMBER_ONE){
            state.tokens_back().attrPush(
                    "start", std::string(markerValue));
        }
    }
    else 
        state.push(bullet_list_open_type,ul_type,1);

    state.tokens_back().map    = {startLine,0};
    //state.tokens_back().markup = markerCharCodeView;

    // 遍历 所有的项目 list items
    auto nextLine        = startLine;
    bool prevEmptyEnd    = false;
    auto terminatorRules = state.md.getBlockRules("list");
    auto oldParentType   = state.parentType;
    state.parentType     = parentType_list;

    int indentAfterMarker,
        indent;
    bool tight{true};
    

    while ( nextLine < endLine ) {
        auto pos = posAfterMarker;
        auto end = state.lineInfo[nextLine].eMarks;

        auto &LI = state.lineInfo[nextLine];
        auto offset = LI.sCount + ( posAfterMarker - (LI.first_nospace_pos()) ); //实际的缩进
        auto initial = offset;
        

        //继续缩进
        for(; pos < end ; ++pos) {
            auto ch = state.charCodeAt(pos);
            if( ch == TAB_CHAR )
                offset += 4- (offset + LI.bsCount) % 4;
            else if( ch == SPACE_CHAR)
                offset++;
            else break;
        }

        auto contentStart = pos;
        if( contentStart >= end ) //内容为空
            indentAfterMarker = 1;
        else
            indentAfterMarker = offset - initial > 4 ? 1 :offset - initial ;
        //如果超过了4个空格 后面算 code block
        
        // "  -  test"
        //  ^^^^^ - 计算这里的整个长度
        indent = initial + indentAfterMarker;

        state.push(list_item_open_type,li_type,1);
        //state.tokens_back().markup = 应该没有什么用
        state.tokens_back().map = {nextLine,0};
        //if (isOrdered) { //感觉也没有什么用
            //token.info = state.src.slice(start, posAfterMarker - 1);
        //}

        //改变现在的信息
        auto oldTight = state.tight;
        auto oldTshift = state.lineInfo[nextLine].tShift;
        auto oldsCount= state.lineInfo[nextLine].sCount;

        //  - example list
        // ^ listIndent position will be here
        //   ^ blkIndent position will be here
        //
        auto oldListIndent = state.listIndent;
        state.listIndent = state.blkIndent;
        state.blkIndent = indent;

        state.tight = true; //收缩
        state.lineInfo[nextLine].tShift = contentStart - state.lineInfo[nextLine].bMarks;
        state.lineInfo[nextLine].sCount = offset;
        if( contentStart >= end && state.isEmpty(nextLine+1) ) { //这里有点不懂,为什么只看这startLine?
            state.line = std::min(state.line+2,endLine);
        }
        else { //递归 进行block 运算
            state.md.blockTokenize(state, nextLine, endLine);
        }

        if( !state.tight || prevEmptyEnd)
            tight = false;
        prevEmptyEnd = ( state.line - nextLine) > 1 && state.isEmpty(state.line-1);

        //递归回溯
        state.blkIndent                 = state.listIndent;
        state.listIndent                = oldListIndent;
        state.tight                     = oldTight;
        state.lineInfo[nextLine].tShift = oldTshift;
        state.lineInfo[nextLine].sCount = oldsCount;

        state.push(list_item_close_type,li_type,-1);
        //state.tokens_back().markup
        nextLine = state.line;
        contentStart = state.lineInfo[nextLine].bMarks;

        //判断是否结束
        if( nextLine >= endLine) break;
        if( state.nest_less_blkIndent(nextLine)) break;
        if( state.isCodeBlock(nextLine)) break;

        bool terminate{false};
        for (const auto& e : terminatorRules) {
            if( e(state,nextLine,endLine,true)){
                terminate = true;
                break;
            }
        }
        if( terminate ) break;

        // 不同的结束符号
        if( isOrdered ){
            posAfterMarker = skipOrderedListMarker(state, nextLine);
            if( posAfterMarker < 0) break;
        }
        else {
            posAfterMarker = skipBulletListMarker(state, nextLine);
            if( posAfterMarker < 0) break;
        }
        if( uCodeChar(markerCharCodeView[0]) != state.charCodeAt(posAfterMarker-1)) //这里可能会有问题
            break;
    }

    if ( isOrdered)
        state.push(ordered_list_close_type,ol_type,-1);
    else
        state.push(bullet_list_close_type ,ul_type,-1);
    //state.tokens_back().markup
    state.line = nextLine;
    state.parentType = oldParentType;
    if( tight )
        markTightParagraphs(state, listTokIdx);

    return true;

}

}
