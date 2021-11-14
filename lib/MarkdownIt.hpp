
#pragma once

#include "define.hpp"
#include "./parser_block.hpp"
#include "./parser_core.hpp"
#include "./rules_core/state_core.hpp"
#include "./preset/preset.hpp"


namespace markdownItCpp {


class MarkdownIt :public MarkdownIt_base {
public:
  MarkdownIt()
  {
    options = { // default
      .html=false,
      .xhtmlOut = false,
      .breaks = false,
      .langPrefix = "language-",
      .linkify = false,
      .typographer = false,
      .quotes = "\u201c\u201d\u2018\u2019",
      .maxNesting = 100
    };
  }

  void parse(std::string_view src,ENV env){
      StateCore state(src,*this,env,tokens);
      core.process(state);
      #ifdef DEBUG
        dbg_one(state.tokens.size());
        for (const auto& e : tokens) {
            std::cout << e ;
        }
      #endif
  }

  virtual const std::vector<BlockFn>& getBlockRules(std::string name) override{
      return block.ruler.getRules(name);
  }
  virtual int getMaxNesting() override{
      return options.maxNesting;
  };

  virtual void blockParse(std::string_view src, MarkdownIt_base &md, ENV env, TokenArrayRef outTokens) {
      block.parse(src,md,env,outTokens);
  }

  ParserBlock block;
  Core core;
  optionsType options;
  TokenArray tokens;
};

} // end namespace markdownItCpp
