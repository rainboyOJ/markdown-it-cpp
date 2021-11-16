
#pragma once

#include "define.hpp"
#include "./preset/preset.hpp"
#include "./parser_core.hpp"
#include "./parser_block.hpp"
#include "./parser_inline.hpp"
#include "./rules_core/state_core.hpp"
#include "./render.hpp"


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

  void parse(std::string& src,ENV env){
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

  virtual void blockParse(std::string_view src, MarkdownIt_base &md, ENV env, TokenArrayRef outTokens) override {
      block.parse(src,md,env,outTokens);
  }
  virtual void inlineParse(std::string_view src , MarkdownIt_base &md, ENV env, TokenArrayRef outTokens) override {
      m_inline.parse(src, md, env, outTokens);
  }

  std::string render(std::string_view v_src,ENV env){
      src = std::string(v_src);
      parse(src, env);
      return m_render.render(tokens, options, env);
  }

  std::string src;
  ParserBlock block;
  ParserInline m_inline;
  Core core;
  optionsType options;
  TokenArray tokens;
  Render m_render;
};

} // end namespace markdownItCpp
