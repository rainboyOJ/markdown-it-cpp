#pragma once
#include <string>
#include <sstream>
#include "../state_base.hpp"

namespace markdownItCpp {

bool block(state_base& state,int ,int, bool){
    if( state.inlineMode ){
        //TODO
    }
    else
        state.md.blockParse(state.src,state.md,state.env,state.tokens);
    return true;
}

}
