//parser_core class
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

#include "./rules_core/rules_core.hpp"

namespace markdownItCpp {


class Core {
public:
    Core(){ // 加入需要的ruler
        ruler.push("block",block);
        ruler.push("inline",Inline);
    }
    void process(StateCore& state){
        //TODO
        auto rules = ruler.getRules("");
        for (const auto& r : rules) {
            r(state);
        }
    }

public:
    Ruler<CoreFn> ruler;
};

};
