#pragma once
#include <string>
#include <sstream>
#include "state_core.hpp"

namespace markdownItCpp {

void block(StateCore& state){
    if( state.inlineMode ){
        //TODO
    }
    else
        state.md.blockParse(state.src,state.md,state.env,state.tokens);
}

}
