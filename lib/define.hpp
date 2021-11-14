//基础的 定义
#pragma once
#include <string>
#include <vector>

namespace markdownItCpp {

/*
 *struct __rule_list {
 *    std::string name;
 *    FN fn;
 *    std::vector<std::string> alts;
 *};
 */
struct ENV { };

class StateBlock;
using BlockFn = std::function<bool(StateBlock&,int,int,bool)>;

class StateCore;
using CoreFn = std::function<void(StateCore&)>;


class Token;
using TokenArray = std::vector<Token>;
using TokenArrayRef = std::vector<Token> &;

//虚基类
class MarkdownIt_base {
public:
    virtual const std::vector<BlockFn>& getBlockRules(std::string) = 0;
    virtual int getMaxNesting() = 0;
    virtual void blockParse(std::string_view,MarkdownIt_base&,ENV,TokenArrayRef) = 0;
};

} // end namespace markdownItCpp
