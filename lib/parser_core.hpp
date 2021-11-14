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

#include "./rules_core/block.hpp"


namespace markdownItCpp {


class Core {
public:
    Core(){
        ruler.push("block",block);
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
