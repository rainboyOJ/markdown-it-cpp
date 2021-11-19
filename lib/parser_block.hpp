//parser_block class
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

#include "./rules_block/state_block.hpp"
#include "./rules_block/paragraph.hpp"
#include "./rules_block/blockquote.hpp"

namespace markdownItCpp {
    

class ParserBlock {
public:
    ParserBlock()
    {
        ruler.push("blockquote", blockquote, {"paragraph", "reference", "blockquote", "list"});
        ruler.push("paragraph", paragraph);
    }


    // Generate tokens for input range
    void tokenize(StateBlock& state,int startLine ,int endLine){
        //得到所有的rule
        auto rules = ruler.getRules("");
        int line = startLine;
        bool hasEmptyLines = false;
        auto maxNesting = state.md.getMaxNesting();
        bool ok = false;

        while ( line < endLine ) {
            state.line = line = state.skipEmptyLines(line);
            if( line >= endLine ) break;

            // Termination condition for nested calls.
            // Nested calls currently used for blockquotes & lists
            //if( state.sCount[line] < state.blkIndent ) break;
            if( state.nest_less_blkIndent(line)) break;

            // If nesting level exceeded - skip tail to the end. That's not ordinary
            // situation and we should not care about content.
            if( state.level >= maxNesting){
                state.line = endLine;
                break;
            }

            // Try all possible rules.
            // On success, rule should:
            //
            // - update `state.line`
            // - update `state.tokens`
            // - return true
            for(int i=0;i<rules.size();++i){
                ok =  rules[i](state,line,endLine,false);
                if(ok) break;
            }
            state.tight = !hasEmptyLines;

            if( state.isEmpty(state.line - 1))
                hasEmptyLines = true;
            // paragraph might "eat" one newline after it in nested lists
            line = state.line;
            if( line < endLine && state.isEmpty(line) ){
                hasEmptyLines = true;
                line++;
                state.line = line;
            }
        }
    }

    void parse(std::string_view src,MarkdownIt_base& md,ENV env,
            TokenArrayRef outTokens) {
        if( src.length() == 0) return;
        StateBlock state(src,md,env,outTokens);
        tokenize(state, state.line, state.lineMax);
        #ifdef DEBUG
        dbg_one(state.tokens.size());
        for (const auto& e : state.tokens) {
            std::cout << e ;
        }
        std::cout << "===========" << std::endl;
        #endif
    }

public:
    Ruler<BlockFn> ruler;
    //StateBlock state;
};

} // end namespace markdownItCpp
