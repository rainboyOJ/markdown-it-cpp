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

namespace markdownItCpp {
    
struct Block_rule_list {
    std::string name;
    BlockFn fn;
    std::vector<std::string> alts;
};

const std::vector<Block_rule_list> _rules {
    { "paragraph",paragraph,{}}
};


class ParserBlock {
public:
    ParserBlock()
    {
        for (const auto& e : _rules) {
            ruler.push(e.name, e.fn, e.alts);
        }
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
            if( state.sCount[line] < state.blkIndent ) break;

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
            for(int i=0;i<=rules.size()-1;++i){
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
    }

public:
    Ruler<BlockFn> ruler;
    //StateBlock state;
};

} // end namespace markdownItCpp
