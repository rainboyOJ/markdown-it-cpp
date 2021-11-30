#pragma once
#include <string>

#include "../preset/token.hpp"
#include "./state_inline.hpp"

namespace markdownItCpp {


/**
 * 处理 行代代码 ``
 */
int indexOf(std::string_view& src,char c,int start){
    for( int i = start ;i< src.length(); i++)
        if( src[i] == c) return i;
    return -1;
}

bool backtick(StateInline& state,bool silent) {
    auto pos = state.pos;
    auto ch  = state.charCodeAt(pos);
    if( ch != GRAVE_CHAR ) return  false;

    auto start = pos++;
    auto end   = state.posMax;

    while ( pos < end && state.charCodeAt(pos) == GRAVE_CHAR )  pos++;
    int openLen = pos - start;

    auto marker = state.slice(start,pos);

    //如果已经扫描完了
    // 且 对应长度的 closemark 没有
    // 或 对应长度的 closemark 在自己之前
    if( state.backticksScanned && ( state.backticks.count(openLen) ==0 ? 0 : state.backticks[openLen] ) <= start) 
    {
        if( !silent ) state.pending += std::string(marker);
        state.pos += openLen;
        return true;
    }

    auto matchEnd = pos ,matchStart = pos;

    while ( (matchStart = indexOf(state.src, '`', matchEnd)) != -1) {
        matchEnd = matchStart + 1;
        while ( matchEnd < end && state.charCodeAt(matchEnd) == GRAVE_CHAR )  matchEnd++;
        auto closeLen = matchEnd - matchStart;

        if( closeLen == openLen ) {
            if(!silent) {
                state.push(code_inline_type,code_type,0);
                std::string str = std::string(state.slice(pos, matchStart));
                for (auto& e : str) if( e == '\n') e =' ';
                trim_string(str);
                state.tokens_back().content = state.md.push_content_cache(std::move(str));
            }
            state.pos = matchEnd;
            return true;
        }
        state.backticks[closeLen] = matchStart;
    }
    state.backticksScanned = true;
    if( !silent) state.pending += std::string(marker);
    state.pos += openLen;
    return true;
}

/**
 * 
 * ~~strike through~~
 */
//插入 token 到 delimiter list
bool strikethroughTokenize(StateInline& state,bool silent) {
    if(silent) return false; //不支持silent 
    auto pos = state.pos;
    auto marker = state.charCodeAt(pos);

    if( marker != TILDE_CHAR ) return false;

    int scanLen = state.scanDelims(pos, true);
    state.pos += scanLen;

    if( scanLen < 2) return false; //长度不对
    if(scanLen % 2 != 0) { // 是奇数
        state.push(text_type,emptyLine,0);
        state.tokens_back().content = tilde_char;
        scanLen-=1;
    }
    for(int i = 0; i < scanLen ; i+=2){
        auto t = state.push(text_type, emptyLine, 0);
        t.content = double_tilde_char;

        state.delimiters.push_back(delimiters_type{
                .end= -1,   //对应的结束的下标
                .jump= i/2,   //初始应该回跳多少
                .length= 0, //长度
                .marker= '~', //对应的marker
                .level= 0,
                .open= true,
                .close= true,
                .token = state.tokens.size() - 1 //对应的token的下标
                });
    }

    //state.pos += scanLen;
    return true;
}


/**
 * emphasis **blod** *倾斜*
 * 强调符号
 */

bool emphasisTokensize(StateInline& state,bool silent) {
    if(silent) return true;
    auto start = state.pos;
    auto ch = state.src[start];
    if(NoneOneOf(ch, '_','*')) return false;

    int scanLen = state.scanDelims(state.pos,true);
    for(int i=0;i<scanLen;++i){
        auto &token = state.push(text_type, emptyLine, 0);
        //token.content = underscore_char;
        token.content = (ch == '_' ? underscore_char : star_char);

        state.delimiters.push_back(delimiters_type{
                .end= -1,   //对应的结束的下标
                .jump= i,   //初始应该回跳多少
                .length= scanLen, //长度
                .marker= ch, //对应的marker
                .level= 0,
                .open= true,
                .close= true,
                .token = state.tokens.size() - 1 //对应的token的下标
                });
    }

    state.pos += scanLen;
    return true;
}

//rule2
void strikethroughPostcess(StateInline& state,std::vector<delimiters_type> & delimiters){

    for (const auto& e : delimiters) {
        if( e.marker != '~') continue;
        if(e.end == -1 ) continue;
        auto& endDelim = delimiters[e.end];


        auto& token   = state.tokens[e.token];
        token.type    = s_open_type;
        token.tag     = s_tag;
        token.nesting = 1;
        token.markup  = double_tilde_char;
        token.content = emptyLine;

        auto& token2   = state.tokens[endDelim.token];
        token2.type    = s_close_type;
        token2.tag     = s_tag;
        token2.nesting = -1;
        token2.markup  = double_tilde_char;
        token2.content = emptyLine;
    }
}

//rule2
void blance_pairs(StateInline& state,std::vector<delimiters_type> & delimiters){
    auto size = delimiters.size();
    for(int i=0;i<size;++i){
        auto &closer = delimiters[i];
        auto opener = &delimiters[i];

        if( !closer.close) continue;

        int openerIdx = i - closer.jump - 1;
        if( openerIdx < -1 )  openerIdx = -1;

        //从后向前一个一个查找
        for( ; openerIdx > -1 ; openerIdx -= opener->jump +1){
            opener = &delimiters[openerIdx];
            if( opener->marker != closer.marker) continue;
            if( opener->open && opener->end < 0 ){ //没有配对过
                int lastJump = 0 ;
                if( openerIdx > 0 && delimiters[openerIdx-1].open == false) // ?
                    lastJump = delimiters[openerIdx-1].jump+1;

                closer.jump = i-openerIdx + lastJump; // ?
                closer.open = false;

                opener->end   = i;
                opener->jump  = lastJump;
                opener->close = false;
                break;
            }
        }
    }
}


//rule2
void emphasisPostcess(StateInline& state,std::vector<delimiters_type> & delimiters){
    int size = delimiters.size();
    for(int i=size-1;i >=0;--i){
        auto &startDelim = delimiters[i];
        if(NoneOneOf(startDelim.marker, '_','*')) continue;
        if( startDelim.end == -1) continue; //没有对应
        auto &endDelim = delimiters[startDelim.end];


        // ** -> strong
        bool isStrong = i > 0 &&
            delimiters[i-1].end == startDelim.end+1 && //前一个对应 对应的后一个
            delimiters[i-1].token == startDelim.token-1 && // 对应的token 相邻
            delimiters[startDelim.end+1].token == endDelim.token+1 && // 对应的token 相邻
            delimiters[i-1].marker == startDelim.marker ; // marker 一样
        auto token     = &state.tokens[startDelim.token];
        token->type    = isStrong ? strong_open_type : em_open_type;
        token->tag     = isStrong ? strong_tag : em_tag;
        token->nesting = 1;
        token->markup  = isStrong ? double_star_char : star_char;

        token     = &state.tokens[endDelim.token];
        token->type    = isStrong ? strong_close_type : em_close_type;
        token->tag     = isStrong ? strong_tag : em_tag;
        token->nesting = -1;
        token->markup  = isStrong ? double_star_char : star_char;

        if( isStrong ){
            state.tokens[delimiters[i-1].token].hidden  = true;
            state.tokens[delimiters[startDelim.end+1].token].hidden  = true;
            i--;
        }
    }
}

}
