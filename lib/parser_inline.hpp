//parser_inline class
#pragma once

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <any>
#include <string_view>
#include <exception>
#include <functional>

#include "define.hpp"
#include "ruler.hpp"

#include "./rules_inline/state_inline.hpp"

#include "./rules_inline/text.hpp"
#include "./rules_inline/newline.hpp"

namespace markdownItCpp {


class ParserInline {
public:
    ParserInline(){
        ruler.push("text", text);
        ruler.push("newline",newline);
    }

    void skipToken(StateInline& state){
        bool ok{false};
        auto pos = state.pos;
        auto rules = ruler.getRules("");
        auto len = rules.size();
        if( state.cache.count(pos) !=0 ){
            state.pos = state.cache[pos];
            return;
        }
        auto maxNesting = state.md.getMaxNesting();
        if( state.level < maxNesting){
            for(int i =0;i< len ;i++){
                state.level++;
                ok = rules[i](state,true);
                state.level--;
                if(ok) break;
            }
        }
        else {
            state.pos = state.posMax;
        }
        if( !ok) state.pos++;
        state.cache[pos] = state.pos;
    }

    void tokenize(StateInline& state){
        bool ok{false};
        auto rules = ruler.getRules("");
        auto len = rules.size();
        auto end = state.posMax;
        auto maxNesting = state.md.getMaxNesting();
        while ( state.pos < end ) { // 逐个字符检查
            if( state.level < maxNesting ){
                for(int i=0;i<len;++i){
                    ok = rules[i](state,false); //执行成功就跳出for
                    if(ok) break;
                }
            }
            if( ok ) {
                if( state.pos >= end) break;
                continue; //继续
            }
            state.pending += state.src[state.pos++]; // 所有的rule都没有对这个char进行处理,加入pending
        }
        if( state.pending.length() ) //都不符合的情况下,最后还有pending
            state.pushPending();
    }

    void parse(std::string_view src,MarkdownIt_base& md,ENV env,
            TokenArrayRef outTokens) {
        StateInline state(src,md,env,outTokens);
        tokenize(state);
        auto rules = ruler2.getRules("");
        auto len = rules.size();
        for(int i=0;i< len;++i){
            rules[i](state,false);
        }
    }

public:
    Ruler<InlineFn> ruler;
    Ruler<InlineFn> ruler2;
};

};