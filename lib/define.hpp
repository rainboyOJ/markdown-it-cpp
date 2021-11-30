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


class StateBlock;
using BlockFn = std::function<bool(StateBlock&,int,int,bool)>;

class StateCore;
using CoreFn = std::function<void(StateCore&)>;

struct delimiters_type {
    int  end{-1};   //对应的结束的下标
    int  jump{0};   //初始应该回跳多少
    int  length{0}; //长度
    char marker{0}; //对应的marker
    int  level{0};
    bool open{true};
    bool close{true};
    size_t  token{0}; //对应的token的下标
};

class StateInline;
using InlineFn  = std::function<bool(StateInline&,bool)>;
using InlineFn2 = std::function<void(StateInline&,std::vector<delimiters_type>&)>;


using TokenArray = std::vector<Token>;
using TokenArrayRef = std::vector<Token> &;

template<typename Render>
using default_render_ruleFn = std::function<std::string(TokenArrayRef,int,optionsType&,ENV&,const Render *)>;

struct References {
    std::string title;
    std::string href;
};

//虚基类
class MarkdownIt_base {
public:
    virtual const std::vector<BlockFn>& getBlockRules(std::string) = 0;
    virtual int getMaxNesting() = 0;
    virtual void blockParse(std::string_view,MarkdownIt_base&,ENV,TokenArrayRef) = 0;
    virtual void blockTokenize(StateBlock&,int,int) = 0;
    virtual void inlineParse(std::string_view src,MarkdownIt_base&,ENV,TokenArrayRef) = 0;
    virtual void inlineTokenize(StateInline&) = 0;
    virtual std::string_view push_content_cache(std::string&& str) = 0;

    std::unordered_map<std::string,References> references;
};

} // end namespace markdownItCpp
