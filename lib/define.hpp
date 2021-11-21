//基础的 定义
#pragma once
#include <functional>
#include <string>
#include <vector>
#include "./preset/options.hpp"
#include "token.hpp"

namespace markdownItCpp {

/*
 *struct __rule_list {
 *    std::string name;
 *    FN fn;
 *    std::vector<std::string> alts;
 *};
 */
struct ENV { };

enum NESTING {
    close = -1,
    open = 1,
    self_close = 0
};

struct delimiters_type {
    bool close{false};
    int end{-1};
    int jump{0};
    int length{2};
    int level{0};
    int marker{42};
    bool open{true};
    int token{0};
};

class StateBlock;
using BlockFn = std::function<bool(StateBlock&,int,int,bool)>;

class StateCore;
using CoreFn = std::function<void(StateCore&)>;


class StateInline;
using InlineFn = std::function<bool(StateInline&,bool)>;


using TokenArray = std::vector<Token>;
using TokenArrayRef = std::vector<Token> &;

template<typename Render>
using default_render_ruleFn = std::function<std::string(TokenArrayRef,int,optionsType&,ENV&,const Render *)>;

//虚基类
class MarkdownIt_base {
public:
    virtual const std::vector<BlockFn>& getBlockRules(std::string) = 0;
    virtual int getMaxNesting() = 0;
    virtual void blockParse(std::string_view,MarkdownIt_base&,ENV,TokenArrayRef) = 0;
    virtual void blockTokenize(StateBlock&,int,int) = 0;
    virtual void inlineParse(std::string_view src,MarkdownIt_base&,ENV,TokenArrayRef) = 0;
    virtual std::string_view push_content_cache(std::string&& str) = 0;
};

} // end namespace markdownItCpp
