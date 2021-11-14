
#pragma once
#include <string>
#include <vector>

#include "ruler.hpp"
#include "./token.hpp"
//#include "./state_base.hpp"
#include "define.hpp"

namespace markdownItCpp {


class MarkdownIt_base {
public:
    virtual const std::vector<FN>& getRules(std::string,bool core) = 0;
    virtual int getMaxNesting() = 0;
    virtual void blockParse(std::string_view,MarkdownIt_base&,ENV,TokenArrayRef) = 0;
};

} // end namespace markdownItCpp
