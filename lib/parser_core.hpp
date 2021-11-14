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

#include "ruler.hpp"

#include "./rules_core/block.hpp"


namespace markdownItCpp {

class Core {
public:
    Core(){
        ruler.push("block",block,{});
    }
    void process(state_base & state){
        //TODO
        auto rules = ruler.getRules("");
        for (const auto& r : rules) {
            r(state,0,0,0);
        }
    }

    Ruler ruler;
};

};
