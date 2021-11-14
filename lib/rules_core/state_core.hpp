#pragma once
#include <string>
#include <sstream>

//#include "../MarkdownIt.hpp"
#include "../MarkdownIt_base.hpp"
#include "../token.hpp"
#include "../common/utils.hpp"

#include "../state_base.hpp"

namespace markdownItCpp {

//template<typename MarkdownIt>
//class StateCore :public state_base<MarkdownIt>{
class StateCore :public state_base{
public:
    StateCore(std::string_view src,MarkdownIt_base& md,ENV env,
            TokenArrayRef tokens
            )
      :state_base(src,md,env,tokens)
    {}
};

}
