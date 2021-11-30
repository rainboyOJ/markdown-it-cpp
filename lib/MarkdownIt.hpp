
#pragma once

#include "define.hpp"
#include "./preset/preset.hpp"
#include "./parser_core.hpp"
#include "./parser_block.hpp"
#include "./parser_inline.hpp"
#include "./rules_core/state_core.hpp"
#include "./render.hpp"

#include "./rules_core/normarlize.hpp"

#include <memory>


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
      //std::cout << "parseend ======================" << std::endl;
      //for (const auto& e : tokens) {
          //if( e.content.length() ){
              //std::cout << std::hex << reinterpret_cast<uint64_t>(e.content.data()) << std::endl;
              //std::cout  << std::endl;
          //}
      //}
      //std::cout << "==" << std::endl;
      //for (const auto& e : content_cache) {
          //std::cout << std::hex << reinterpret_cast<uint64_t>(e->data()) << std::endl;
          //std::cout << e->c_str() << std::endl;
      //}
      
      //输出最终的token
      #ifdef DEBUG
        dbg_one(state.tokens.size());
        for (const auto& e : tokens) {
            std::cout << e ;
        }
      #endif
  }

  virtual std::string_view push_content_cache(std::string&& str) override{
      content_cache.emplace_back(std::make_shared<std::string>(std::move(str)));
      return *content_cache.back();
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
  virtual void blockTokenize(StateBlock& state,int s,int e) override {
      block.tokenize(state, s, e);
  }
  virtual void inlineParse(std::string_view src , MarkdownIt_base &md, ENV env, TokenArrayRef outTokens) override {
      m_inline.parse(src, md, env, outTokens);
  }
  virtual void inlineTokenize(StateInline& state) override {
      m_inline.tokenize(state);
  }

  std::string render(std::string_view v_src,ENV env){
      //nor
      //src = std::string(v_src);
      normalize(v_src, src);
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

  std::vector<std::shared_ptr<std::string>> content_cache;

};

} // end namespace markdownItCpp
